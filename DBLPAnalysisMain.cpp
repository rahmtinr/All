/*
 * DBLPAnalysisMain.cpp
 *
 *  Created on: Feb 9, 2015
 *      Author: rahmtin
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
int SIZE_OF_TOP_INNOVATIONS;
bool CREATE_RANDOM_BASELINE;
vector<bool> is_innovative;

bool inc_dec_cmp(const pair<int, int> &x, const pair<int, int> &y) {
	if(x.first == y.first) {
		return x.second >  y.second;
	}
	return x.first < y.first;
}

long long inversion(vector<pair<int,int> > *v, int left, int right) { // v needs to be sorted by the first element
	if(right - left == 1) {
		return 0;
	}
	long long ret = 0;
	int mid = (right + left) / 2;
	ret += inversion(v, left, mid);
	ret += inversion(v, mid, right);
	vector<pair<int, int> > temp;
	int p = left;
	int q = mid;
	while(p < mid && q < right) {
		if((*v)[p].second <= (*v)[q].second) {
			temp.push_back((*v)[p]);
			p++;
		} else {
			temp.push_back((*v)[q]);
			ret += mid - p;
			q++;
		}
	}
	while(p < mid) {
		temp.push_back((*v)[p]);
		p++;
	}
	while(q < right) {
		temp.push_back((*v)[q]);
		q++;
	}
	for(int i = 0; i <(int) temp.size(); i++) {
		(*v)[left++] = temp[i];
	}
	temp.clear();
	return ret;
}

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

	SIZE_OF_TOP_INNOVATIONS = 2000;
	CREATE_RANDOM_BASELINE = false;
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
	if(argc != 4) {
		cerr << "The number of arguments is not correct! Force quitting." << endl;
		return 0;
	}
	srand(time(0));
	initialize(argv);
	ifstream fin(argv[1]);
	ReadAllRecords(fin, &records, &reviews, &edges, &author_id, &rev_author_id);
	RemoveFakeNodes(&edges, author_id.size() + 1);
	Components(&edges, author_id.size() + 1);
	sort(reviews.begin(), reviews.end());
	reviews[0].index = 0;
	// Erase fake authors
	for(int i = 0; i < (int)reviews.size(); i++) {
		for(int j = 0; j < (int)reviews[i].authors.size(); j++) {
			if(fake_nodes.find(author_id[reviews[i].authors[j]]) != fake_nodes.end()) {
				reviews[i].authors.erase(reviews[i].authors.begin() + j);
				j--;
			}
		}
	}
	// Find the candidate for the paper
	for(int i = 0; i < (int)reviews.size(); i++){
		int cur_exp = -1;
		int nominated_author = 0;
		if(reviews[i].authors.size() == 0 ) {
			reviews[i].authors.push_back("Dummy Author" + SimpleIntToString(i));
		}
		for(int j = 0; j < (int)reviews[i].authors.size(); j++) {
			string author = reviews[i].authors[j];
			if(experience_level.find(author) == experience_level.end()) {
				experience_level[author] = 0;
			}
			if(cur_exp == -1) {
				cur_exp = experience_level[author];
			}
			//TODO YOU CAN CHANGE HERE FOR THE EXP!
			cur_exp = max(cur_exp,  experience_level[author]);
			if(cur_exp == experience_level[author]) {
				nominated_author = j;
			}
			experience_level[author]++;
		}
		reviews[i].current_experience_level = cur_exp;
		reviews[i].current_exp_best_author = reviews[i].authors[nominated_author];
	}
	ofstream entire_dataset_distribution_fout("../Output_All/DBLP/Bursts/DocRatio/distribution.txt");
	for(int i = 0; i < (int) reviews.size(); i++) {
		int final_exp = -1;
		int nominated_author = 0;
		for(int j = 0; j < (int)reviews[i].authors.size(); j++) {
			string author = reviews[i].authors[j];
			if(final_exp == -1) {
				final_exp = experience_level[author];
			}
			//TODO YOU CAN CHANGE HERE FOR THE EXP!
			final_exp = max(final_exp,  experience_level[author]);
			if(final_exp == experience_level[author]) {
				nominated_author = j;
			}
		}
		reviews[i].final_exp_best_author = reviews[i].authors[nominated_author];
		reviews[i].final_experience_level = experience_level[reviews[i].final_exp_best_author];
		entire_dataset_distribution_fout << reviews[i].current_experience_level << " "
				<< reviews[i].final_experience_level << endl;
	}
	// Random shuffle of nominated authors.
	{
		if(CREATE_RANDOM_BASELINE == true) {
			vector<pair<string, int> > baseline_current_authors;
			vector<pair<string, int> > baseline_final_authors;
			for(int i = 0; i < (int) reviews.size(); i++) {
				if(reviews[i].current_exp_best_author.substr(0, 5) == "Dummy") {
					continue;
				}
				baseline_current_authors.push_back(make_pair(reviews[i].current_exp_best_author, reviews[i].current_experience_level));
				baseline_final_authors.push_back(make_pair(reviews[i].final_exp_best_author, reviews[i].final_experience_level));
			}
			random_shuffle(baseline_current_authors.begin(), baseline_current_authors.end());
			random_shuffle(baseline_final_authors.begin(), baseline_final_authors.end());
			int last_unused = 0;
			for(int i = 0; i < (int) reviews.size(); i++) {
				if(reviews[i].authors[0].substr(0, 5) == "Dummy") {
					continue;
				}
				reviews[i].current_exp_best_author = baseline_current_authors[last_unused].first;
				reviews[i].current_experience_level = baseline_current_authors[last_unused].second;
				reviews[i].final_exp_best_author = baseline_final_authors[last_unused].first;
				reviews[i].final_experience_level = baseline_final_authors[last_unused].second;
				last_unused++;
			}
		}
	}
	{
		string s;
		int x;
		ifstream fin_innovation_best_burst("./DBLPparser/words_start_burst.txt");
		while(fin_innovation_best_burst >> s >> x) {
			if(s == "") {
				continue;
			}
			WordTimeLine word_time_line;
			word_time_line.word = s;
			word_time_line.burst_start = x;
			top_innovations.push_back(word_time_line);
		}
	}
	map<string, vector<Review>*> innovators_reviews;
	//	ofstream innovators_out(Amazon::Global::output_directory + "distribution.txt");
	vector<Review> reviews_have_word[SIZE_OF_TOP_INNOVATIONS + 10];
	vector<pair<int, int> > most_innovative(reviews.size());
	//	if(Amazon::Global::state_machine_doc_ratio == false) {
	//		Innovations::FindInnovationsBursts(&reviews, &top_innovations, &innovators_reviews);
	//	} else {
	int word_counter = 0;
	for(int j = 0; j < (int)reviews.size(); j++) {
		most_innovative[j].first = 0;
		most_innovative[j].second = j;
		is_innovative.push_back(false);
	}
	for(int k = 0; k < (int)top_innovations.size(); k++) {
		WordTimeLine temp = top_innovations[k];
		reviews_have_word[word_counter].clear();
		int index = temp.burst_start + 1;
		for(int j = 0 ; j < (int)reviews.size(); j++) {
			if(reviews[j].time.day < index - 1) {
				continue;
			}
			if(reviews[j].time.day > index + 1) {
				break;
			}
			stringstream ss(reviews[j].text);
			while(!ss.eof()) {
				string s;
				ss >> s;
				if(s == temp.word) {
					most_innovative[j].first++;
					reviews_have_word[word_counter].push_back(reviews[j]);
					is_innovative[j] = true;
					break;
				}
			}
		}
		innovators_reviews.insert(make_pair(temp.word, &reviews_have_word[word_counter]));
		word_counter++;
	}
#if 0
	/*	sort(most_
	 * innovative.begin(), most_innovative.end());
	reverse(most_innovative.begin(), most_innovative.end());
	cerr << "------> most innovative paper " << endl;
	for(int i = 0; i < 100; i++) {
		cerr << most_innovative[i].first << "\t" << reviews[most_innovative[i].second].time.day + 1935 << "\t" <<reviews[most_innovative[i].second].text << endl;
	}
	return 0;
	 */
	ofstream correlation_innovations_final_exp(Amazon::Global::output_directory + "innovation_final_exp.txt");
	ofstream correlation_innovations_current_exp(Amazon::Global::output_directory + "innovation_current_exp.txt");
	for(int k = 0; k < (int)top_innovations.size(); k += 20) {
		int num_of_innovation_reviews = 0;
		map<string, int> innovator_ids;
		map<int, int> pdf_current_experience;
		map<int,int> pdf_final_experience;
		map<string, int> innovation_at_cur_exp;
		int sum_cdf = 0;
		{ // Innovation present
			for(auto p : innovators_reviews) {
				bool first = false;
				for(Review review : *(p.second)) {
					if(Amazon::Global::state_machine_doc_ratio == false) {
						if(first == false) {
							innovator_ids[review.user_id] ++;
						}
					} else {
						//Final experience
						innovator_ids[review.final_exp_best_author] ++;

						//Current experience
						innovation_at_cur_exp[review.current_exp_best_author + SimpleIntToString(review.index)] =
								innovator_ids[review.current_exp_best_author];
						experience_level[review.current_exp_best_author + SimpleIntToString(review.index)] = review.current_experience_level;

					}
					first = true;
					//				innovators_out << review.current_experience_level << " " << review.final_experience_level << endl;
					/*
				for(int i = 0; i < review.authors.size(); i++) {
					innovators_out << review.authors[i] << ", ";
				}
				innovators_out <<" ::::: " << p.first << endl;
				innovators_out << endl;
					 */
					num_of_innovation_reviews ++;
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
		/*
	ofstream data_facts_out(Amazon::Global::output_directory + "random_facts.txt");
	data_facts_out << "number of innovation words: " << innovators_reviews.size() << endl;
	data_facts_out << "Average number of helpfulness in innovations: " << upvotes_of_reviews / num_of_innovation_reviews << endl;
	data_facts_out << "Average fraction of helpfulness in innovations: " << fraction_helpfulness / num_of_innovation_reviews << endl;
		 */
		{ // Innovation final experience
			ofstream innovators_cdf_out2(Amazon::Global::output_directory + "innovator_final_cdf.txt");
			sum_cdf = 0;
			innovators_cdf_out2 << (--pdf_final_experience.end())->first + 1 << " " << 0 << endl;
			for(map<int,int>::iterator it = pdf_final_experience.end(); it != pdf_final_experience.begin();) {
				it--;
				sum_cdf += it -> second;
				innovators_cdf_out2 << it->first << " " << sum_cdf / (double)num_of_innovation_reviews << endl;
			}
		}
		{ // All reviews present experience
			pdf_current_experience.clear();
			pdf_final_experience.clear();
			for(Review review: reviews) {
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

		{ // All reviews final experience
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

		// Correlation between experience and # of innovations + counting the number of components.
		set<int> different_components;

		for(auto p : innovator_ids) {
			string author = p.first;
			if(author.substr(0,5) == "Dummy") {
				continue;
			}
			int num_of_innovations = p.second;
			//		correlation_innovations_final_exp << num_of_innovations << "\t" << experience_level[author] << "\t" << author << endl;
			correlation_innovations_final_exp << num_of_innovations << "\t" << experience_level[author] << " " << k << endl;
			different_components.insert(author_component[author_id[author]]);
		}
		//		cerr << "Different components of innovative authors: " << different_components.size() << endl;

		for(auto p : innovation_at_cur_exp) { // Each innovation is the end of that guy! We imagine that the guy dies after that
			string author = p.first;
			if(author.substr(0,5) == "Dummy") {
				continue;
			}
			int num_of_innovations = p.second;
			correlation_innovations_current_exp << num_of_innovations << "\t" << experience_level[author] << " " << k << endl;
		}
	}
	{
		double best_ratio = 0;
		double worst_ratio = 1;
		string best_word, worst_word;
		// DFS over authors of a word
		for(int k = 0; k <(int) top_innovations.size(); k++) {
			set<int> valid_nodes;
			int index = top_innovations[k].burst_start + 1;
			for(int i = 0; i < (int)reviews.size(); i++) {
				if(reviews[i].time.day < index - 1) {
					continue;
				}
				if(reviews[i].time.day > index + 1) {
					break;
				}
				if(reviews[i].authors[0].substr(0,5) == "Dummy") {
					continue;
				}
				stringstream ss(reviews[i].text);
				while(!ss.eof()) {
					string s;
					ss >> s;
					if(s == top_innovations[k].word) {
						for(int j = 0; j < (int)reviews[i].authors.size(); j++) {
							valid_nodes.insert(author_id[reviews[i].authors[j]]);
						}
						break;
					}
				}
			}
			//			cerr << top_innovations[k].word << ":::: " << endl;
			DfsOverValid(top_innovations[k].word, &valid_nodes, best_ratio, worst_ratio, best_word, worst_word);
		}
		cerr << "Biggest giant component: " << best_word << " " << best_ratio << endl;
		cerr << "Smallest giant component: " << worst_word << " " << worst_ratio << endl;
	}

#endif

	/*
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
	 */
	// UserDistributionBasedOnNumberOfReviews(&reviews, &distribution_for_entire_data_set);
	/**/
	//	UserAngrinessBasedOnNumberOfReviews(&reviews);
	// StarAveragePerMonth(&reviews);
	//	StarAveragePerMonthAccumulatedOverYears(&reviews);


	// Richard Feynman trace back
	for(int k = 0; k < (int)top_innovations.size(); k++) {
		WordTimeLine word_time_line = top_innovations[k];
		int burst_start = word_time_line.burst_start;
		string word = word_time_line.word;
		word = " " + word + " ";
		vector<int> indecies_contain_word;
		set<int> good_nodes, bad_nodes;
		set<int> current_good_nodes, current_bad_nodes;
		for(int i = 0; i < (int)reviews.size(); i++) {
			if(reviews[i].text.find(word) == reviews[i].text.npos || reviews[i].authors[0].substr(0, 5) == "Dummy") {
				continue;
			}
			indecies_contain_word.push_back(i);
		}
		int c = 1;
		int current_year = burst_start - c;
		for(int i = 0; i < (int)indecies_contain_word.size(); i++) {
			string author;
			int index = indecies_contain_word[i];
			if(reviews[index].time.day < burst_start - c) { // Before our seeds
				continue;
			}
			if(current_year != reviews[index].time.day) {
				for(int x : current_bad_nodes) {
					bad_nodes.insert(x);
				}
				for(int x : current_good_nodes) {
					good_nodes.insert(x);
				}
				current_bad_nodes.clear();
				current_good_nodes.clear();
				current_year = reviews[index].time.day;
			}
			if(reviews[index].time.day == burst_start - c) { // Set our seeds
				for(int j = 0; j < (int)reviews[index].authors.size(); j++) {
					author = reviews[index].authors[j];
					good_nodes.insert(author_id[author]);
				}
			} else if(reviews[index].time.day < burst_start + 3){ // Find if they are connected or not
				bool check = false;
				for(int j = 0; j < (int)reviews[index].authors.size(); j++) {
					author = reviews[index].authors[j];
					int id = author_id[author];
					if(good_nodes.find(id) != good_nodes.end()) {
						check = true;
						break;
					}
				}
				for(int j = 0; j < (int)reviews[index].authors.size(); j++) {
					author = reviews[index].authors[j];
					int id = author_id[author];
					if(bad_nodes.find(id) != bad_nodes.end() || good_nodes.find(id) != good_nodes.end()) {
						continue;
					}
					if(check == false) {
						if(current_good_nodes.find(id) == current_good_nodes.end()) {
							current_bad_nodes.insert(id);
						}
					} else {
						if(current_bad_nodes.find(id) == current_bad_nodes.end()) { // here we are being optimistic
							current_bad_nodes.erase(id);
						}
						current_good_nodes.insert(id);
					}
				}
			} else {
				break;
			}
		}
		for(int x : current_bad_nodes) {
			bad_nodes.insert(x);
		}
		for(int x : current_good_nodes) {
			good_nodes.insert(x);
		}
		current_bad_nodes.clear();
		current_good_nodes.clear();
		cout << word << " " << burst_start + 1935 << " " << (double)good_nodes.size() / (good_nodes.size() + bad_nodes.size()) << " good: " << good_nodes.size() << " bad: " << bad_nodes.size() << endl;
	}


	/*
	{
		map<string, int> present_exp;
		map<string, int> present_innovations;
		// Comparing innovations with final xp
		vector<pair<int, int> >prediction_tuple[2000]; // In each row we have a tuple of # innovation #final xp
		for(int i = 0; i < (int)reviews.size(); i++) {
			if(is_innovative[i] == true) {
				present_innovations[reviews[i].current_exp_best_author]++;
			}
			if(reviews[i].authors[0].substr(0, 5) == "Dummy") {
				continue;
			}
			for(int j = 0; j < (int)reviews[i].authors.size(); j++) {
				string author = reviews[i].authors[j];
				present_exp[author]++;
				prediction_tuple[present_exp[author]].push_back(make_pair(present_innovations[author] , experience_level[author]));
			}
		}
		ofstream pairwise_comparison_fout(Amazon::Global::output_directory + "pairwise_comparison.txt");
		// first present exp for this author,second column is inversion, third column is the total number of pairs
		// forth column is accuracy
		for(int i = 0; i < 1000; i++) {
			if(prediction_tuple[i].size() < 2) {
				continue;
			}
			for(int j = 0; j < (int)prediction_tuple[i].size(); j++) {
				prediction_tuple[i][j].second = -prediction_tuple[i][j].second;
			}
			sort(prediction_tuple[i].begin(), prediction_tuple[i].end());
			int bef = 0;
			long long wrong = 0;
			for(int j = 0; j < (int)prediction_tuple[i].size(); j++) {
				if(prediction_tuple[i][j].first == prediction_tuple[i][bef].first) {
					continue;
				}
//				total += (long long)(prediction_tuple[i].size() - j) * (j - bef);
				wrong += ((long long)(j - bef) * (j - bef - 1)) / 2;
				bef = j;
			}
			wrong += ((long long)(prediction_tuple[i].size() - bef) * (prediction_tuple[i].size() - bef - 1)) / 2;
			long long total = ((long long)prediction_tuple[i].size() * (prediction_tuple[i].size() - 1)) / 2 - wrong;
			long long correct = inversion(&(prediction_tuple[i]), 0, prediction_tuple[i].size());
			pairwise_comparison_fout << i << " " << correct << " " << total << " " << (double)correct / total << endl;
		}
	}
	*/
	return 0;
}

