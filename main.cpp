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
set<WordTimeLine> words_states;
vector<WordTimeLine> top_innovations;
// Langauge innovations
vector<pair<string, vector<Review> > >  innovations[100];
map<int,int> distribution_for_entire_data_set;
map<string, int> experience_level;
set<WordTimeLine> burst_innovation;
string MODE;


string filename;
string burst_mode;
string real_time;

void initialize(char *argv[]) {
	filename = argv[1];
	burst_mode = argv[2];
	real_time = argv[3];
	MODE = "";
	int last_slash = 0, last_dot = 0;
	for(int i = filename.length() - 1; i >=0; i--) {
		if(filename[i] == '/' && last_slash == 0) {
			last_slash = i;
		}
		if(filename[i] == '.' && last_dot == 0) {
			last_dot = i;
		}

	}
	Global::NAMEOFDATASET = filename.substr(last_slash + 1 ,
			last_dot - last_slash - 1) + MODE;

	if(burst_mode == "Longest") {
		Amazon::Global::burst_mode = LONGBURST;
	} else if(burst_mode == "MaxBenefit") {
		Amazon::Global::burst_mode = MAXBURST;
	} else if(burst_mode == "All"){
		Amazon::Global::burst_mode = ALL;
	} else {
		cerr << "Second argument did not match! Force quitting." <<endl;
		exit(0);
	}

	if(real_time == "RealTime"){
		Amazon::Global::real_time = true;
	} else if (real_time == "ReviewTime") {

	} else {
		cerr << "Third argument did not match! Force quitting." <<endl;
		exit(0);
	}

	cerr << Global::NAMEOFDATASET <<endl;

	Innovations::numbers_for_appearances = {4 , 8, 10};
	Innovations::numbers_for_products = {3 , 6, 10};
	Innovations::numbers_for_authors = {3, 5, 7};

	// Set +INF and -INF for dates
	Amazon::Global::min_year = 2030;
	Amazon::Global::max_year = 1995;
	Amazon::Global::earliest.year = 2030;

	//State machine parameters.
	Amazon::Global::state_coeffecient = 10;
	Amazon::Global::probability_of_state_change = 0.1;
	Amazon::Global::threshold_for_innovation = 3;



}

int main(int argc, char *argv[]) {
	if(argc != 4) {
		cerr << "The number of arguments is not correct! Force quitting." << endl;
		return 0;
	}
	initialize(argv);
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
	cerr << Amazon::Global::earliest << endl;
	//	cerr << reviews.size() <<endl;
	//	MyFilter("text", "turbotax");
	//	MyFilter("text", "hr block", &reviews);
	//	MyFilter("text", "taxact", &reviews);
	cerr << reviews.size() << endl;
	sort(reviews.begin(), reviews.end(), CompareReviewOnUserId);
	reviews.erase(unique( reviews.begin(), reviews.end()), reviews.end());
	cerr << reviews.size() << endl;
	sort(reviews.begin(), reviews.end());
	reviews[0].index = 0;
	for(int i = 1; i < (int) reviews.size(); i++) {
		if(reviews[i].time == reviews[i-1].time) {
			reviews[i].index = reviews[i-1].index;
		} else {
			reviews[i].index = i;
		}
	}
	for(int i = 0; i < (int)reviews.size(); i++){
		if(experience_level.find(reviews[i].user_id) == experience_level.end()) {
			experience_level[reviews[i].user_id] = 0;
		}
		reviews[i].current_experience_level = experience_level[reviews[i].user_id];
		experience_level[reviews[i].user_id]++;
	}
	for(int i = 0; i < (int) reviews.size(); i++) {
		reviews[i].final_experience_level = experience_level[reviews[i].user_id];
	}
/*
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
*/
	Innovations::FindBursts(&words_states, &reviews);
	for(WordTimeLine word_states : words_states) {
		string word = word_states.word;
		vector<int> states = *(word_states.states);
		int longest_one = 0;
		int current = 0;
		int best_start = 0;
		for(int i = 0; i < (int) states.size(); i++) {
			if(states[i] == 1) {
				current++;
			} else {
				if(Amazon::Global::burst_mode == MAXBURST) { // Looking for best interval
					word_states.CalculateCosts(i - current, current);
				}
				if (longest_one == current) {
					best_start = i - current;
				}
				current = 0;
			}
			longest_one = max(longest_one, current);
		}
		if(longest_one >= (int)states.size() / Amazon::Global::threshold_for_innovation + 5
				|| (longest_one > 200 && longest_one >= (int)states.size() / (3 * Amazon::Global::threshold_for_innovation))){


			if(Amazon::Global::burst_mode == LONGBURST) { // Longest burst difference
				word_states.CalculateCosts(best_start, longest_one);
			}

			if(Amazon::Global::burst_mode == ALL) {  // Entire difference
				word_states.CalculateCosts(0, states.size());
			}
			burst_innovation.insert(word_states);
		}
	}
	ofstream fout("../Output_All/" + Global::NAMEOFDATASET + "_bursts/" + real_time + "/" + burst_mode + "/timeline.txt");
	cerr << "Size of innovations found: " << burst_innovation.size() << endl;
	int x = 0;
	for(WordTimeLine word_time_line : burst_innovation) {
		x++;
		if (x == 50) {
			break;
		}
		string word = word_time_line.word;
		vector<int> times = *(word_time_line.timeline);
		vector<int> states = *(word_time_line.states);
		top_innovations.push_back(word_time_line);
		for(int j=0; j < (int)times.size(); j++) {
			fout << word << " " << times[j] << " " << j+1  << " " << states[j] << " " << (*word_time_line.dates)[j].year + (*word_time_line.dates)[j].month/(double)12 << endl;
		}
	}
	map<string, vector<Review>*> innovators_reviews;
	ofstream innovators_out("../Output_All/" + Global::NAMEOFDATASET + "_bursts/" + real_time + "/" + burst_mode + "/distribution.txt");
	Innovations::FindInnovationsBursts(&reviews, &top_innovations, &innovators_reviews);
	for(auto p : innovators_reviews) {
		innovators_out << p.first << " " << p.second->size() << endl;
		for(Review review : *(p.second)) {
			innovators_out << review.current_experience_level << " " << review.final_experience_level << endl;
		}
		innovators_out << endl << endl << endl;
	}

	// UserDistributionBasedOnNumberOfReviews(&reviews, &distribution_for_entire_data_set);
	/**/
	//	UserAngrinessBasedOnNumberOfReviews(&reviews);
	//	StarAveragePerMonth(&reviews);
	//	StarAveragePerMonthAccumulatedOverYears(&reviews);
	cerr << "trying to finish it!" << endl;
	return 0;
}
