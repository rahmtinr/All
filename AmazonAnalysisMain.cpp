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
	{
		string temp(argv[8]);
		if(temp == "Bigram") {
			Amazon::Global::bigram = true;
		} else {
			Amazon::Global::bigram = false;
		}
	}

	{
		string temp(argv[9]);
		if(temp == "Brand") {
			Amazon::Global::brand = true;
            cerr << " BRAND IS TRUE " << endl;
		} else {
			Amazon::Global::brand = false;
		}
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
	if(argc != 10) { // brand added
		cerr << "The number of arguments is not correct! Force quitting." << endl;
		return 0;
	}
	initialize(argv);
	ifstream fin(argv[1]);
	bool reddit = false;
	if(Global::NAMEOFDATASET.find("reddit") != string::npos) {
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
		string bigram_string = "words_";
		if(Amazon::Global::bigram == true) {
			bigram_string = "bigrams_";
		}
		if(cristians_innovations == false) {
			filename = Amazon::Global::output_directory + bigram_string + "min_life_span_start_burst_coeff_" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
			//			filename = Amazon::Global::output_directory + bigram_string + "empty2009_start_burst_coeff_" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt"; //TODO
			cerr << filename << endl;
		} else {
			filename = Amazon::Global::output_directory + bigram_string + "min_life_span_start_burst_cristian_0.05_dict.txt";
		}
		ifstream fin_innovation_best_burst(filename.c_str());
		// TODO
		Amazon::Global::output_directory += "mlf_" + bigram_string;
		//		Amazon::Global::output_directory += bigram_string + "empty2009_";
		ofstream fout_innovation_best_burst(Amazon::Global::output_directory + "innovation_time_histogram" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt");
		cerr << "-----> " << Amazon::Global::output_directory + "innovation_time_histogram" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt" << endl;
		while(fin_innovation_best_burst >> s >> x) {
			if(s == "") {
				continue;
			}
			WordTimeLine word_time_line;
			word_time_line.word = s;
			innovation_words[s] = top_innovations.size();
			word_time_line.burst_start = x;
			fout_innovation_best_burst << x << endl;
			top_innovations.push_back(word_time_line);
			if((int)top_innovations.size() == SIZE_OF_TOP_INNOVATIONS) {
				break;
			}
		}
	}
	{ // distribution of reviews over time - weekly bucketed! Now that day is based on week
		map<int, int> dist;
		for(int i = 0; i < (int)reviews.size(); i++) {
			dist[reviews[i].time.day]++;
		}
		ofstream fout_dist(("../Output_All/"  + Global::NAMEOFDATASET + "_bursts/" + Global::NAMEOFDATASET + "_distribution.txt").c_str());
		fout_dist << "Week \t ReviewNumber" << endl;
		for(pair<int, int> x : dist) {
			fout_dist << x.first << " " << x.second << endl;
		}
	}
	cerr << "top_innovations size: " << top_innovations.size() << endl;
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
#if 0
	{
		bool final = Amazon::Global::final;
		// [a,b] and blue and black plot
		cerr << "STARTING  [a,b]" << endl;
		int K_bef = 0;
		string output_count[30000];
		const int SHIFTER = 1100;
		const int REL_SIZE = SHIFTER * 2 + 10;
		const int CUT_OFF_EXP = 10;
		int num_of_reviews_more_than_cut_off = 0;
		int denominator = Amazon::Global::denominator;
		for(int i = 0; i < (int)reviews.size(); i++) {
			if(final == true && reviews[i].final_experience_level >= CUT_OFF_EXP) {
				num_of_reviews_more_than_cut_off++;
			}
			if(final == false && reviews[i].current_experience_level >= CUT_OFF_EXP) {
				num_of_reviews_more_than_cut_off++;
			}
		}
		K_bef = CUT_OFF_EXP - 1;
		int alpha_bef = 0;
		int K = CUT_OFF_EXP - 1;
		int alpha = 0;
		for (int numerator = 1; numerator <= denominator; numerator++) {
			K_bef = K;
			alpha_bef = alpha;
			alpha = 0;
			vector<long long> num_of_innovative_reviews_relative_to_burst(REL_SIZE);
			vector<long long> sum_of_innovative_reviews_relative_to_burst(REL_SIZE);
			vector<pair<long long, long long> > authors_exp_relative_to_burst(REL_SIZE); // (sum, number of authors)
			for(int i = 0; i < REL_SIZE; i++) {
				authors_exp_relative_to_burst[i] = make_pair(0, 0);
			}
			fill(num_of_innovative_reviews_relative_to_burst.begin(), num_of_innovative_reviews_relative_to_burst.end(), 0);
			fill(sum_of_innovative_reviews_relative_to_burst.begin(), sum_of_innovative_reviews_relative_to_burst.end(), 0);
			double fraction = 0;
			// find K, where K is the least final_exp which half the reviews come from them
			fraction = numerator / (double) denominator;
			fraction = 1 / fraction;
			cerr << "FINDING K for " << fraction << endl;
			{
				vector<int> counter_exp(50010);
				fill(counter_exp.begin(), counter_exp.end(), 0);
				int just_checking = 0;
				for(int i = 0; i <(int) reviews.size(); i++) {
					counter_exp[reviews[i].final_experience_level]++; // We are dividing authors to $denominator buckets by their final xp
					just_checking = max(just_checking, reviews[i].final_experience_level);
				}
				cerr << "highest experience: " << just_checking << endl;
				int index = CUT_OFF_EXP - 1;
				cerr << "GOING INTO THE LOOP" << endl;
				while(fraction * alpha + 0.5 < num_of_reviews_more_than_cut_off) {
					index++;
					alpha += counter_exp[index];
				}
				K = index;
			}
			cerr << "------>" << K << " " << alpha - alpha_bef << " " << num_of_reviews_more_than_cut_off << endl;
			cerr << fraction << " " << K << " " << K_bef << " " << alpha << " " << alpha_bef << " " << num_of_reviews_more_than_cut_off << endl;
			cerr << "COMPUTING NUM" << endl;
			for(int i = 0; i < (int)reviews.size(); i++) {
				if(final == true && reviews[i].final_experience_level < CUT_OFF_EXP) {
					continue;
				}
				if(final == false && reviews[i].current_experience_level < CUT_OFF_EXP) {
					continue;
				}
				if(reviews[i].final_experience_level > K || reviews[i].final_experience_level <= K_bef) {
					continue;
				}
				stringstream ss(reviews[i].text);
				string s;

				while(!ss.eof()) {
					ss >> s;
					if(innovation_words.find(s) == innovation_words.end()) {
						continue;
					}
					int start = top_innovations[innovation_words[s]].burst_start;
					int index = reviews[i].time.day - start + SHIFTER;
					num_of_innovative_reviews_relative_to_burst[index]++;
				}
			}

			//		cerr << "COMPUTING SUM" << endl;
			for(int i = 1; i < REL_SIZE; i++) {
				sum_of_innovative_reviews_relative_to_burst[i] = sum_of_innovative_reviews_relative_to_burst[i - 1] + num_of_innovative_reviews_relative_to_burst[i];
			}
			// quantile wise analysis of the papers in the final/present experience create more than half of the innovation?
			int temp_counter = 0;
			for(int a = SHIFTER - 100; a < SHIFTER + 100; a++) {
				for(int b = a + 1; b < SHIFTER + 100; b++) {
					//cerr << a << " %%%%%% " << b << endl;
					if(numerator == 1) {
						output_count[temp_counter] = SimpleIntToString(a - SHIFTER) + "\t" + SimpleIntToString(b - SHIFTER);
					}
					long long sum_a_b = sum_of_innovative_reviews_relative_to_burst[b] - sum_of_innovative_reviews_relative_to_burst[a - 1];
					output_count[temp_counter] += "\t" + SimpleDoubleToString(sum_a_b / (double)(alpha - alpha_bef));
					temp_counter++;
				}
			}
		}
		string filename;
		if(final == true) {
			filename = Amazon::Global::output_directory + "final_[a,b]_top_" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + "_" + SimpleIntToString(denominator) + "parts.txt";
		} else {
			filename = Amazon::Global::output_directory + "current_[a,b]_top_" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + "_" + SimpleIntToString(denominator)  + "parts.txt";
		}
		ofstream ab_fout(filename.c_str());
		int temp_counter = 0;
		ab_fout << "a\tb\t";
		for(int i = 1; i <= denominator; i++) {
			ab_fout << "cq"<< i << "\t";
		}
		ab_fout << endl;
		while(output_count[temp_counter].size() != 0) {
			ab_fout << output_count[temp_counter] << endl;
			temp_counter++;
		}
	}
#endif
#if 0 //TODO
	{
		cerr << "Got to median comparison" << endl;
		// Median comparison
		bool final = Amazon::Global::final;
		//		if(final == true) {
		const int SHIFTER = 1100;
		const int REL_SIZE = SHIFTER * 2 + 10;
		vector<int> median_finder[REL_SIZE];
		const int CUT_OFF_EXP = 10;
		vector<long long> num_of_innovative_reviews_relative_to_burst(REL_SIZE);
		vector<long long> sum_of_innovative_reviews_relative_to_burst(REL_SIZE);
		vector<pair<long long, long long> > authors_exp_relative_to_burst(REL_SIZE);
		map<int, int> pdf_exp;
		vector<int> cdf_exp;
		int num_of_reviews_more_than_cut_off = 0;
		int biggest_exp = -1;
		for(int i = 0; i < REL_SIZE; i++) {
			authors_exp_relative_to_burst[i] = make_pair(0, 0);
		}
		fill(num_of_innovative_reviews_relative_to_burst.begin(), num_of_innovative_reviews_relative_to_burst.end(), 0);
		fill(sum_of_innovative_reviews_relative_to_burst.begin(), sum_of_innovative_reviews_relative_to_burst.end(), 0);
		for(int i = 0; i < (int)reviews.size(); i++) {
			if(reviews[i].final_experience_level < CUT_OFF_EXP) {
				continue;
			}
			num_of_reviews_more_than_cut_off++;
			stringstream ss(reviews[i].text);
			string s;
			pdf_exp[reviews[i].final_experience_level]++;
			biggest_exp = max(biggest_exp, reviews[i].final_experience_level);
			while(!ss.eof()) {
				ss >> s;
				if(innovation_words.find(s) == innovation_words.end()) {
					continue;
				}
				int start = top_innovations[innovation_words[s]].burst_start;
				int index = reviews[i].time.day - start + SHIFTER;
				num_of_innovative_reviews_relative_to_burst[index]++;
				pair<long long, long long> p;
				p = authors_exp_relative_to_burst[index];
				authors_exp_relative_to_burst[index] = make_pair(p.first + reviews[i].final_experience_level, p.second + 1);
				median_finder[index].push_back(reviews[i].final_experience_level);
			}
		}
		cdf_exp.push_back(0);
		cdf_exp[0] += pdf_exp[0];
		for(int i = 1; i <= biggest_exp; i++) {
			cdf_exp.push_back(cdf_exp[i-1]);
			cdf_exp[i] += pdf_exp[i];
		}
		for(int i = 0; i < REL_SIZE; i++) {
			if(i == 0) {
				sum_of_innovative_reviews_relative_to_burst[i] = num_of_innovative_reviews_relative_to_burst[i];
			} else {
				sum_of_innovative_reviews_relative_to_burst[i] = num_of_innovative_reviews_relative_to_burst[i] + sum_of_innovative_reviews_relative_to_burst[i - 1];
			}
			sort(median_finder[i].begin(), median_finder[i].end());
		}
		string filename;
		filename = Amazon::Global::output_directory + "final_relative_year_usage_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
		ofstream rel_year_fout(filename.c_str());
		double temp1 = 0, temp2 = 1;
		for(int i = 0; i < REL_SIZE ; i++) {
			temp1 += authors_exp_relative_to_burst[i].first; // adding up the sum of experiences
			temp2 += authors_exp_relative_to_burst[i].second; // divide
			//   cerr << i << " " << authors_exp_relative_to_burst[i].first << " " << authors_exp_relative_to_burst[i].second << endl;
			//   cerr << temp1 << " " << temp2 << " " << temp1 / temp2 << endl;
			if(median_finder[i].size() == 0) {
				median_finder[i].push_back(0);
			}
			rel_year_fout << i - SHIFTER << " " << temp1 / (double)temp2 << " " << sum_of_innovative_reviews_relative_to_burst[i] << " " << median_finder[i][median_finder[i].size() / 2]<< endl;
		}
		/**/
		cerr << "I'm gonna start bucketing" << endl;
		// Bucketing weeks to have same size and then averaging over different weeks instead of accumulating the experience over time
		{
			const int bucket_num_size = 1;
			int bucket_num[bucket_num_size] = {1500/*, 500, 1000, 2000, 5000, 10000, 20000, 30000*/};
			for(int bucket_index = 0; bucket_index < bucket_num_size; bucket_index++) {
				for(int i = 0; i < REL_SIZE; i++) {
					authors_exp_relative_to_burst[i] = make_pair(0, 0);
					median_finder[i].clear();
				}
				int week[REL_SIZE];
				int first_empty = 1;
				week[0] = -1100;
				int each_bucket = bucket_num[bucket_index];
				long long sum = 0;
				/*
				if(bucket_index == bucket_num_size - 1) {
					int iterations = 0;
					while(true) {
						iterations++;
						each_bucket = bucket_num[bucket_index] * iterations;
						sum = 0;
						first_empty = 1;
						for(int j = 0; j < REL_SIZE; j++) {
							sum += num_of_innovative_reviews_relative_to_burst[j];
							if(sum > each_bucket) {
								week[first_empty++] = j + 1;
								sum = 0;
							}
						}
						if(first_empty < 30) {
							break;
						}
					}
				} else {*/
				for(int j = 0; j < REL_SIZE; j++) {
					sum += num_of_innovative_reviews_relative_to_burst[j];
					if(sum > each_bucket) {
						week[first_empty++] = j + 1;
						sum = 0;
					}
				}
				//			}
				for(int i = 0; i < (int)reviews.size(); i++) {
					if(reviews[i].final_experience_level < CUT_OFF_EXP) {
						continue;
					}
					stringstream ss(reviews[i].text);
					string s;
					while(!ss.eof()) {
						ss >> s;
						if(innovation_words.find(s) == innovation_words.end()) {
							continue;
						}
						int start = top_innovations[innovation_words[s]].burst_start;

						pair<long long, long long> p;
						int bucket = upper_bound(week, week + first_empty, reviews[i].time.day - start + SHIFTER) - week - 1;
						p = authors_exp_relative_to_burst[bucket];
						authors_exp_relative_to_burst[bucket] = make_pair(p.first + reviews[i].final_experience_level, p.second + 1);
						median_finder[bucket].push_back(reviews[i].final_experience_level);
					}
				}

				for(int i = 0; i < REL_SIZE; i++) {
					sort(median_finder[i].begin(), median_finder[i].end());
				}
				{
					string filename;
					filename = Amazon::Global::output_directory + "final_relative_year_usage_bucketed_" + SimpleIntToString(each_bucket)  +"_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
					ofstream fout_bucket(filename.c_str());
					fout_bucket << "Bucket_number\tStart_week\tAverage_experience\tMedian_experience" << endl;
					for(int i = 1; i < first_empty; i++) {
						if(median_finder[i].size() == 0) {
							median_finder[i].push_back(0);
						}
						fout_bucket << i << "\t" << week[i] << "\t" << authors_exp_relative_to_burst[i].first / (double)authors_exp_relative_to_burst[i].second << " " << median_finder[i][median_finder[i].size() / 2] << endl;
					}
				}
				{
					string filename;
					filename = Amazon::Global::output_directory + "final_relative_year_usage_bucketed_" + SimpleIntToString(each_bucket) + "_median_comparison_"
							+ SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
					// Where does the median experience of each innovation bucket lie comparing to all the reviews
					ofstream fout_bucket_median_comparison(filename.c_str());
					fout_bucket_median_comparison << "Bucket_number\tStart_week\tFraction" << endl;
					for(int i = 0; i < first_empty; i++) {
						if(median_finder[i].size() == 0) {
							median_finder[i].push_back(0);
						}
						int median = median_finder[i][median_finder[i].size() / 2];
						fout_bucket_median_comparison << i << "\t" << week[i] << "\t" << cdf_exp[median] / (double)num_of_reviews_more_than_cut_off << endl;
					}
				}
			}
		}
		//	}
	}
#endif

#if 1
	{
		if(Amazon::Global::bigram == false) {
			// Comparison with "No country for old men"
			cerr << "Comparison to no country for old men" << endl;
			int K;
			int K_bef = 0;
			int num[5][200000];
			int denom[5][200000];
			const int CUT_OFF_EXP = 10;
			int num_of_reviews_more_than_cut_off = 0;
			int denominator = 4;
			memset(num, 0, sizeof num);
			memset(denom, 0, sizeof denom);

			for(int i = 0; i < (int)reviews.size(); i++) {
				if(reviews[i].final_experience_level >= CUT_OFF_EXP) {
					num_of_reviews_more_than_cut_off++;
				}
			}
			K_bef = CUT_OFF_EXP - 1;
			K = CUT_OFF_EXP - 1;
			for(int numerator = 1; numerator <= denominator; numerator++) {
				K_bef = K;
				map<string, int> cur_exp;

				int alpha = 0;
				double fraction = 0;
				// find K, where K is the least final_exp which half the reviews come from them
				fraction = numerator / 4.0;
				fraction = 1 / fraction;
				cerr << "FINDING K for " << fraction << endl;
				{
					vector<int> counter_exp(200010);
					fill(counter_exp.begin(), counter_exp.end(), 0);
					for(int i = 0; i <(int) reviews.size(); i++) {
						counter_exp[reviews[i].final_experience_level]++; // EXP - final
					}
					int index = CUT_OFF_EXP;
					cerr << "GOING INTO THE LOOP" << endl;
					while(fraction * alpha < num_of_reviews_more_than_cut_off && index < 200000) {
						index++;
						alpha += counter_exp[index];
						//			cerr << alpha * fraction << " " << num_of_reviews_more_than_cut_off << endl;
					}
					K = index;
				}
				cerr <<"------>" << K_bef << " " << K << " " << alpha << " " << reviews.size() << endl;
				cerr << "COMPUTING NUM" << endl;
				for(int i = 0; i < (int)reviews.size(); i++) {
					if(reviews[i].final_experience_level < CUT_OFF_EXP) {
						continue;
					}
					stringstream ss(reviews[i].text);
					string s;
					for(string author : reviews[i].authors) {
						cur_exp[author]++;
					}
					if(reviews[i].final_experience_level > K || reviews[i].final_experience_level <= K_bef) { // Only the max final_exp added
						continue;
					}
					while(!ss.eof()) {
						ss >> s;
						if(innovation_words.find(s) == innovation_words.end()) {
							continue;
						}
						/*
					int innovation_index = innovation_words[s];
					if(top_innovations[innovation_index].burst_start - 1 > reviews[i].time.day || top_innovations[innovation_index].burst_start + 5 <= reviews[i].time.day) { // the range where we count the word as an innovation
						continue;
					}
						 */
						num[numerator][(50 * reviews[i].current_experience_level / reviews[i].final_experience_level)]++; // Only the max final_exp added
					}
				}
				for(pair<string, int> p : experience_level) {
					if(p.second > K || p.second <= K_bef) { // Only the max final_exp added
						continue;
					}
					for(int j = 0; j <= p.second; j++) {
						denom[numerator][(int)(50 * (j / p.second))]++;
					}
				}
			}

			string filename;
			filename = Amazon::Global::output_directory + "cristian_comparison_coeff_" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
			ofstream cristian_fout(filename.c_str());
// life mapped to [0, 1]
			for(int i = 1 ; i <= 4; i++) {
				for(int j = 0 ; j < 50; j++) {
					cristian_fout << i << " " << j << " " << (double) num[i][j] / denom[i][j] << endl;
				}
			}
/*	// Cristian life phase
			for(int i = 1; i <= 4; i++) {
				int each_bucket = 30;
				long long sum_numerator = 0;
				long long sum_denominator = 0;
				int bucket_num = 1;
				int bound = 20000;
				while(true) {
					int bef = bound;
					int blah = 0;
					for(int bef=bound; bef > bound - each_bucket; bef--) {
						blah += num[i][bef];
					}
					if(blah > 10) {
						break;
					}
					bound -= each_bucket;
				}
				for(int j = 0; j < bound; j+=each_bucket) {
					sum_numerator = 0;
					sum_denominator = 0;
					for(int k = j; k < j + each_bucket; k++) {
						sum_numerator += num[i][k];
						sum_denominator += denom[i][k];
					}
					bucket_num++;
					if(sum_numerator == 0) {
						continue;
					}
					cristian_fout << i << " " << bucket_num	 << " " << sum_numerator / ((double)sum_denominator + 1) << endl;
				}
			}
*/
		}
	}
#endif

#if 1  // current experience median comparison
	//	if(Amazon::Global::final == false) {
	const int shifter = 1100;
	int bucket_size = 1500;
	vector<int> week_dist[shifter];
	vector<double> relative_alphas[2 * shifter];
	for(Review review : reviews) {
		int absolute_week = review.time.day;
		week_dist[absolute_week].push_back(review.current_experience_level);
	}
	for(int i = 0; i < shifter; i++) {
		sort(week_dist[i].begin(), week_dist[i].end());
	}
	for(int i = 0; i < (int)reviews.size(); i++) {
		stringstream ss(reviews[i].text);
		string s;
		while(!ss.eof()) {
			ss >> s;
			if(innovation_words.find(s) == innovation_words.end()) {
				continue;
			}
			int start = top_innovations[innovation_words[s]].burst_start;
			int absolute_week = reviews[i].time.day;
			int relative_week = absolute_week - start + shifter;
			int index = upper_bound(week_dist[absolute_week].begin(), week_dist[absolute_week].end(), reviews[i].current_experience_level) - week_dist[absolute_week].begin();
			relative_alphas[relative_week].push_back(index / (double)week_dist[absolute_week].size());
		}
	}
	string filename = Amazon::Global::output_directory + "current_relative_year_usage_bucketed_" + SimpleIntToString(bucket_size)  +"_median_comparison_" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";

	ofstream current_exp_out(filename.c_str());
	current_exp_out << "Start_week   Average     Fraction" << endl;
	double average = 0;
	double median;
	vector<double> values_in_this_bucket;
	int starting_week;
	int num = 0;
	for(int i = 0; i < 2 * shifter; i++) {
		if(relative_alphas[i].size() == 0) {
			continue;
		}
		num += relative_alphas[i].size();
		for(int j = 0; j < (int)relative_alphas[i].size(); j++){
			average += relative_alphas[i][j];
			values_in_this_bucket.push_back(relative_alphas[i][j]);
		}
		sort(values_in_this_bucket.begin(), values_in_this_bucket.end());
		if(num >= bucket_size || i == 2 * shifter - 1) {
			starting_week = i;
			median = values_in_this_bucket[values_in_this_bucket.size() / 2];
			average /= num;
			current_exp_out << starting_week - shifter << " " << average << " " << median << endl;

			average = 0;
			median = 0;
			values_in_this_bucket.clear();
			num = 0;
		}
	}
	//	}
#endif

#if 0
	{
		cerr << "Got to median comparison for only the first occurrence for each user" << endl;
		// Median comparison
		bool final = Amazon::Global::final;
		const int SHIFTER = 1100;
		const int REL_SIZE = SHIFTER * 2 + 10;
		vector<int> median_finder[REL_SIZE];
		const int CUT_OFF_EXP = 10;
		vector<long long> num_of_innovative_reviews_relative_to_burst(REL_SIZE);
		vector<long long> sum_of_innovative_reviews_relative_to_burst(REL_SIZE);
		vector<pair<long long, long long> > authors_exp_relative_to_burst(REL_SIZE);
		map<int, int> pdf_exp;
		vector<int> cdf_exp;
		set<pair<string, string> > user_word_to_relative_week;
		int num_of_reviews_more_than_cut_off = 0;
		int biggest_exp = -1;
		for(int i = 0; i < REL_SIZE; i++) {
			authors_exp_relative_to_burst[i] = make_pair(0, 0);
		}
		fill(num_of_innovative_reviews_relative_to_burst.begin(), num_of_innovative_reviews_relative_to_burst.end(), 0);
		fill(sum_of_innovative_reviews_relative_to_burst.begin(), sum_of_innovative_reviews_relative_to_burst.end(), 0);
		for(int i = 0; i < (int)reviews.size(); i++) {
			if(final == true && reviews[i].final_experience_level < CUT_OFF_EXP) {
				continue;
			}
			if(final == false && reviews[i].current_experience_level < CUT_OFF_EXP) {
				continue;
			}
			num_of_reviews_more_than_cut_off++;

			stringstream ss(reviews[i].text);
			string s;
			if(final == true) {
				pdf_exp[reviews[i].final_experience_level]++;
				biggest_exp = max(biggest_exp, reviews[i].final_experience_level);
			} else {
				pdf_exp[reviews[i].current_experience_level]++;
				biggest_exp = max(biggest_exp, reviews[i].current_experience_level);
			}
			while(!ss.eof()) {
				ss >> s;
				if(innovation_words.find(s) == innovation_words.end()) {
					continue;
				}
				int start = top_innovations[innovation_words[s]].burst_start;
				int index = reviews[i].time.day - start + SHIFTER;
				num_of_innovative_reviews_relative_to_burst[index]++;
			}
		}
		cdf_exp.push_back(0);
		cdf_exp[0] += pdf_exp[0];
		for(int i = 1; i <= biggest_exp; i++) {
			cdf_exp.push_back(cdf_exp[i-1]);
			cdf_exp[i] += pdf_exp[i];
		}
		for(int i = 0; i < REL_SIZE; i++) {
			if(i == 0) {
				sum_of_innovative_reviews_relative_to_burst[i] = num_of_innovative_reviews_relative_to_burst[i];
			} else {
				sum_of_innovative_reviews_relative_to_burst[i] = num_of_innovative_reviews_relative_to_burst[i] + sum_of_innovative_reviews_relative_to_burst[i - 1];
			}
		}
		/**/
		cerr << "I'm gonna start bucketing" << endl;
		// Bucketing weeks to have same size and then averaging over different weeks instead of accumulating the experience over time
		{
			const int bucket_num_size = 8;
			int bucket_num[bucket_num_size] = {100, 500, 1000, 2000, 5000, 10000, 20000, 30000};
			for(int bucket_index = 0; bucket_index < bucket_num_size; bucket_index++) {
				for(int i = 0; i < REL_SIZE; i++) {
					authors_exp_relative_to_burst[i] = make_pair(0, 0);
					median_finder[i].clear();
				}
				user_word_to_relative_week.clear();
				int week[REL_SIZE];
				int first_empty = 1;
				week[0] = -1100;
				int each_bucket = bucket_num[bucket_index];
				long long sum = 0;
				if(bucket_index == bucket_num_size - 1) {
					int iterations = 0;
					while(true) {
						iterations++;
						each_bucket = bucket_num[bucket_index] * iterations;
						sum = 0;
						first_empty = 1;
						for(int j = 0; j < REL_SIZE; j++) {
							sum += num_of_innovative_reviews_relative_to_burst[j];
							if(sum > each_bucket) {
								week[first_empty++] = j + 1;
								sum = 0;
							}
						}
						if(first_empty < 30) {
							//			ofstream largest_bucket_fout("largest_bucket.txt", ios::out | ios::app);
							//			largest_bucket_fout << "Name: " << Global::NAMEOFDATASET << " Final:" << Amazon::Global::final << " coeff:" << Amazon::Global::state_coeffecient << " each bucket:" << each_bucket << endl;
							break;
						}
					}
				} else {
					for(int j = 0; j < REL_SIZE; j++) {
						sum += num_of_innovative_reviews_relative_to_burst[j];
						if(sum > each_bucket) {
							week[first_empty++] = j + 1;
							sum = 0;
						}
					}
				}
				for(int i = 0; i < (int)reviews.size(); i++) {
					if(final == true && reviews[i].final_experience_level < CUT_OFF_EXP) {
						continue;
					}
					if(final == false && reviews[i].current_experience_level < CUT_OFF_EXP) {
						continue;
					}
					stringstream ss(reviews[i].text);
					string s;
					while(!ss.eof()) {
						ss >> s;
						if(innovation_words.find(s) == innovation_words.end()) {
							continue;
						}
						{
							pair<string, string> p = make_pair(reviews[i].user_id, s);
							if(user_word_to_relative_week.find(p) != user_word_to_relative_week.end()) {
								continue;
							}
							user_word_to_relative_week.insert(p);
						}

						int start = top_innovations[innovation_words[s]].burst_start;
						pair<long long, long long> p;
						int bucket = upper_bound(week, week + first_empty, reviews[i].time.day - start + SHIFTER) - week - 1;
						p = authors_exp_relative_to_burst[bucket];
						if(final == true) { // for averaging out we can either always use the final exp or use their present experience at that time
							authors_exp_relative_to_burst[bucket] = make_pair(p.first + reviews[i].final_experience_level, p.second + 1);
							median_finder[bucket].push_back(reviews[i].final_experience_level);
						} else {
							authors_exp_relative_to_burst[bucket] = make_pair(p.first + reviews[i].current_experience_level, p.second + 1);
							median_finder[bucket].push_back(reviews[i].current_experience_level);
						}
					}
				}

				for(int i = 0; i < REL_SIZE; i++) {
					sort(median_finder[i].begin(), median_finder[i].end());
				}
				{
					string filename;
					if(final == true) {
						filename = Amazon::Global::output_directory + "final_relative_year_first_usage_bucketed_" + SimpleIntToString(each_bucket)  +"_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
					} else {
						filename = Amazon::Global::output_directory + "current_relative_year_first_usage_bucketed_" + SimpleIntToString(each_bucket)  +"_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
					}
					ofstream fout_bucket(filename.c_str());
					fout_bucket << "Bucket_number\tStart_week\tAverage_experience\tMedian_experience" << endl;
					for(int i = 1; i < first_empty; i++) {
						if(median_finder[i].size() == 0) {
							median_finder[i].push_back(0);
						}
						fout_bucket << i << "\t" << week[i] << "\t" << authors_exp_relative_to_burst[i].first / (double)authors_exp_relative_to_burst[i].second << " " << median_finder[i][median_finder[i].size() / 2] << endl;
					}
				}
				{
					string filename;
					if(final == true) {
						filename = Amazon::Global::output_directory + "final_relative_year_first_usage_bucketed_" + SimpleIntToString(each_bucket) + "_median_comparison_"
								+ SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
					} else {
						filename = Amazon::Global::output_directory + "current_relative_year_first_usage_bucketed_" + SimpleIntToString(each_bucket) +"_median_comparison_"
								+ SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
					}
					// Where does the median experience of each innovation bucket lie comparing to all the reviews
					ofstream fout_bucket_median_comparison(filename.c_str());
					fout_bucket_median_comparison << "Bucket_number\tStart_week\tFraction" << endl;
					for(int i = 0; i < first_empty; i++) {
						if(median_finder[i].size() == 0) {
							median_finder[i].push_back(0);
						}
						int median = median_finder[i][median_finder[i].size() / 2];
						fout_bucket_median_comparison << i << "\t" << week[i] << "\t" << cdf_exp[median] / (double)num_of_reviews_more_than_cut_off << endl;
					}
				}
			}
		}
	}
#endif
#if 0
	{
		cerr << "Starting the distribution comparison (review on exp vs innovation on exp) and (innovation on exp vs random pick on exp)" << endl;
		// innovations vs num of reviews for authors with more than 20 reviews
		map<int, int> exp_innovoation_num;
		map<int, int> exp_review_num;
		int innovation_denom = 0;
		int review_denom = 0;
		const int CUT_OFF_EXP = 10;
		bool final = Amazon::Global::final;
		vector<int> experience_array;
		int max_exp_innovated = 0;
		map<int, int> random_exp_to_count;
		for(int i = 0; i < (int)reviews.size(); i++) {
			if(reviews[i].final_experience_level < CUT_OFF_EXP) {
				continue;
			}
			review_denom++;
			if(final == true) {
				exp_review_num[reviews[i].final_experience_level]++;
				experience_array.push_back(reviews[i].final_experience_level);
			} else {
				exp_review_num[reviews[i].current_experience_level]++;
				experience_array.push_back(reviews[i].current_experience_level);
			}
			stringstream ss(reviews[i].text);
			string s;
			while(!ss.eof()) {
				ss >> s;
				if(innovation_words.find(s) == innovation_words.end()) {
					continue;
				}
				int innovation_index = innovation_words[s];
				/*			if(top_innovations[innovation_index].burst_start - 1 > reviews[i].time.day || top_innovations[innovation_index].burst_start + 1 <= reviews[i].time.day) { // the range where we count the word as an innovation
					continue;
				}
				 */
				innovation_denom++;
				if(final == true) {
					exp_innovoation_num[reviews[i].final_experience_level]++;
					max_exp_innovated = max(max_exp_innovated, reviews[i].final_experience_level);
				} else {
					exp_innovoation_num[reviews[i].current_experience_level]++;
					max_exp_innovated = max(max_exp_innovated, reviews[i].current_experience_level);
				}
			}
		}
		srand(time(0));
		int index;
		int num_of_trials = 50;
		for(int i = 0; i < num_of_trials; i++) {
			for(int j = 0; j < innovation_denom; j++) {
				index = rand() % experience_array.size();
				random_exp_to_count[experience_array[index]]++;
			}
		}
		for(pair<int, int> p : random_exp_to_count) {
			random_exp_to_count[p.first] = p.second / num_of_trials;
		}
		{
			string filename = Amazon::Global::output_directory + "innovation_review_proportion_";
			if(final == true) {
				filename += "final_";
			} else {
				filename += "current_";
			}
			filename += SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations";
			filename += "_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
			ofstream proportion_fout(filename.c_str());
			for(pair<int, int> p : exp_review_num) {
				int exp_level = p.first;
				int numerator = p.second;
				if(innovation_denom == 0) {
					innovation_denom = 1;
				}
				proportion_fout << exp_level << " " << numerator / (double) review_denom << " " << exp_innovoation_num[exp_level] / (double)innovation_denom << endl;
			}
		}
		{
			string filename;
			if(final == true){
				filename = Amazon::Global::output_directory + "_final_innovation_vs_random_distribution_comparison_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
			} else {
				filename = Amazon::Global::output_directory + "_current_innovation_vs_random_distribution_comparison_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
			}
			ofstream random_fout(filename.c_str());
			random_fout  << "exp_level\tinnovators\trandom" << endl;
			for(int i = 0; i < max_exp_innovated; i++) {
				random_fout << i << "\t" << exp_innovoation_num[i] << "\t" << random_exp_to_count[i] << endl;
			}
		}
	}
#endif

	cerr << "trying to finish it!" << endl;
	return 0;
}
