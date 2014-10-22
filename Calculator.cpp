/*
    Contributors:
        Satinder Singh
        Marc Latore
	Enter an expression in inFix notation for calculations.
  Programs allows a debugging option to show user Polish Notation.
*/

#include <iostream>
#include <cctype>
#include <cmath>
#include <limits> // to place QNAN into array
#include <string>
#include <stack>
#include <sstream>
#include <vector>
using namespace std;

string toString(double d);
bool isOpenParen(char ch);
bool isCloseParen(char ch);
bool isProper(char ch);
vector<string> toVector(string s, bool &error);
bool isParen(vector <string> s);
bool CorrectParen(vector<string> s, int depth, char last);
int precedence(char ch);
bool isNum(string s);
void readOp(vector<string> &v, string s, bool &error);
void readNum(vector<string> &v, string s, bool&error);


bool isOpenParen(char ch){ return ch == '(' || ch == '{' || ch == '[';}

bool isCloseParen(char ch){ return ch == ')' || ch == '}' || ch == ']';}

bool isBinary(char ch) { return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '^' || ch == '=';}  

bool isProper(char ch) {return isBinary(ch) || isCloseParen(ch) || isOpenParen(ch); }

void readOp(vector<string> &v, string s, bool &error){
	if( s.empty() ) return;
	if( isCloseParen(s[0]) ){
		v.push_back(s.substr(0,1));
		readOp(v, s.substr(1), error);
	}
	else if( isBinary(s[0]) ){
		v.push_back(s.substr(0,1));
		readNum(v,s.substr(1), error);
	}
	else{
		error = true;
		return;
	}
}

void readNum(vector<string> &v, string s, bool&error){
	if(s.empty()) {error = true; return;}
	if (isOpenParen(s[0])){
		v.push_back(s.substr(0,1));
		readNum(v, s.substr(1), error);
	}else if (islower(s[0])){
		v.push_back(s.substr(0,1));
		readOp(v, s.substr(1), error);
	}else {
		int oneMore = 0;
		if (s[0] == '.') oneMore = 1;
		stringstream ss(s);
		double d;
		if(! (ss>>d) ) {error = true; return;}
		string temp = toString(d);
		v.push_back(temp);
		readOp(v, s.substr(temp.size() - oneMore), error);
	}
}

vector<string> toVector(string s, bool &error){
	vector<string> v;
	int period = 0;
	int equal = 0;
	for(unsigned int i=0; i < s.length(); i++){
		if(s[i] == ' ') s.erase(i--,1); 
		else if( !( islower(s[i]) || isdigit(s[i]) || isProper(s[i]) || s[i] == '.')) {
				error = true;
				return v;
		}
		else if( i+1 < s.length() && s[i] == s[i+1] && isBinary(s[i]) && s[i] != '-'){ // Example: 3++8
			error = true;
			return v;
		}
		else if( i+2 < s.length() && islower(s[i]) && isBinary(s[i+1]) && s[i+2] == '=' ){ // Example: a+=4
			error = true;
			return v;
		}
		else if( isdigit(s[i]) || s[i] == '.' ){ // Example: look for 2.3.5 + 3.4.5
			for(unsigned int j=i, period = 0;  j < s.length() && (isdigit(s[j]) || s[j] == '.'); j++ )
			{
				if(s[j] == '.' && ++period > 1) {
					error = true;
					return v;
				}
			}
		}
		else if( s[i] == '=' && ++equal > 1){ // Example: 2=3=3
			error = true;
			return v;
		}
	}

	readNum(v, s, error);
	return v;
}
bool isParen(vector <string> s){
	for (unsigned int i=0; i<s.size(); i++){
		if (isOpenParen(s[i][0]) || isCloseParen(s[i][0])) return true;
	}
	return false;
}
bool CorrectParen(vector<string> s, int depth=0, char last=' ')
{
	if( s.empty() ) return true;
	for(unsigned int i=0; i < s.size(); i++){
		if( isOpenParen(s[i][0]) ){
			depth++;
		}
		else if( isCloseParen(s[i][0]) ){
			depth--;
			if(s[i][0] == '}') last = '}';
			else if(s[i][0] == ']') last = ']';
			else if(s[i][0] == ')') last = ')';
		}

		if(depth == 0){
			if((s[0][0] == '{' && last == '}' ) || (s[0][0] == '[' && last == ']') || (s[0][0] == '(' && last == ')')){
				s.erase(s.begin()+i);
				s.erase(s.begin());
				return CorrectParen(s); // delete character i and delete s[0]
			}
			return false;
		}
	}
	return false; 
}

int precedence(char ch){
	if(ch == '^') return 3;
	if(ch == '*' || ch == '/' || ch == '%') return 2;
	return 1;
}

bool isNum(string s) {
	int size = s.length();
	if( s[0] == '-' && s.length() == 1) return false;
	for(int i=0; i < size; i++)
		if( !isdigit(s[i]) && s[i] != '.' && s[i] != '-' ) return false; // Example: could be negative 5
	return true;
}

bool isLetter(string s){ return s.size() == 1 && islower(s[0]);}
bool isBinary(string s) { return s.size() == 1 && isBinary(s[0]);}
bool compare(string s, char c) {return s.size() == 1 && s[0] == c;}
double toDouble(string s){
     double d;
     stringstream ss(s); 
     ss >> d;
     return d;
}

string toString(double d){
	stringstream ss;
	ss << d;
	return ss.str();
}

double mod(double y, double x){
	if(y<0) y=-y;
	if (x>0){
		while(x>y) x-=y;
	}else{
		y=-y;
		while(x<y) x-=y;
	}return x;
}
vector<string> inPost(vector<string> s, bool &error, bool verbose = true){
	vector<string> v;

	if(isParen(s)){  
		vector<string> temp = s;
		for(unsigned int i=0; i < temp.size(); i++){
			if( !(isOpenParen(temp[i][0]) || isCloseParen(temp[i][0])) )
				temp.erase(temp.begin() + i--);
		}
		error = !CorrectParen(temp); // only works if only but parenthesis
		if(error) return v;
	}
	int stLength = s.size();
	stack <char> stk;
	for(int i=0; i < stLength; i++){
		string temp = s[i];
		if( isNum(temp) || isLetter(temp) )
			v.push_back(temp);
		else if( isOpenParen( temp[0] ) ) stk.push(temp[0]);
		else if( isBinary(s[i]) ){
			while( !stk.empty() && !isOpenParen(stk.top()) && precedence(temp[0]) <= precedence(stk.top()) ){
				string temp2 = " ";
				temp2[0]=	stk.top();
				v.push_back(temp2);
				stk.pop();
			}
			stk.push(temp[0]);
		}
		else if( isCloseParen(temp[0]) ){
			while(!isOpenParen(stk.top()) ){
				temp = stk.top();   
				v.push_back(temp);	 
				stk.pop();
			}
			stk.pop();
		}
	}
	while(!stk.empty()){
		string temp = " ";
		temp[0] = stk.top();
		v.push_back( temp );
		stk.pop();
	}
	return v;
}

double evaluate(vector<string> v, double letters[], bool&error, bool showPost){
	if (error) return 0;
	char saveto = '0';//saveto will stay 0 until the end
	if (v.size() > 1 && v[1] == "="){
		if (islower(v[0][0])){
			saveto = v[0][0];
			v.erase(v.begin(),v.begin()+2);
		}else{
			error = true; return 0;
		}
	}//now we have a shorter expression if we need to store it.

	v = inPost(v,error);
	if(error) return 0;
	if(showPost){
		for(unsigned int i =0; i < v.size(); i++)
			cout << v[i] << ' ';
		cout << endl;
	}
	stack<double> stk;
	while (!v.empty()){
		if (isLetter(v[0]))
			stk.push(letters[v[0][0]-'a']);
		else if ( isNum(v[0]) )
			stk.push( toDouble(v[0]) );
		else if (isBinary(v[0])){    
			double num1, num2;
			if (stk.size() < 2){
				error = true; 
				return 0;
			}
			else{
				num1 = stk.top();
				stk.pop();
				num2 = stk.top();
				stk.pop();
				if(v[0][0] == '+') stk.push(num1+num2);
				else if(v[0][0] == '-') stk.push(num2-num1);
				else if(v[0][0] == '*') stk.push(num1*num2);
				else if(v[0][0] == '%') stk.push(mod(num1,num2));
				else if(v[0][0] == '^') stk.push(pow(num2,num1));
				else stk.push(num2/num1);
			}
		}
		else{
			error =true;
			return 0;
		}
		v.erase(v.begin());
	}// here the vector is empty and a numeric result to the expression is at the top of the stack
	if (islower(saveto)){
		letters[saveto-'a'] = stk.top();
	}
	return stk.top();
}

int main(){
	double letters[26];
	string exp;
	bool error;
	for(int i=0; i < 26; i++) 
		letters[i] = numeric_limits<double>::quiet_NaN( ); // fill array with NAN

	char showPost; 
	cout << "Would you like Debugging Mode to Show PostFix? (y or n): ";
	cin >> showPost;
	cin.ignore();
	do{
		cout << "\n> ";
		getline(cin,exp);
		if (exp == "") break;
		error = false;
		vector <string> v = toVector(exp, error);
		double result = evaluate (v, letters, error, showPost == 'y');
		if( error ) cout << "illegal expression";
		else{
			cout << result;
		}
		v.clear();
	}while (exp != "");
	return 0;
}

