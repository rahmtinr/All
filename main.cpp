#include<algorithm>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cmath>
#include<ctime>
#include<fstream>
#include<iostream>
#include<map>
#include<set>
#include<sstream>
#include<string>
#include<vector>

#include "Aggregation.h"
#include "AmazonUtility.h"
#include "Innovations.h"
#include "MyUtility.h"
#include "ProductRelated.h"
#include "ReviewReader.h"
#include "Reviews.h"
#include "Seasonal.h"
#include "UserRelated.h"
#include "Videos.h"
using namespace std;


vector<Review> reviews;
map<string, vector<int>* > words_states;
// Langauge innovations
vector<pair<string, vector<Review> > >  innovations[100];
map<int,int> distribution_for_entire_data_set;
set<string> jon_innovation;
string MODE;

void initialize(string s) {
	MODE = "";
	int last_slash = 0, last_dot = 0;
	for(int i = s.length() - 1; i >=0; i--) {
		if(s[i] == '/' && last_slash == 0) {
			last_slash = i;
		}
		if(s[i] == '.' && last_dot == 0) {
			last_dot = i;
		}

	}
	Global::NAMEOFDATASET = s.substr(last_slash + 1 ,
			last_dot - last_slash - 1) + MODE;
	cerr << Global::NAMEOFDATASET <<endl;
	Innovations::numbers_for_appearances = {4 , 8, 10};
	Innovations::numbers_for_products = {3 , 6, 10};
	Innovations::numbers_for_authors = {3, 5, 7};
	Amazon::Global::min_year = 2015;
	Amazon::Global::max_year = 1995;
	Amazon::Global::earliest.year = 2015;
	Amazon::Global::state_coeffecient = 2;
	Amazon::Global::probability_of_state_change = 0.1;
	Innovations::numbers_for_appearances = {4 , 8, 10};
	Innovations::numbers_for_products = {3 , 6, 10};
	Innovations::numbers_for_authors = {3, 5, 7};
	Amazon::Global::min_year = 2015;
	Amazon::Global::max_year = 1995;
	Amazon::Global::earliest.year = 2015;
	Amazon::Global::state_coeffecient = 10;
	Amazon::Global::probability_of_state_change = 0.1;
	Amazon::Global::threshold_for_innovation = 10;

}

int main(int argc, char *argv[]) {
	initialize(argv[1]);
	ifstream fin(argv[1]);
	// Read input.
	while (true) {
		if (!ReadOneReview(fin, &reviews)) {
			break;
		}
		Amazon::Global::min_year = min(Amazon::Global::min_year, reviews[reviews.size() - 1].time.year);
		Amazon::Global::max_year = max(Amazon::Global::max_year, reviews[reviews.size() - 1].time.year);
		Amazon::Global::earliest = min(Amazon::Global::earliest, reviews[reviews.size() - 1].time);
		Amazon::Global::latest = max(Amazon::Global::latest, reviews[reviews.size() - 1].time);
	}
	//	cerr << reviews.size() <<endl;
	//	MyFilter("text", "turbotax");
	//	MyFilter("text", "hr block", &reviews);
	//	MyFilter("text", "taxact", &reviews);
	cerr << reviews.size() << endl;
	sort(reviews.begin(), reviews.end(), CompareReviewOnUserId);
	reviews.erase(unique( reviews.begin(), reviews.end(), ReviewEquality), reviews.end());
	cerr << reviews.size() << endl;
	/**/
	// CountMonthlyAccumulatedReviews(&reviews);
	// CountYearlyReviews(&reviews);
	// PerItemPerMonth(&reviews);
	// PerItemPerYear(&reviews);
	// Top products.
	// int size_of_list = 40;
	// TopProducts(size_of_list, &reviews);
	// Video Average vs All average.
	// ReviewsWithVideo(&reviews);
	// StarAveragePerYear(&reviews);
	// Time in Day is useless! The timestamp is on a daily basis
	// StarAveragePerTimeInTheDay(&reviews);
	//
	// sort(reviews.begin(), reviews.end());
	// Innovations::LearnDictionary(0, reviews.size() / 2, &reviews);
	// Innovations::FindInnovations(reviews.size() / 2, &reviews, innovations); // returns pair of word and review it was started.
	// Innovations::AnalyseInnovation(innovations, &reviews);
	Innovations::FindBursts(&words_states, &reviews);
	for(pair<string,vector<int> *> word_states : words_states) {
		string word = word_states.first;
		vector<int> states = *(word_states.second);
		int longest_one = 0;
		int current = 0;
		for(int i = 0; i < (int) states.size(); i++) {
			if(states[i] == 1) {
				current++;
			} else {
				current = 0;
			}
			longest_one = max(longest_one, current);
		}
		if(longest_one >= states.size() / Amazon::Global::threshold_for_innovation + 25) {
			jon_innovation.insert(word);
			cerr << word << endl;
		}
	}
	// UserDistributionBasedOnNumberOfReviews(&reviews, &distribution_for_entire_data_set);
	/**/
	//	UserAngrinessBasedOnNumberOfReviews(&reviews);
	//	StarAveragePerMonth(&reviews);
	//	StarAveragePerMonthAccumulatedOverYears(&reviews);
	return 0;
}
