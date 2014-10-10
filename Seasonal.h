/*
 * Seasonal.h
 *
 *  Created on: Oct 3, 2014
 *      Author: rahmtin
 */

#ifndef SEASONAL_H_
#define SEASONAL_H_

#include<algorithm>
#include<iostream>
#include<fstream>
#include<map>
#include<vector>

#include "MyUtility.h"
#include "Reviews.h"

using namespace std;


void SeasonalTopWordsTitle(vector<Review> *reviews) {
	// Seasonal variations, string to count of the word in that season
	map<string, int> words_in_winter, words_in_summer;
	// Count to the word so we can sort them
	vector<pair<int, string> > words_in_winter_vec, words_in_summer_vec;
	// Number of words in different seasons
	int summer_count, winter_count;
	for (Review review : *reviews) {
		string word;
		if (review.time.month == 0 || review.time.month == 1 || review.time.month == 11) { // winter
			winter_count++;
			stringstream ss(review.product_title);
			while (ss.good()) {
				ss >> word;
				words_in_winter[SimpleRemoveAnySymbol(word)]++;
			}
		}
		if (review.time.month == 5 || review.time.month == 6 || review.time.month == 7) { // summer
			summer_count++;
			stringstream ss(review.product_title);
			while (ss.good()) {
				ss >> word;
				words_in_summer[SimpleRemoveAnySymbol(word)]++;
			}
		}
	}
//	cout << winter_count << "  " << summer_count << endl;
	for ( auto x : words_in_summer) {
		words_in_summer_vec.push_back(make_pair(x.second, x.first));
	}
	for ( auto x : words_in_winter) {
		words_in_winter_vec.push_back(make_pair(x.second, x.first));
	}
	sort(words_in_summer_vec.begin(), words_in_summer_vec.end());
	sort(words_in_winter_vec.begin(), words_in_winter_vec.end());
	reverse(words_in_summer_vec.begin(), words_in_summer_vec.end());
	reverse(words_in_winter_vec.begin(), words_in_winter_vec.end());
	ofstream summer_top_words("../Output_All/summer_top_words_" + Global::NAMEOFDATASET +".txt");
	ofstream winter_top_words("../Output_All/winter_top_words_" + Global::NAMEOFDATASET +".txt");
	for (auto x : words_in_summer_vec ) {
		summer_top_words << x.first << " " << x.second <<endl;
	}

	for (auto x : words_in_winter_vec ) {
		winter_top_words << x.first << " " << x.second <<endl;
	}
}



#endif /* SEASONAL_H_ */
