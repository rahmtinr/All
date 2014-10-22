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
#include<iostream>
#include<sstream>
#include<string>
#include<vector>

using namespace std;

const static string month[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};

class Global {
public:
	static string NAMEOFDATASET;
};
string Global::NAMEOFDATASET;


class MyTime {
public:
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int weekday;
	int epoch_time;

	MyTime(){}
	friend ostream &operator<<(ostream&, const MyTime&);
	MyTime(struct tm* time){
		year = time->tm_year+1900;
		month = time->tm_mon;
		day = time->tm_mday;
		hour = time->tm_hour;
		minute = time->tm_min;
		second = time->tm_sec;
		weekday = time->tm_wday;
		epoch_time = mktime(time);
	}

	bool operator < (const MyTime &other) const {
		if(year == other.year) {
			if(month == other.month) {
				return day < other.day;
			}
			return month < other.month;
		}
		return year < other.year;
	}

	int Day(MyTime earliest_day){
		int temp = abs(earliest_day.epoch_time - epoch_time);
		return temp/(60*60*24);
	}
};

ostream& operator <<(ostream& out, const MyTime& my_time){
	out << my_time.month + 1 << "/" << my_time.day << "/" << my_time.year<< "   ";
	out << my_time.hour << ":" << my_time.minute << ":" << my_time.second;
	return out;
}
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

double FindAverage(vector<double> *data) {
	if (data->size() == 0) {
		return 0;
	}
	double average = 0;
	for (int i = 0; i < (int)data->size(); i++) {
		average += (*data)[i];
	}
	return average / data->size();
}

double FindConfidenceInterval95(vector<double> *data, double average) {
	if (data->size() == 0) {
		return 0;
	}
	double error = 0;
	for (int i = 0; i < (int)data->size(); i++) {
		error += ((*data)[i]-average) * ((*data)[i]-average);
	}

	error /= data->size();
	error /= data->size();
	return 1.96*sqrt(error);
}

string ToString(int x) {
	stringstream ss;
	string ret;
	ss << x;
	ss >> ret;
	return ret;
}

#endif /* MYUTILITY_H_ */
