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
	static double threshold_for_innovation;
	static bool real_time;
	static BurstMode burst_mode;
	static bool remove_unknown;
	static string output_directory;
	static bool state_machine_doc_ratio;
	static vector<double> sum_ln;
	static bool final;
	static int denominator;
};
}

class WordTimeLine {
public:
	string word;
	double *alpha;
	vector<bool> *states;
	vector<int> *review_index;
	double difference;
	int burst_start;
	int burst_end;
	WordTimeLine() {
		states = new vector<bool>();
		review_index = new vector<int>();
		alpha = new double[2];

		difference = 0;
		burst_start = 0;
		burst_end = 0;
	}
	bool operator < (const WordTimeLine &other) const {
		if (difference == other.difference)
			return word < other.word;
		return difference > other.difference;
	}


	void CalculateCosts(int starter, int len, vector<Review> *reviews) {
		double opt_cost = 0, static_cost = 0;
		double p = Amazon::Global::probability_of_state_change;
		static_cost = 0;
		if(Amazon::Global::state_machine_doc_ratio == false) {
			if(starter != 0) {
				opt_cost = 2* log(p/(1-p));
			}
			for(int i = starter; i < starter + len - 1; i++) {
				int after;
				int before;
				if(Amazon::Global::real_time == true) { // Gap is based on real time
					after = (*reviews)[(*(review_index))[i+1]].time.Day(Amazon::Global::earliest);
					before = (*reviews)[(*(review_index))[i]].time.Day(Amazon::Global::earliest);
				} else { // Gap is based on the review number
					// The index here is the day by day index not their index in the reviews array
					after = (*reviews)[(*(review_index))[i+1]].index;
					before = (*reviews)[(*(review_index))[i]].index;
				}
				int gap = after - before;
				if((*states)[i + 1] != (*states)[i]) {
					opt_cost += log(p/(1-p));
				}
				opt_cost += (-1) * (log(alpha[(*states)[i]]) - alpha[(*states)[i]] * gap);
				static_cost += (-1) * (log(alpha[0]) - alpha[0] * gap);
			}
			difference = max(static_cost - opt_cost, difference);
			if(EqDouble(difference, static_cost - opt_cost)) {
				burst_start = starter;
				burst_end = starter + len;
			}
		}
	}

};

int Amazon::Global::min_year;
int Amazon::Global::max_year;
MyTime Amazon::Global::earliest;
MyTime Amazon::Global::latest;
double Amazon::Global::state_coeffecient;
double Amazon::Global::probability_of_state_change;
double Amazon::Global::threshold_for_innovation;
BurstMode Amazon::Global::burst_mode;
bool Amazon::Global::real_time;
bool Amazon::Global::remove_unknown;
string Amazon::Global::output_directory;
bool Amazon::Global::state_machine_doc_ratio;
bool Amazon::Global::final;
vector<double> Amazon::Global::sum_ln;
int Amazon::Global::denominator;

#endif /* AMAZONUTILITY_H_ */
