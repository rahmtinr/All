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
#include<sstream>
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
	int current_experience_level; // how many reviews written before this one
	int final_experience_level; // how many reviews written.
	string current_exp_best_author;
	string final_exp_best_author;
	vector<string> authors;
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
		if(user_id == other.user_id) {
			stringstream ss(text);
			stringstream sss(other.text);
			int diff = 0;
			int total = 0;
			string temp1, temp2;
			while(!ss.eof() && !sss.eof()) {
				ss >> temp1;
				sss >> temp2;
				total++;
				if(temp1 != temp2) {
					diff++;
				}
			}
			while(!ss.eof()) {
				total++;
				diff++;
				ss >> temp1;
			}
			while(!sss.eof()) {
				total++;
				diff++;
				sss >> temp2;
			}
			if((double)diff / total < 0.1) { // true means they are equal
				return true;
			}
			return false;
		}
		return false;
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
