/*
 * Empty2009.cpp
 *
 *  Created on: Sep 13, 2015
 *      Author: Rahmtin
 */

#include<algorithm>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cmath>
#include<ctime>
#include<fstream>
#include<iostream>
#include<iomanip>
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


vector<Review> reviews, after_2009_reviews;
set<WordTimeLine> words_states;
vector<WordTimeLine> top_innovations;
// Langauge innovations
vector<pair<string, vector<Review> > >  innovations;
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
	string temp(argv[4]);
	if(temp == "DocRatio") {
		Amazon::Global::state_machine_doc_ratio = true;
	} else {
		Amazon::Global::state_machine_doc_ratio = false;
	}
	int last_slash = -1, last_dot = 0;
	for(int i = filename.length() - 1; i >=0; i--) {
		if(filename[i] == '/' && last_slash == -1) {
			last_slash = i;
		}
		if(filename[i] == '.' && last_dot == 0) {
			last_dot = i;
		}

	}
	Global::NAMEOFDATASET = filename.substr(last_slash + 1 ,
			last_dot - last_slash - 1);

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

	// Preprocess
	int bound = 20 * 1000 * 1000;
	Amazon::Global::sum_ln.push_back(0);
	Amazon::Global::sum_ln.push_back(0);
	for(int i = 2; i < bound; i++) {
		Amazon::Global::sum_ln.push_back(Amazon::Global::sum_ln.back() + log(i));
	}

	Innovations::numbers_for_appearances = {4 , 8, 10};
	Innovations::numbers_for_products = {3 , 6, 10};
	Innovations::numbers_for_authors = {3, 5, 7};

	// Set +INF and -INF for dates
	Amazon::Global::min_year = 2030;
	Amazon::Global::max_year = 1995;
	Amazon::Global::earliest.year = 2030;

	//State machine parameters.
	Amazon::Global::state_coeffecient = SimpleStringToDouble(argv[5]);
	Amazon::Global::probability_of_state_change = 0.1;
	Amazon::Global::threshold_for_innovation = 3;
	//	Amazon::Global::state_machine_doc_ratio = false;
	//Remove unknown reviews
	Amazon::Global::remove_unknown = true;

	//Output Directory

	Amazon::Global::output_directory = "../Output_All/"  + Global::NAMEOFDATASET + "_bursts/" + real_time + "/" + burst_mode + "/" + Global::NAMEOFDATASET + "_";

	//Read StopWords
	ifstream fin_stop_words("stopwords.txt");
	string s;
	while(getline(fin_stop_words, s)) {
		if(s == "") {
			continue;
		}
		stop_words.insert(s);
	}
	{
		string temp(argv[6]);
		if(temp == "Bigram") {
			Amazon::Global::bigram = true;
		} else {
			Amazon::Global::bigram = false;
		}
	}
}

int main(int argc, char *argv[]) {
	if(argc != 7) {
		cerr << "The number of arguments is not correct! Force quitting." << endl;
		return 0;
	}
	initialize(argv);
	ifstream fin(argv[1]);
	// Read input.
	bool reddit = false;
	if(Global::NAMEOFDATASET.substr(0, 6) == "reddit") {
		reddit = true;
	}
	while (true) {
		string filename(argv[1]);
		if(reddit == true) {
			if(!ReadOneRedditReview(fin, &reviews)) {
				break;
			}
		} else {
			if (!ReadOneReview(fin, &reviews)) {
				break;
			}
		}
		if(reviews.size() == 0) {
			continue;
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
	 */

	if(EqDouble(9.0, Amazon::Global::state_coeffecient))
	{ // No country for old members paper
		map<string, long long> word_freq;
		map<string, int> word_burst_start_date;
		int common_divider = 20;
		Innovations::LearnDictionary(0, reviews.size() / common_divider, &reviews);
		Innovations::FindCristianInnovations(reviews.size() / common_divider, &reviews, &innovations); // returns pair of word and the review it was started.
		string filename = Amazon::Global::output_directory + "words_start_burst_cristian_0.05_dict.txt";
		ofstream fout_cristian_method(filename.c_str());
		cerr << "# of innovations: " << innovations.size() << endl;
		for(int i = 0; i < (int)innovations.size(); i++) {
			int week_that_burst_started = (((innovations[i].second)[0]).time.epoch_time / (24 * 60 * 60) - (25 * 365)) / 7;
			word_burst_start_date[innovations[i].first] = week_that_burst_started;
			word_freq[innovations[i].first] = 0;
		}
		for(int i = 0; i < (int)reviews.size(); i++) {
			stringstream ss(reviews[i].text);
			string s;
			while(!ss.eof()) {
				ss >> s;
				if(word_freq.find(s) == word_freq.end()) {
					continue;
				}
				word_freq[s]++;
			}
		}
		vector<pair<long long, string> > freq_word;
		for(pair<string, long long> p : word_freq) {
			freq_word.push_back(make_pair(p.second, p.first));
		}
		sort(freq_word.begin(), freq_word.end());
		int starting_point = max(0, (int)freq_word.size() - 500);
		for(int i = starting_point; i < (int)freq_word.size(); i++) {
			string word = freq_word[i].second;
			fout_cristian_method << word << " " << word_burst_start_date[word] << endl;
		}
		// Innovations::AnalyseInnovation(innovations, &reviews);
	}

	if(Amazon::Global::state_machine_doc_ratio == true) { // need to change the time only by day and forget about the year
		for(int i = 0; i < (int)reviews.size(); i++) {
			reviews[i].time.day = reviews[i].time.epoch_time / (24 * 60 * 60) - (25 * 365); //setting the starting point to 25 * 365 days after Jan 1, 1970. The first review is in 1997 anyways
			reviews[i].time.day /= 7;
			// Bucket a week
			if(reviews[i].time.day > 52 * 14)  {
				after_2009_reviews.push_back(reviews[i]);
			}
		}
		reviews = after_2009_reviews;
        after_2009_reviews.clear();
		Innovations::FindBurstsDocumentRatio(&words_states, &reviews);
	} else {
		Innovations::FindBurstsTimeDifference(&words_states, &reviews);
	}
    cerr << "size after filtering " << reviews.size() << endl;
	for(WordTimeLine word_states : words_states) {
		string word = word_states.word;
		vector<bool> states = *(word_states.states);
		int longest_one = 0;
		int current = 0;
		int best_start = 0;
		for(int i = 0; i < (int) states.size(); i++) {
			if(states[i] == 1) {
				current++;
			} else {
				if(Amazon::Global::burst_mode == MAXBURST) { // Looking for best interval
					word_states.CalculateCosts(i - current, current, &reviews);
				}
				if (longest_one == current) {
					best_start = i - current;
				}
				current = 0;
			}
			longest_one = max(longest_one, current);
		}
		if(Amazon::Global::state_machine_doc_ratio == false) {
			if(longest_one >= (int)states.size() / Amazon::Global::threshold_for_innovation + 5
					|| (longest_one > 100 && longest_one >= (int)states.size() / (3 * Amazon::Global::threshold_for_innovation))){

				if(Amazon::Global::burst_mode == LONGBURST) { // Longest burst difference
					word_states.CalculateCosts(best_start, longest_one, &reviews);
				}

				if(Amazon::Global::burst_mode == ALL) {  // Entire difference
					word_states.CalculateCosts(0, states.size(), &reviews);
				}
				burst_innovation.insert(word_states);
			}
		}
		if(Amazon::Global::state_machine_doc_ratio == true && longest_one > 8 && word_states.review_index->size() > 100){
			burst_innovation.insert(word_states);
		}
	}
	string bigram_string = "words_empty2009_";
	if(Amazon::Global::bigram == true) {
		bigram_string = "bigrams_empty2009_";
	}
	string filename = Amazon::Global::output_directory +  bigram_string + "start_burst_coeff_" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
	ofstream innovation_burst_year_out(filename.c_str());

	string filename2 = Amazon::Global::output_directory + bigram_string + "start_burst_freq_coeff_" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
	ofstream innovation_burst_year_freq_out(filename2.c_str());

    cerr << "YAAAAAAAAAH     " << burst_innovation.size() << endl;

    int killed1 = 0, killed2 = 0;
	for(WordTimeLine word_time_line : burst_innovation) {
		if(word_time_line.burst_start >	 52 * 16) { // burst should be after start of 2010
			bool check = false;
			long long temp = 0;
			for(int i = 0; i < (int)word_time_line.review_index->size(); i++) {
				int index = (*word_time_line.review_index)[i];
				if(reviews[index].time.day < 52 * 15) {
					temp++;
				} else {
					if(50 * temp < (int)word_time_line.review_index->size()) { // less than 2 percent should be in 2010
						check = true;
					} else {
//                        cerr << word_time_line.word << " " << temp << "  " << word_time_line.review_index->size() << endl;
                    }
					break;
				}
			}
			if(check == true) {
				innovation_burst_year_out << word_time_line.word << "   " << word_time_line.burst_start << endl;
				innovation_burst_year_freq_out << word_time_line.word << "   " << word_time_line.burst_start << " " << word_time_line.review_index->size() << endl;
			} else { 
                killed2++;
            }
		} else {
            killed1++;
        }
	}
    cerr << "Killed: " << killed1 << " " << killed2 << endl;
	cerr << "trying to finish it!" << endl;
	return 0;
}
