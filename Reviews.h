/*
 * Reviews.h
 *
 *  Created on: Oct 1, 2014
 *      Author: rahmtin
 */

#ifndef REVIEWS_H_
#define REVIEWS_H_

#include<iostream>
#include<cstdio>
#include "MyUtility.h"

using namespace std;

class Review {
public:
	string product_id;
	string product_title;
	string price;
	string user_id;
	string profile_name;
	string helpfulness;
	string score;
	MyTime time;
	string summary;
	string text;

	void print() {
		cout<< "ProductId: " << product_id << endl;
		cout<< "UserId: " << user_id << endl;
		cout<< "ProfileName: " << profile_name << endl;
		cout<< "Helpfulness: " << helpfulness << endl;
		cout<< "Score: " << score << endl;
		time.print();
		cout<< "Summary: " << summary << endl;
		cout<< "Text: " << text << endl;
	}
	bool operator < (const Review &other) const {
		if (time.year != other.time.year) {
			return time.year < other.time.year;
		}
		if (time.month != other.time.month) {
			return time.month < other.time.month;
		}
		if (time.day != other.time.day) {
			return time.day < other.time.day;
		}
		return product_id < other.product_id;
	}
	bool operator == (const Review &other) const {
		return (product_id == other.product_id && user_id == other.user_id && text == other.text);
	}
};

bool CompareReviewOnUserId(const Review &x,const Review &y) {
	if(x.user_id == y.user_id) {
		return x.text < y.text;
	}
	return x.user_id < y.user_id;
}


bool ReviewEquality(const Review &x, const Review &y) {
	return (x.user_id == y.user_id && x.text == y.text);
}


#endif /* REVIEWS_H_ */
