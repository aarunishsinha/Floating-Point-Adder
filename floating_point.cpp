#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <bitset>

using namespace std;

string input_arr [1000];											// An array to store in input strings.

// Shifts the input string by n bits to the right.
string shift_right(string frac, int n){
	string out = frac;
	for(int i = 0; i<n; i++){
		out.pop_back();
		out = "0" + out;
	}
	return out;
}

// Takes two binary strings(which may or maynot be of same length) and adds them. 
string addBinary(string a, string b) {
	string result = "";
	int s = 0;
	int i = a.size() - 1, j = b.size() - 1;
	while(i >= 0 || j >=0 || s == 1) {
		s += ((i >= 0)? a[i] - '0': 0); 
        s += ((j >= 0)? b[j] - '0': 0);
        result = char(s % 2 + '0') + result;
        s /= 2;
        i--;
        j--;
	}
	return result;
}

// Given a binary string and returns it's 2's complement in the form of another string.
string twoComplement(string str) {
	int n = str.length();
	int i;
	for (i = n-1 ; i >= 0 ; i--) 
        if (str[i] == '1') 
            break;
    if (i == -1) 
        return '1' + str;
    for (int k = i-1 ; k >= 0; k--) 
    { 
        if (str[k] == '1') 
            str[k] = '0'; 
        else
            str[k] = '1'; 
    }
    return str;
}

// Takes the fraction and exponent bit strings of the output and normalises them by either shilfting left or right.
void normalise(string& out_frac, string& out_exponent, long long int& out_exp) {
	char * pEnd;
	while(out_frac[0] == '0') {
		out_frac = out_frac.substr(1, (out_frac.length() - 1));
		out_frac = out_frac + "0";
		out_exp--;
	}
	out_exponent = bitset<8>(out_exp).to_string();
	if(out_frac.length() > 23) {
		out_exponent = addBinary(out_exponent, "1");
		out_exp = strtoull(out_exponent.c_str(), &pEnd, 2);
	}
}

// Takes the fraction part of the output binary string and rounds it.
void roundOff(string& out_frac) {
	if(out_frac.length() > 23){
		if(out_frac[out_frac.length()-1] == '1'){
			out_frac.pop_back();
			out_frac = addBinary(out_frac, "1");
		}
		else {
			out_frac.pop_back();
		}
	}
}

int main(int argc, char* argv[]) {
	ifstream infile;
	infile.open(argv[1]);
	string str;

	int cycle_cnt = 0;
	
	int index = 0;

	while(getline(infile, str)) {												// Read from the input file and store the strings in an array.
		input_arr[index] = str;
		index++;
	}

	for(int i = 0; i < index; i++){
		string input = input_arr[i];
		string f1, f2, signbit1, signbit2, exponent1, exponent2, fraction1, fraction2;
		char * pEnd;
		int except = 0;
		// Parsing the input to get the two input floats to be added.
		int len = input.length();
		char str[len+1];
		strcpy(str, input.c_str());
		char *token = strtok(str, " ");
		f1 = token;
		token = strtok(NULL, " ");
		f2 = token;

		signbit1 = f1.substr(0,1);
		exponent1 = f1.substr(1,8);
		fraction1 = f1.substr(9,24);
		signbit2 = f2.substr(0,1);
		exponent2 = f2.substr(1,8);
		fraction2 = f2.substr(9,24);

		cycle_cnt++;															// STEP 1

		while((exponent1).compare(exponent2) != 0) {
			long long int exp1, exp2;
			exp1 = strtoull(exponent1.c_str(), &pEnd, 2);						// Converts a binary string into a long long int.
			exp2 = strtoull(exponent2.c_str(), &pEnd, 2);
			if (exp1 > exp2) {													// If exponent of the second floating point number is smaller then shift right until their exponents are same.
				fraction2 = shift_right(fraction2, (exp1-exp2));
				exp2 = exp1;
				exponent2 = exponent1;
			}
			else {																// If exponent of the first floating point number is smaller then shift right until their exponents are same.
				fraction1 = shift_right(fraction1, (exp2-exp1));
				exp1 = exp2;
				exponent1 = exponent2;
			}
		}
		cycle_cnt++;															// STEP 2 - Adding the significands.

		string out_frac, out_signbit;											// Sign Bit and the Fraction part of the output floating point.

		if((signbit1 == "0") && (signbit2 == "0")) {							// If sign bits are 0 then simply add the fraction parts and set the output sign bit to be 0.
			out_frac = addBinary(fraction1, fraction2);
			out_signbit = "0";
		}
		else if((signbit1 == "1") && (signbit2 == "0")) {						// If the sign bits are different, convert the negative floating point to it's 2's complement and add them
			string sign_frac1 = "0" + fraction1;
			sign_frac1 = twoComplement(sign_frac1);
			string sign_frac2 = "0" + fraction2;
			string added = addBinary(sign_frac1, sign_frac2);
			if(added.length() == 25) {
				added = added.substr(1, (added.length() - 1));
			}
			int n_len = added.length();
			if(added[0] == '0') {
				out_frac = added.substr(1,(n_len - 1));
				out_signbit = "0";
			}
			else{
				out_frac = twoComplement(added).substr(1, (n_len - 1));
				out_signbit = "1";
			}
		}
		else if((signbit1 == "0") && (signbit2 == "1")) {
			string sign_frac1 = "0" + fraction1;
			string sign_frac2 = "0" + fraction2;
			sign_frac2 = twoComplement(sign_frac2);
			string added = addBinary(sign_frac1, sign_frac2);
			if(added.length() == 25) {
				added = added.substr(1, (added.length() - 1));
			}
			int n_len = added.length();
			if(added[0] == '0') {
				out_frac = added.substr(1,(n_len - 1));
				out_signbit = "0";
			}
			else{
				out_frac = twoComplement(added).substr(1, (n_len - 1));
				out_signbit = "1";
			}
		}
		else{																	// If both are negative then unsigned addition of the fractional part and set sign bit of output as 1.	
			out_frac = addBinary(fraction1, fraction2);
			out_signbit = "1";
		}

		cycle_cnt++;															// STEP 3 - Normalise the output.

		string out_exponent = exponent1;										// Exponent of the output floating point.
		long long int out_exp = strtoull(out_exponent.c_str(), &pEnd, 2);
		
		normalise(out_frac, out_exponent, out_exp);								// Normalising the output.

		long long int powerTwo = out_exp - 127;

		cout<<out_frac<<endl;

		if(powerTwo > 127) {													// Checking of overflow and underflow
			cout<<"Exception : OVERFLOW"<<endl;
			except = 1;
		}
		else if (powerTwo < -126) {
			cout<<"Exception : UNDERFLOW"<<endl;
			except = 1;
		}
		else {
			cycle_cnt++; 														// STEP 4 - Rounding the output floating point to required number of bits.
			roundOff(out_frac);
			while(out_frac.length() > 23) {											// If after rounding of the floating point is denormalised then repeat from Step 3.
			normalise(out_frac, out_exponent, out_exp);
			cycle_cnt++;

			out_exp = strtoull(out_exponent.c_str(), &pEnd, 2);

			long long int powerTwo = out_exp - 127;

			if(powerTwo > 127) {
				cout<<"Exception : OVERFLOW"<<endl;
				except = 1;
			}
			else if (powerTwo < -126) {
				cout<<"Exception : UNDERFLOW"<<endl;
				except = 1;
			}
			else {
				cycle_cnt++; 
				roundOff(out_frac);
			}
		}
		}

		
		if(except != 1){ 
			cout<<(out_signbit + out_exponent + out_frac)<<endl;
		}
	}
	cout<<"Total Cycle Count : ";
	cout<<cycle_cnt<<endl;
}