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
	SIZE_OF_TOP_INNOVATIONS = 500;

	// Set +INF and -INF for dates
	Amazon::Global::min_year = 2030;
	Amazon::Global::max_year = 1995;
	Amazon::Global::earliest.year = 2030;

	//State machine parameters.
	Amazon::Global::state_coeffecient = SimpleStringToInt(argv[5]);
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
	while (true) {
		if (!ReadOneReview(fin, &reviews)) {
			break;
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
	{
		string s;
		int x;
		ifstream fin_innovation_best_burst(Amazon::Global::output_directory + "words_start_burst_coeff_" + SimpleIntToString(int(Amazon::Global::state_coeffecient + 0.2)) + ".txt");

		while(fin_innovation_best_burst >> s >> x) {
			if(s == "") {
				continue;
			}
			WordTimeLine word_time_line;
			word_time_line.word = s;
			innovation_words[s] = top_innovations.size();
			word_time_line.burst_start = x;
			top_innovations.push_back(word_time_line);
			if((int)top_innovations.size() == SIZE_OF_TOP_INNOVATIONS) {
				break;
			}
		}
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
#if 0
	{
		// [a,b]
		cerr << "STARTING  [a,b]" << endl;
		bool final = true;
		string output_count[30000];
		const int SHIFTER = 1100;
		const int REL_SIZE = SHIFTER * 2;
		double average[REL_SIZE];
		const int CUT_OFF_EXP = 20;
		int num_of_reviews_more_than_cut_off = 0;
		for(int i = 0; i < reviews.size(); i++) {
			if(final == true && reviews[i].final_experience_level >= CUT_OFF_EXP) {
				num_of_reviews_more_than_cut_off++;
			}
			if(final == false && reviews[i].current_experience_level >= CUT_OFF_EXP) {
				num_of_reviews_more_than_cut_off++;
			}
		}
		for (int numerator = 1; numerator <= 4; numerator++) {
			vector<int> num_of_innovative_reviews_relative_to_burst(REL_SIZE);
			vector<int> sum_of_innovative_reviews_relative_to_burst(REL_SIZE);
			vector<pair<int, int> > authors_exp_relative_to_burst(REL_SIZE); //(sum, number of authors)
			for(int i = 0; i < REL_SIZE; i++) {
				authors_exp_relative_to_burst[i] = make_pair(0, 0);
			}
			fill(num_of_innovative_reviews_relative_to_burst.begin(), num_of_innovative_reviews_relative_to_burst.end(), 0);
			fill(sum_of_innovative_reviews_relative_to_burst.begin(), sum_of_innovative_reviews_relative_to_burst.end(), 0);
			int K;
			int alpha = 0;
			double fraction = 0;
			// find K, where K is the least final_exp which half the reviews come from them
			fraction = numerator / 4.0;
			fraction = 1 / fraction;
			cerr << "FINDING K for " << fraction << endl;
			{
				vector<int> counter_exp(5010);
				fill(counter_exp.begin(), counter_exp.end(), 0);
				for(int i = 0; i <(int) reviews.size(); i++) {
					counter_exp[reviews[i].final_experience_level]++; // We are dividing authors to 4 buckets by their final xp
				}
				int index = CUT_OFF_EXP - 1;
				cerr << "GOING INTO THE LOOP" << endl;
				while(fraction * alpha < num_of_reviews_more_than_cut_off) {
					index++;
					alpha += counter_exp[index];
				}
				K = index;
			}
			cerr << "------>" << K << " " << alpha << " " << reviews.size() << endl;
			cerr << "COMPUTING NUM" << endl;
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
					int start = top_innovations[innovation_words[s]].burst_start;
					num_of_innovative_reviews_relative_to_burst[reviews[i].time.day - start + SHIFTER]++;
					pair<int, int> p;
					p = authors_exp_relative_to_burst[reviews[i].time.day - start + SHIFTER];
					if(final == true) { // for averaging out we can either always use the final exp or use their present experience at that time
						authors_exp_relative_to_burst[reviews[i].time.day - start + SHIFTER] = make_pair(p.first + reviews[i].final_experience_level, p.second + 1);
					} else {
						authors_exp_relative_to_burst[reviews[i].time.day - start + SHIFTER] = make_pair(p.first + reviews[i].current_experience_level, p.second + 1);
					}
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
					if(numerator == 1 ) {
						output_count[temp_counter] = SimpleIntToString(a - SHIFTER) + "\t" + SimpleIntToString(b - SHIFTER);
					}
					int sum_a_b = sum_of_innovative_reviews_relative_to_burst[b] - sum_of_innovative_reviews_relative_to_burst[a - 1];
					output_count[temp_counter] += "\t" + SimpleDoubleToString(sum_a_b / (double)alpha);
					temp_counter++;
				}
			}
			/*
		 	if(numerator == 4) {
				for(int i = 0; i < REL_SIZE; i++) {
					average[i] = authors_exp_relative_to_burst[i].first / (double)authors_exp_relative_to_burst[i].second;
				}
				string filename = Amazon::Global::output_directory + "final_relative_year_usage_all_exp_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations.txt";
				ofstream rel_year_fout(filename.c_str());
				int temp1 = 0, temp2 = 1;
				for(int i = 0; i < REL_SIZE ; i++) {
					temp1 += authors_exp_relative_to_burst[i].first; // adding up the sum of experiences
					temp2 += authors_exp_relative_to_burst[i].second; // divide
					cerr << i << " " << temp1 << " " << temp2 << endl;
					rel_year_fout << i - SHIFTER << " " << temp1 / (double)temp2 << " " << sum_of_innovative_reviews_relative_to_burst[i] << endl;
				}
			}
			 */
		}


		string filename;
		if(final == true) {
			filename = Amazon::Global::output_directory + "final_[a,b]_top_" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleIntToString(int(Amazon::Global::state_coeffecient + 0.2)) + ".txt";
		} else {
			filename = Amazon::Global::output_directory + "current_[a,b]_top_" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleIntToString(int(Amazon::Global::state_coeffecient + 0.2)) + ".txt";
		}
		ofstream ab_fout(filename.c_str());
		int temp_counter = 0;
		ab_fout << "a\tb\tcq1\tcq2\tcq3\tcq4" << endl; //cq = count on quartile, binq = binary on quarter
		while(output_count[temp_counter].size() != 0) {
			ab_fout << output_count[temp_counter] << endl;
			temp_counter++;
		}

	}
#endif
#if 0
	{
		// Comparison with "No country for old men"
		int K;
		int K_bef = 0;
		int num[5][10000];
		int denom[5][10000];
		const int CUT_OFF_EXP = 20;
		int num_of_reviews_more_than_cut_off = 0;
		const bool binary = false;
		memset(num, 0, sizeof num);
		for(int i = 0; i < reviews.size(); i++) {
			if(reviews[i].final_experience_level >= CUT_OFF_EXP) {
				num_of_reviews_more_than_cut_off++;
			}
		}
		for(int numerator = 1; numerator <= 4; numerator++) {
			map<string, int> cur_exp;

			int alpha = 0;
			double fraction = 0;
			// find K, where K is the least final_exp which half the reviews come from them
			fraction = numerator / 4.0;
			fraction = 1 / fraction;
			cerr << "FINDING K for " << fraction << endl;
			{
				vector<int> counter_exp(10010);
				fill(counter_exp.begin(), counter_exp.end(), 0);
				for(int i = 0; i <(int) reviews.size(); i++) {
					counter_exp[reviews[i].final_experience_level]++; // EXP - final
				}
				int index = CUT_OFF_EXP - 1;
				cerr << "GOING INTO THE LOOP" << endl;
				while(fraction * alpha < num_of_reviews_more_than_cut_off) {
					index++;
					alpha += counter_exp[index];
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
					int innovation_index = innovation_words[s];
					if(top_innovations[innovation_index].burst_start - 1 > reviews[i].time.day || top_innovations[innovation_index].burst_start + 1 <= reviews[i].time.day) { // the range where we count the word as an innovation
						continue;
					}
					int start = top_innovations[innovation_words[s]].burst_start;
					num[numerator][reviews[i].current_experience_level]++; // Only the max final_exp added
					if(binary == true) {
						break;
					}
				}
			}
			for(pair<string, int> p : experience_level) {
				if(p.second > K || p.second <= K_bef) { // Only the max final_exp added
					continue;
				}
				for(int j = 0; j <= p.second; j++) {
					denom[numerator][j]++;
				}
			}
			K_bef = K;
		}

		string filename;
		if(binary == true) {
			filename = Amazon::Global::output_directory + "cristian_probability_binary_comparison_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleIntToString(int(Amazon::Global::state_coeffecient + 0.2)) + ".txt";
		} else {
			filename = Amazon::Global::output_directory + "cristian_probability_count_comparison_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleIntToString(int(Amazon::Global::state_coeffecient + 0.2)) + ".txt";
		}
		ofstream cristian_fout(filename.c_str());
		for(int i = 1; i <= 4; i++) {
			for(int j = 0; j < 1000; j++) {
				cristian_fout << i << " " << j << " " << num[i][j] / ((double)denom[i][j] + 1) << endl;
			}
		}
	}
#endif

#if 1
	// innovations vs num of reviews for authors with more than 20 reviews
	map<int, int> exp_innovoation_num;
	map<int, int> exp_review_num;
	int innovation_denom = 0;
	int review_denom = 0;
	const int CUT_OFF_EXP = 20;

	const bool binary = true;
	const bool final_exp = true;

	for(int i = 0; i < reviews.size(); i++) {
		if(reviews[i].final_experience_level < CUT_OFF_EXP) {
			continue;
		}
		review_denom++;
		if(final_exp == true) {
			exp_review_num[reviews[i].final_experience_level]++;
		} else {
			exp_review_num[reviews[i].current_experience_level]++;
		}
		stringstream ss(reviews[i].text);
		string s;
		while(!ss.eof()) {
			ss >> s;
			if(innovation_words.find(s) == innovation_words.end()) {
				continue;
			}
			int innovation_index = innovation_words[s];
			if(top_innovations[innovation_index].burst_start - 1 > reviews[i].time.day || top_innovations[innovation_index].burst_start + 1 <= reviews[i].time.day) { // the range where we count the word as an innovation
				continue;
			}
			innovation_denom++;
			if(final_exp == true) {
				exp_innovoation_num[reviews[i].final_experience_level] ++;
			} else {
				exp_innovoation_num[reviews[i].current_experience_level] ++;
			}
			if(binary == true) {
				break;
			}
		}
	}
	string filename = Amazon::Global::output_directory + "innovation_review_proportion_";
	if(binary == true) {
		filename += "binary_";
	} else {
		filename += "count_";
	}
	if(final_exp == true) {
		filename += "final_";
	} else {
		filename += "present_";
	}
	filename += SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations";
	filename += "_coeff" + SimpleIntToString(int(Amazon::Global::state_coeffecient + 0.2)) + ".txt";
	ofstream proportion_fout(filename.c_str());
	for(pair<int, int> p : exp_review_num) {
		int exp_level = p.first;
		int numerator = p.second;
		double out;
		if(innovation_denom == 0) {
			innovation_denom = 1;
		}
		proportion_fout << exp_level << " " << numerator / (double) review_denom << " " << exp_innovoation_num[exp_level] / (double)innovation_denom << endl;
	}
#endif
	cerr << "trying to finish it!" << endl;
	return 0;
}