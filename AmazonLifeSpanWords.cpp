/*
 * AmazonLifeSpanWords.cpp
 *
 *  Created on: May 27, 2015
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


vector<Review> reviews;
set<WordTimeLine> words_states;
vector<WordTimeLine> top_innovations;
// Langauge innovations
vector<pair<string, vector<Review> > >  innovations[100];
map<int,int> distribution_for_entire_data_set;
map<string, int> experience_level;
set<WordTimeLine> burst_innovation;
string MODE;

int SIZE_OF_TOP_INNOVATIONS;
string filename;
string burst_mode;
string real_time;
map<string, int> earliest, latest;
void initialize(char *argv[]) {
	filename = argv[1];
	burst_mode = argv[2];
	real_time = argv[3];
	{
		string temp(argv[4]);
		if(temp == "DocRatio") {
			Amazon::Global::state_machine_doc_ratio = true;
		} else {
			Amazon::Global::state_machine_doc_ratio = false;
		}
	}
	{
		string temp(argv[6]);
		if(temp == "final") {
			Amazon::Global::final = true;
		} else {
			Amazon::Global::final = false;
		}
	}
	{
		string temp(argv[7]);
		if(temp == "4") {
			Amazon::Global::denominator = 4;
		} else {
			Amazon::Global::denominator = 2;
		}
		cerr << "denominator: " << " " << temp << " " << Amazon::Global::denominator << endl;
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
	cerr << "Final = " << argv[6] << " " << Amazon::Global::final << endl;
	// Preprocess
	int bound = 20 * 1000 * 1000;
	Amazon::Global::sum_ln.push_back(0);
	Amazon::Global::sum_ln.push_back(0);
	for(int i = 2; i < bound; i++) {
		Amazon::Global::sum_ln.push_back(Amazon::Global::sum_ln.back() + log(i));
	}
	SIZE_OF_TOP_INNOVATIONS = 500;

	// Set +INF and -INF for dates
	Amazon::Global::min_year = 2030;
	Amazon::Global::max_year = 1995;
	Amazon::Global::earliest.year = 2030;

	//State machine parameters.
	Amazon::Global::state_coeffecient = SimpleStringToDouble(argv[5]);
	Amazon::Global::probability_of_state_change = 0.1;
	Amazon::Global::threshold_for_innovation = 3;
	//	Amazon::Global::state_machine_doc_ratio = false;
	// Remove unknown reviews
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

}

int main(int argc, char *argv[]) {
	if(argc != 8) {
		cerr << "The number of arguments is not correct! Force quitting." << endl;
		return 0;
	}
	initialize(argv);
	ifstream fin(argv[1]);
	bool reddit = false;
	if(Global::NAMEOFDATASET.substr(0, 6)== "reddit") {
		reddit = true;
	}
	// Read input.
	while (true) {
		if(reddit == false) {
			if (!ReadOneReview(fin, &reviews)) {
				break;
			}
		} else {
			if(!ReadOneRedditReview(fin,&reviews)) {
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
	for(int i = 0; i < (int)reviews.size(); i++) {
		// This constant change should be same as the one that we used in finding the innovations
		reviews[i].time.day = reviews[i].time.epoch_time / (24 * 60 * 60) - (25 * 365); // Setting the starting point to 25 * 365 days after Jan 1, 1970. The first review is in 1997 anyways
		// Bucket a week
		reviews[i].time.day /= 7;
	}
	map<string, int> innovation_words;
	bool cristians_innovations = false;
	if(Amazon::Global::state_coeffecient < 1.01) { // if the coeff passed to the function is less than one it is cristian's innovation and the number is the fraction used for learning the dictionary
		cristians_innovations = true;
	}
	{
		string s;
		int x;
		string filename;
		if(cristians_innovations == false) {
			filename = Amazon::Global::output_directory + "words_start_burst_coeff_" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
		} else {
			filename = Amazon::Global::output_directory + "words_start_burst_cristian_0.05_dict.txt";
		}
		ifstream fin_innovation_best_burst(filename.c_str());

		while(fin_innovation_best_burst >> s >> x) {
			if(s == "") {
				continue;
			}
			WordTimeLine word_time_line;
			word_time_line.word = s;
			innovation_words[s] = top_innovations.size();
			word_time_line.burst_start = x;
			top_innovations.push_back(word_time_line);
		}
	}
	const int CUT_OFF_EXP = 10;
	for(Review review : reviews) {
		if(review.final_experience_level < CUT_OFF_EXP) {
			continue;
		}
		stringstream ss(review.text);
		string s;
		while(!ss.eof()) {
			ss >> s;
			if(innovation_words.find(s) == innovation_words.end()) {
				continue;
			}
			if(earliest.find(s) == earliest.end()){
				earliest[s] = review.time.day;
			}
			latest[s] = review.time.day;
		}
	}
	int counter = 0;
	string filename = Amazon::Global::output_directory + "words_min_life_span_start_burst_coeff_" +
			SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";

	ofstream filtered_words_fout(filename.c_str());
	for(int i = 0; i < (int)top_innovations.size(); i++) {
		cerr << latest[top_innovations[i].word] << " " << earliest[top_innovations[i].word] << endl;
		if(latest[top_innovations[i].word] - earliest[top_innovations[i].word] > 52 * 8) { // a life cycle of at least 4 year
			filtered_words_fout << top_innovations[i].word << " " << top_innovations[i].burst_start << endl;
			counter++;
		}
	}
	return 0;
}
