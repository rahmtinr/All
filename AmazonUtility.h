/*
 * AmazonUtility.h
 *
 *  Created on: Oct 1, 2014
 *      Author: rahmtin
 */

#ifndef AMAZONUTILITY_H_
#define AMAZONUTILITY_H_

#include<cmath>
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
	static MyTime earliest;
	static MyTime latest;
	static double state_coeffecient;
	static double probability_of_state_change;
	static int threshold_for_innovation;
};
}

class WordTimeLine {
public:
	string word;
	double alpha[2];
	vector<int> *timeline;
	vector<int> *states;
	vector<MyTime> *dates;
	double opt_cost, static_cost;
	double difference;
	WordTimeLine() {
		timeline = new vector<int>();
		states = new vector<int>();
		dates = new vector<MyTime>();
		static_cost = 0;
		opt_cost = 0;
		difference = 0;
	}
	bool operator < (const WordTimeLine &other) const {
		if (difference == other.difference)
			return word < other.word;
		return difference > other.difference;
	}

	void Benefit() {
		difference = static_cost - opt_cost;
	}

	void CalculateCosts() {
		opt_cost = 0;
		static_cost = 0;
		double p = Amazon::Global::probability_of_state_change;
		for(int i = 1; i < (int)states->size(); i++) {
			int gap = (*timeline)[i] - (*timeline)[i-1];
			opt_cost += (-1) * (log(alpha[(*states)[i]]) - 1 * alpha[(*states)[i]] * gap);
			if((*states)[i] != (*states)[i-1]) {
				opt_cost += log(p/(1-p));
			}
			static_cost += (-1) * (log(alpha[0]) - 1 * alpha[0] * gap);
		}
		Benefit();
	}

	void CalculateCosts(int starter, int len) {
			opt_cost = 0;
			static_cost = 0;
			double p = Amazon::Global::probability_of_state_change;
			for(int i = starter; i < starter + len; i++) {
				int gap = (*timeline)[i] - (*timeline)[i-1];
				opt_cost += (-1) * (log(alpha[(*states)[i]]) - 1 * alpha[(*states)[i]] * gap);
				if((*states)[i] != (*states)[i-1]) {
					opt_cost += log(p/(1-p));
				}
				static_cost += (-1) * (log(alpha[0]) - 1 * alpha[0] * gap);
			}
			Benefit();
		}

};

int Amazon::Global::min_year;
int Amazon::Global::max_year;
MyTime Amazon::Global::earliest;
MyTime Amazon::Global::latest;
double Amazon::Global::state_coeffecient;
double Amazon::Global::probability_of_state_change;
int Amazon::Global::threshold_for_innovation;
#endif /* AMAZONUTILITY_H_ */
