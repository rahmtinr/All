/*
 * MyUtility.h
 *
 *  Created on: Oct 1, 2014
 *      Author: rahmtin
 */

#ifndef MYUTILITY_H_
#define MYUTILITY_H_

#define FAIL false
#define SUCCESS true
#include<cmath>
using namespace std;

const string month[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};

class MyTime {
public:
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int weekday;

	MyTime(){}
	void print() {
		cout<< "Time: " << month + 1 << "/" << day << "/" << year<< "   ";
		cout<< hour << ":" << minute << ":" << second <<endl;
	}
	MyTime(struct tm* time){
		year = time->tm_year+1900;
		month = time->tm_mon;
		day = time->tm_mday;
		hour = time->tm_hour;
		minute = time->tm_min;
		second = time->tm_sec;
		weekday = time->tm_wday;
	}
};

int TimeDifferenceInMonth(MyTime first, MyTime second) {
	int first_month = first.year * 12 + first.month;
	int second_month = second.year * 12 + second.month;
	return abs(first_month - second_month);
}

string SimpleRemoveAnySymbol(string input) {
	string result = "";
	for (int i = 0; i <(int) input.size(); i++) {
		if(isalpha(input[i]) || input[i] =='\''){
			result += input[i];
		}
	}
	return result;
}

//returns -1 if denominator is equal to 0.
double SimpleStringFractionToDouble(string input) {
	stringstream ss(input);
	double numerator, denominator;
	char ch;
	ss >> numerator >> ch >> denominator;
	if (denominator == 0){
		return -1;
	}
	return numerator / denominator;
}

double SimpleStringToDouble(string input) {
	stringstream ss(input);
	double ret;
	ss >> ret;
	return ret;
}


bool EqDouble(double x, double y) {
	if(abs(x-y) < 1e-6) {
		return true;
	}
	return false;
}

string SimpleToLower(string s) {
	for (int i = 0; i < (int)s.size(); i++) {
		s[i] = tolower(s[i]);
	}
	return s;
}



#endif /* MYUTILITY_H_ */
