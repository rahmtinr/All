/*
 * Reviews.h
 *
 *  Created on: Oct 1, 2014
 *      Author: rahmtin
 */

#ifndef REVIEWS_H_
#define REVIEWS_H_

#include<cstdio>
#include<iostream>

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
	int index;
	friend ostream& operator << (ostream& out, const Review &review);
	bool operator < (const Review &other) const { // this is based on time!
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

ostream& operator << (ostream& out, const Review &review) {
	out << "ProductId: " << review.product_id << endl;
	out << "UserId: " << review.user_id << endl;
	out << "ProfileName: " << review.profile_name << endl;
	out << "Helpfulness: " << review.helpfulness << endl;
	out << "Score: " << review.score << endl;
	out << "Time: " << review.time << endl;
	out << "Summary: " << review.summary << endl;
	out << "Text: " << review.text << endl;
	return out;
}

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
