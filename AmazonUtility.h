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

enum BurstMode{ MAXBURST, LONGBURST, ALL};

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
	static bool real_time;
	static BurstMode burst_mode;
};
}

class WordTimeLine {
public:
	string word;
	double *alpha;
	vector<int> *timeline;
	vector<int> *states;
	vector<MyTime> *dates;
	vector<int> *review_index;
	double opt_cost, static_cost;
	double difference;
	int burst_start;
	int burst_end;
	WordTimeLine() {
		timeline = new vector<int>();
		states = new vector<int>();
		dates = new vector<MyTime>();
		review_index = new vector<int>();
		alpha = new double[2];
		static_cost = 0;
		opt_cost = 0;
		difference = 0;
		burst_start = 0;
		burst_end = 0;
	}
	bool operator < (const WordTimeLine &other) const {
		if (difference == other.difference)
			return word < other.word;
		return difference > other.difference;
	}

	void Benefit() {
		if(static_cost < opt_cost) {
			cout << "FUCK " <<  word << " " << static_cost << " " << opt_cost << endl;
		}
//		cerr << static_cost <<" " << opt_cost << endl;
		difference = max(static_cost - opt_cost, difference);
	}

	void CalculateCosts(int starter, int len) {
			double p = Amazon::Global::probability_of_state_change;
			static_cost = 0;
			if(starter != 0) {
				opt_cost = 2* log(p/(1-p));
			}
			for(int i = starter; i < starter + len - 1; i++) {
				int gap = (*timeline)[i + 1] - (*timeline)[i];
				if((*states)[i + 1] != (*states)[i]) {
					opt_cost += log(p/(1-p));
				}
				opt_cost += (-1) * (log(alpha[(*states)[i]]) - alpha[(*states)[i]] * gap);
				static_cost += (-1) * (log(alpha[0]) - alpha[0] * gap);
			}
			Benefit();
			if(EqDouble(difference, static_cost - opt_cost)) {
				burst_start = starter;
				burst_end = starter + len;
			}
		}

};

int Amazon::Global::min_year;
int Amazon::Global::max_year;
MyTime Amazon::Global::earliest;
MyTime Amazon::Global::latest;
double Amazon::Global::state_coeffecient;
double Amazon::Global::probability_of_state_change;
int Amazon::Global::threshold_for_innovation;
BurstMode Amazon::Global::burst_mode;
bool Amazon::Global::real_time;

#endif /* AMAZONUTILITY_H_ */
