/*
 * DBLP_main.cpp
 *
 *  Created on: Dec 22, 2014
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
#include "DBLPComponents.h"
#include "DBLPReader.h"
#include "Innovations.h"
#include "MyUtility.h"
#include "ProductRelated.h"
#include "ReviewReader.h"
#include "Reviews.h"
#include "Seasonal.h"
#include "UserRelated.h"
#include "Videos.h"
using namespace std;

vector<DBLPRecord> records;
vector<Review> reviews;
map<string, int> author_id;
map<int, string> rev_author_id;
set<pair<int, int> >  edges;
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
	int last_slash = -1, last_dot = 0;
	for(int i = filename.length() - 1; i >=0; i--) {
		if(filename[i] == '/' && last_slash == -1) {
			last_slash = i;
		}
		if(filename[i] == '.' && last_dot == 0) {
			last_dot = i;
		}

	}
	real_time = "RealTime";
	Global::NAMEOFDATASET = "DBLP";

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

	Innovations::numbers_for_appearances = {4 , 8, 10};
	Innovations::numbers_for_products = {3 , 6, 10};
	Innovations::numbers_for_authors = {3, 5, 7};

	// Set +INF and -INF for dates
	Amazon::Global::min_year = 2030;
	Amazon::Global::max_year = 1900;
	Amazon::Global::earliest.year = 2030;
	Amazon::Global::real_time = true;

	//State machine parameters.
	Amazon::Global::state_coeffecient = 3;
	Amazon::Global::probability_of_state_change = 0.1;
	Amazon::Global::threshold_for_innovation = 3;
	string temp(argv[3]);
	if(temp == "DocRatio") {
		Amazon::Global::state_machine_doc_ratio = true;
	} else {
		Amazon::Global::state_machine_doc_ratio = false;
	}

	//Remove unknown reviews
	Amazon::Global::remove_unknown = true;

	// PreProcess
	int bound = 2 * 1000 * 1000;
	Amazon::Global::sum_ln.push_back(0);
	Amazon::Global::sum_ln.push_back(0);
	for(int i = 2; i < bound; i++) {
		Amazon::Global::sum_ln.push_back(Amazon::Global::sum_ln.back() + log(i));
	}

	//Output Directory
	if(Amazon::Global::state_machine_doc_ratio == true) {
		Amazon::Global::output_directory = "../Output_All/DBLP/Bursts/DocRatio/" + Global::NAMEOFDATASET + "_";
	} else {
		Amazon::Global::output_directory = "../Output_All/DBLP/Bursts/Appearance/" + Global::NAMEOFDATASET + "_";
	}
}

int main(int argc, char *argv[]) {
	if(argc != 4) {
		cerr << "The number of arguments is not correct! Force quitting." << endl;
		return 0;
	}
	initialize(argv);
	ifstream fin(argv[1]);
	{
		int counter = 1;
		// Read input.
		while (true) {
			if (!ReadOneRecord(fin, &records)) {
				break;
			}
			if(records.size() == 0) {
				continue;
			}
			Review review;
			DBLPRecord record = records.back();
			review.time.year = 0;
			review.time.month = 0;
			// Changing years to days
			review.time.day = (record.year - 1935); // 1936 is the first paper so we change

			review.time.epoch_time = (record.year - 1935) * 60 * 60 * 24;
			review.text = record.title;
			review.product_title = record.venue;
			review.product_id = record.venue;
			review.helpfulness = "0/1";
			reviews.push_back(review);
			Amazon::Global::min_year = min(Amazon::Global::min_year, record.year);
			Amazon::Global::max_year = max(Amazon::Global::max_year, record.year);

			for(int i = 0 ; i < (int)record.authors.size(); i++) {
				if(author_id.find(record.authors[i]) == author_id.end()) {
					author_id[record.authors[i]] = counter ++;
					rev_author_id[counter - 1] = record.authors[i];
				}
				int x = author_id[record.authors[i]];
				for (int j = i + 1; j < (int)record.authors.size(); j++) {
					if(author_id.find(record.authors[j]) == author_id.end()) {
						author_id[record.authors[j]] = counter++;
						rev_author_id[counter - 1] = record.authors[j];
					}
					int y = author_id[record.authors[j]];
					if(y < x) {
						swap(x,y);
					}
					edges.insert(make_pair(x,y));
				}
			}
		}
	}
	cerr << "Min year: " << Amazon::Global::min_year << endl;
	cerr << "Max year: " << Amazon::Global::max_year << endl;
	cerr << "Number of edges: " << edges.size() << endl;
	cerr << "Number of authors: " << author_id.size() << endl;
	Components(&edges, author_id.size() + 1);

	sort(reviews.begin(), reviews.end());
	reviews[0].index = 0;
	for(int i = 1; i < (int) reviews.size(); i++) {
		if(reviews[i].time == reviews[i-1].time) {
			reviews[i].index = reviews[i-1].index;
		} else {
			reviews[i].index = i;
		}
	}
	/*
	// TODO what do you define as the experience level for a paper with a lot of authors!
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
	*/
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
	cerr << "Review size: " << reviews.size() << endl;
	if(Amazon::Global::state_machine_doc_ratio == true) {
		Innovations::FindBurstsDocumentRatio(&words_states, &reviews);
	} else {
		Innovations::FindBurstsTimeDifference(&words_states, &reviews);
	}
	for(WordTimeLine word_states : words_states) {
		string word = word_states.word;
		vector<bool> states = *(word_states.states);
		int longest_one = 0;
		int current = 0;
		for(int i = 0; i < (int) states.size(); i++) {
			if(states[i] == 1) {
				current++;
			} else {
				if(Amazon::Global::burst_mode == MAXBURST) { // Looking for best interval
					word_states.CalculateCosts(i - current, current, &reviews);
				}
				current = 0;
			}
			longest_one = max(longest_one, current);
		}
		if(Amazon::Global::state_machine_doc_ratio == false) {
			if(longest_one >= (int)states.size() / Amazon::Global::threshold_for_innovation + 5
					|| (longest_one > 100 && longest_one >= (int)states.size() / (3 * Amazon::Global::threshold_for_innovation))){
				burst_innovation.insert(word_states);
			}
		}
		if(Amazon::Global::state_machine_doc_ratio ==true && longest_one > 3){
			burst_innovation.insert(word_states);
		}
		if(word == "quantum") {
			cerr << "quantum" << endl;
			for(int i = 1; i < (int) states.size(); i++) {
				cerr << (i - 1) + 1935 << "     "  << states[i] << endl;
			}
		}
		if(word == "algorithm") {
			cerr << "ALG:" << endl;
			for(int i = 1; i < (int) states.size(); i++) {
				cerr << (i - 1) + 1935 << "     "  << states[i] << endl;
			}
		}
	}
	ofstream fout(Amazon::Global::output_directory + "timeline.txt");
	cerr << "Size of innovations found: " << burst_innovation.size() << endl;
	int x = 0;
	for(WordTimeLine word_time_line : burst_innovation) {
		//		cerr << word_time_line.word << " " << word_time_line.difference << endl;
		x++;
		if (x == 1000) {
			break;
		}
		string word = word_time_line.word;
		vector<MyTime> dates;
		vector<int> times;
		for(int index : *(word_time_line.review_index)) {
			if(Amazon::Global::real_time == true) { // Gap is based on real time
				times.push_back(reviews[index].time.Day(Amazon::Global::earliest));
			} else { // Gap is based on the review number
				// The index here is the day by day index not their index in the reviews array
				times.push_back(reviews[index].index);
			}
			dates.push_back(reviews[index].time);
		}

		for(int index : *(word_time_line.review_index)) {
			dates.push_back(reviews[index].time);
		}

		vector<bool> states = *(word_time_line.states);
		top_innovations.push_back(word_time_line);

		if(Amazon::Global::state_machine_doc_ratio == true) {
			for(int j = 0; j < (int)states.size(); j++) {
				fout << word << " " << 0 << " " << j+1  << " " << states[j] << " " << j + 1935 << endl;
			}
		} else {
			for(int j = 0; j < (int)states.size(); j++) {
				fout << word << " " << times[j] << " " << j+1  << " " << states[j] << " " << dates[j].year + dates[j].month/(double)12 << endl;
			}

		}
	}
/*	map<string, vector<Review>*> innovators_reviews;
	ofstream innovators_out(Amazon::Global::output_directory + "distribution.txt");
	Innovations::FindInnovationsBursts(&reviews, &top_innovations, &innovators_reviews);
	int num_of_innovation_reviews = 0;
	double upvotes_of_reviews = 0;
	double fraction_helpfulness = 0;
	map<string, int> innovator_ids;

	map<int, int> pdf_current_experience;
	map<int,int> pdf_final_experience;
	int sum_cdf = 0;

	{ // Innovation present
		for(auto p : innovators_reviews) {
			bool first = false;
			for(Review review : *(p.second)) {
				if(first == false)
					innovator_ids[review.user_id] ++;
				first = true;
				innovators_out << review.current_experience_level << " " << review.final_experience_level << endl;
				num_of_innovation_reviews ++;
				upvotes_of_reviews += SimpleStringToDouble(review.helpfulness);
				fraction_helpfulness += SimpleStringFractionToDouble(review.helpfulness);
				pdf_current_experience[review.current_experience_level] ++;
				pdf_final_experience[review.final_experience_level] ++;
			}
		}
		ofstream innovators_cdf_out(Amazon::Global::output_directory + "innovator_present_cdf.txt");
		innovators_cdf_out << (--pdf_current_experience.end())->first + 1 << " " << 0 << endl;
		for(map<int,int>::iterator it = pdf_current_experience.end(); it != pdf_current_experience.begin();) {
			it--;
			sum_cdf += it -> second;
			innovators_cdf_out << it->first << " " << sum_cdf / (double)num_of_innovation_reviews << endl;
		}
	}*/
	/*
	ofstream data_facts_out(Amazon::Global::output_directory + "random_facts.txt");
	data_facts_out << "number of innovation words: " << innovators_reviews.size() << endl;
	data_facts_out << "Average number of helpfulness in innovations: " << upvotes_of_reviews / num_of_innovation_reviews << endl;
	data_facts_out << "Average fraction of helpfulness in innovations: " << fraction_helpfulness / num_of_innovation_reviews << endl;
	{ // Innovation final
		ofstream innovators_cdf_out2(Amazon::Global::output_directory + "innovator_final_cdf.txt");
		sum_cdf = 0;
		innovators_cdf_out2 << (--pdf_final_experience.end())->first + 1 << " " << 0 << endl;
		for(map<int,int>::iterator it = pdf_final_experience.end(); it != pdf_final_experience.begin();) {
			it--;
			sum_cdf += it -> second;
			innovators_cdf_out2 << it->first << " " << sum_cdf / (double)num_of_innovation_reviews << endl;
		}
	}

	{ // All reviews present
		upvotes_of_reviews = 0;
		fraction_helpfulness = 0;
		pdf_current_experience.clear();
		pdf_final_experience.clear();
		for(Review review: reviews) {
			upvotes_of_reviews += SimpleStringToDouble(review.helpfulness);
			fraction_helpfulness += SimpleStringFractionToDouble(review.helpfulness);
			pdf_current_experience[review.current_experience_level] ++;
			pdf_final_experience[review.final_experience_level] ++;
		}
		ofstream all_cdf_out(Amazon::Global::output_directory + "all_present_cdf.txt");
		all_cdf_out << (--pdf_current_experience.end())->first + 1 << " " << 0 << endl;
		sum_cdf = 0;
		for(map<int,int>::iterator it = pdf_current_experience.end(); it != pdf_current_experience.begin();) {
			it--;
			sum_cdf += it -> second;
			all_cdf_out << it->first << " " << sum_cdf / (double)reviews.size() << endl;
		}
	}
	ofstream input_distribution_out("../Output_All/" + Global::NAMEOFDATASET + "_bursts/" + "distribution.txt");
	for(Review review : reviews) {
		input_distribution_out << review.current_experience_level << " " << review.final_experience_level << endl;
	}

	{ // All reviews final
		ofstream all_cdf_out2(Amazon::Global::output_directory + "all_final_cdf.txt");
		all_cdf_out2 << (--pdf_final_experience.end())->first + 1 << " " << 0 << endl;
		sum_cdf = 0;
		for(map<int,int>::iterator it = pdf_final_experience.end(); it != pdf_final_experience.begin();) {
			it--;
			sum_cdf += it -> second;
			all_cdf_out2 << it->first << " " << sum_cdf / (double)reviews.size() << endl;
		}
		pdf_current_experience.clear();
		pdf_final_experience.clear();
	}

	{ // Usage after innovation
		if(real_time == "RealTime") {
			int sum = 0;
			map<int,int> distribution_usage_after_innovations;
			for(WordTimeLine word_time_line : burst_innovation) {
				for(int i = word_time_line.burst_start; i < (int)word_time_line.states->size(); i++) {
					MyTime current = reviews[(*(word_time_line.review_index))[i]].time;
					MyTime started =reviews[(*(word_time_line.review_index))[word_time_line.burst_start]].time;
					int day = current.Day(started);
					distribution_usage_after_innovations[day] ++;
					sum ++;
				}
			}
			ofstream pdf_after_innovation_out(Amazon::Global::output_directory + "pdf_after_innovation.txt");
			for(pair<int, int> p : distribution_usage_after_innovations) {
				pdf_after_innovation_out << p.first << " " << p.second / (double)sum << endl;
			}
		}
	}
	{ // random baseline
		vector<int> baseline;
		for(Review review : reviews) {
			baseline.push_back(baseline.size() - 1);
		}
		ofstream random_baseline_innovation_out(Amazon::Global::output_directory + "random_baseline_innovation.txt");
		for(int i = 0; i < 10; i++) {
			random_shuffle(baseline.begin(), baseline.end());
			for(int j = 0; j < num_of_innovation_reviews; j++) {
				random_baseline_innovation_out << i << " " << reviews[baseline[j]].current_experience_level << " " << reviews[baseline[j]].final_experience_level << endl;
			}
		}
	}
	{ // do people innovate more than once or not?
		map<string, int> user_num_of_innovations;
		for(auto p : innovators_reviews) {
			set<string> temp;
			for(Review review : *(p.second)) {
				temp.insert(review.user_id);
			}
			for(string user : temp) {
				user_num_of_innovations[user]++;
			}
		}
		ofstream num_of_user_innovations(Amazon::Global::output_directory + "innovation_for_each_user.txt");
		for(pair<string, int> user_num : user_num_of_innovations) {
			num_of_user_innovations << user_num.first << " " << user_num.second << endl;
		}
	}
	{// Comparison if real innovations come later than review innovations
		map<int, int> time_to_num;
		for(auto p : innovators_reviews) {
			for(Review review : *(p.second)) {
				time_to_num[review.time.year * 12 + review.time.month] ++;
			}
		}
		ofstream innovation_timing_out(Amazon::Global::output_directory + "innovation_timing.txt");
		for(pair<int, int> time_num : time_to_num) {
			innovation_timing_out << fixed << setprecision(2);
			innovation_timing_out << time_num.first/12.0 << " " << time_num.second << endl;
		}
	}
	{ // When do different experiences show up in innovations
		ofstream xp_level_out(Amazon::Global::output_directory + "experiences_showing_up_on_timeline.txt");
		for(WordTimeLine word_time_line : burst_innovation) {
			set<string> users;
			MyTime started =reviews[(*(word_time_line.review_index))[word_time_line.burst_start]].time;
			for(int i = word_time_line.burst_start; i < (int)word_time_line.states->size(); i++) {
				int index = (*(word_time_line.review_index))[i];
				if(users.find(reviews[index].user_id) == users.end()) {
					users.insert(reviews[index].user_id);
					xp_level_out << word_time_line.word << " " << reviews[index].current_experience_level << " " << reviews[index].final_experience_level;
					xp_level_out << " " << reviews[index].time.Day(started)<< endl;
				}
			}
		}
	}*/
	// UserDistributionBasedOnNumberOfReviews(&reviews, &distribution_for_entire_data_set);
	/**/
	//	UserAngrinessBasedOnNumberOfReviews(&reviews);
	// StarAveragePerMonth(&reviews);
	//	StarAveragePerMonthAccumulatedOverYears(&reviews);
	cerr << "trying to finish it!" << endl;
	return 0;
}
