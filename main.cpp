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
}

int main(int argc, char *argv[]) {
	if(argc != 6) {
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

	if(EqDouble(3.0, Amazon::Global::state_coeffecient))
	{ // No country for old members paper
		Innovations::LearnDictionary(0, reviews.size() / 4, &reviews);
		Innovations::FindCristianInnovations(reviews.size() / 4, &reviews, &innovations); // returns pair of word and review it was started.
		string filename = Amazon::Global::output_directory + "words_start_burst_cristian_quarter_dict.txt";
		ofstream fout_cristian_method(filename.c_str());
		cerr << "# of innovations: " << innovations.size() << endl;
		for(int i = 0; i < innovations.size(); i++) {
			int week_that_burst_started = (((innovations[i].second)[0]).time.epoch_time / (24 * 60 * 60) - (25 * 365)) / 7;
			fout_cristian_method << innovations[i].first << " " << week_that_burst_started << endl;
		}
		// Innovations::AnalyseInnovation(innovations, &reviews);
	}

	if(Amazon::Global::state_machine_doc_ratio == true) { // need to change the time only by day and forget about the year
		for(int i = 0; i < reviews.size(); i++) {
			reviews[i].time.day = reviews[i].time.epoch_time / (24 * 60 * 60) - (25 * 365); //setting the starting point to 25 * 365 days after Jan 1, 1970. The first review is in 1997 anyways
			// Bucket a week
			reviews[i].time.day /= 7;
		}
		Innovations::FindBurstsDocumentRatio(&words_states, &reviews);
	} else {
		Innovations::FindBurstsTimeDifference(&words_states, &reviews);
	}
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
	string filename = Amazon::Global::output_directory + "words_start_burst_coeff_" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
	ofstream innovation_burst_year_out(filename.c_str());

	string filename2 = Amazon::Global::output_directory + "words_start_burst_freq_coeff_" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
	ofstream innovation_burst_year_freq_out(filename2.c_str());

	for(WordTimeLine word_time_line : burst_innovation) {
		innovation_burst_year_out << word_time_line.word << "   " << word_time_line.burst_start << endl;
		innovation_burst_year_freq_out << word_time_line.word << "   " << word_time_line.burst_start << " " << word_time_line.review_index->size() << endl;
	}
        cerr << " burst size: " << burst_innovation.size() << endl;
	{
		string filename = Amazon::Global::output_directory + "innovation_words_summary_different_coeff.txt";
		map<string, string> saved;
		if(EqDouble(Amazon::Global::state_coeffecient, 2.6) == false) {
			ifstream fin_append(filename.c_str());
			string s1;
			string s2;
			getline(fin_append, s2);
			while(fin_append.good()) {
				fin_append >> s1;
				getline(fin_append, s2);
				saved[s1] = s2;
			}
			fin_append.close();
		}
		ofstream fout_append(filename.c_str());
		fout_append << "Coeff\tAverage_Freq\tMedian_Freq" << endl;
		int num = 0;
		double average = 0;
		int median;
		vector<int> median_finder;
		for(WordTimeLine word_time_line : burst_innovation) { // Find average and median.
			average += word_time_line.review_index->size();
			median_finder.push_back(word_time_line.review_index->size());
			num++;
			if(num == 500) {
				break;
			}
			if(num == 0) {
				num++;
				median_finder.push_back(0);
			}
		}
		average /= num;
		median = median_finder[median_finder.size() / 2];
		for(pair<string, string> p : saved) {
			fout_append << p.first << " " << p.second << endl;
		}
		fout_append << Amazon::Global::state_coeffecient << " " << average << " " << median << endl;
	}
#if 0

	ofstream fout(Amazon::Global::output_directory + "timeline.txt");

	cerr << "Size of innovations found: " << burst_innovation.size() << endl;
	int x = 0;
	for(WordTimeLine word_time_line : burst_innovation) {
		//		cerr << word_time_line.word << " " << word_time_line.difference << endl;
		x++;
		if (x == min((int)burst_innovation.size() / 10, 50)) {
			break;
		}
		string word = word_time_line.word;
		vector<int> times;
		vector<MyTime> dates;
		for(int index : *(word_time_line.review_index)) {
			if(Amazon::Global::real_time == true) { // Gap is based on real time
				times.push_back(reviews[index].time.Day(Amazon::Global::earliest));
			} else { // Gap is based on the review number
				// The index here is the day by day index not their index in the reviews array
				times.push_back(reviews[index].index);
			}
			dates.push_back(reviews[index].time);
		}

		vector<bool> states = *(word_time_line.states);
		top_innovations.push_back(word_time_line);
		for(int j = 0; j < (int)times.size(); j++) {
			fout << word << " " << times[j] << " " << j+1  << " " << states[j] << " " << dates[j].year + dates[j].month/(double)12 << endl;
		}
	}
	map<string, vector<Review>*> innovators_reviews;
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
	}
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
	data_facts_out << "Average number of helpfulness in all reviews: " << upvotes_of_reviews / (double)reviews.size() << endl;
	data_facts_out << "Average fraction of helpfulness in all reviews: " << fraction_helpfulness / (double)reviews.size()<< endl;
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
	}
	// UserDistributionBasedOnNumberOfReviews(&reviews, &distribution_for_entire_data_set);
	/**/
	//	UserAngrinessBasedOnNumberOfReviews(&reviews);
	// StarAveragePerMonth(&reviews);
	//	StarAveragePerMonthAccumulatedOverYears(&reviews);
#endif
	cerr << "trying to finish it!" << endl;
	return 0;
}
