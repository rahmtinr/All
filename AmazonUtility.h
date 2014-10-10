/*
 * AmazonUtility.h
 *
 *  Created on: Oct 1, 2014
 *      Author: rahmtin
 */

#ifndef AMAZONUTILITY_H_
#define AMAZONUTILITY_H_

#include<vector>

#include "Reviews.h"

using namespace std;

void MyFilter(string key, string value, vector<Review> *reviews) {
	vector<Review> filter_reviews;
	filter_reviews.clear();
	for ( int i = 0; i < (int)reviews->size(); i++){
		if (key == "productId") {
			if ((*reviews)[i].product_id == value) {
				filter_reviews.push_back((*reviews)[i]);
			}
		}
		if (key == "product_title") {
			if ((*reviews)[i].product_title.find(value) != string::npos) {
				filter_reviews.push_back((*reviews)[i]);
			}
		}
		if (key == "text") { // not having the word makes it get in the reviews
			if((*reviews)[i].text.find(value) == string::npos) {
				filter_reviews.push_back((*reviews)[i]);
			}
		}
	}
	*reviews = filter_reviews;
}


class Product {
public:
	string product_id;
	int count;
	bool operator < (const Product &other) const {
		return count > other.count;
	}
};

class VoteRatio {
public:
	int helpful;
	int all;
	VoteRatio() {
		helpful =0;
		all = 0;
	}
};

namespace Amazon {
	class Global {
	public:
		static int min_year;
		static int max_year;
	};
}

int Amazon::Global::min_year;
int Amazon::Global::max_year;
#endif /* AMAZONUTILITY_H_ */
