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
map<int, int> distribution_for_entire_data_set;
map<string, int> experience_level;
set<WordTimeLine> burst_innovation;
string MODE;

string filename;
string burst_mode;
string real_time;
int SIZE_OF_TOP_INNOVATIONS;
bool CREATE_RANDOM_BASELINE;
vector<bool> is_innovative;

void StrongDfs(int x, vector<vector<int> >*graph, vector<int> *local_mark, vector<int> *fin_time) {
	(*local_mark)[x] = 1;
	for(int temp : (*graph)[x]) {
		if((*local_mark)[temp] == 0) {
			StrongDfs(temp, graph, local_mark, fin_time);
		}
	}
	fin_time->push_back(x);
}

void StrongDfsRev(int x, vector<vector<int> >*graph, vector<int> *local_mark, vector<int> *comp, int comp_num) {
	(*local_mark)[x] = comp_num;
	for(int temp : (*graph)[x]) {
		if((*local_mark)[temp] == 0) {
			StrongDfsRev(temp, graph, local_mark, comp, comp_num);
		}
	}
	comp->push_back(x);
}

int CountDfs(int x, vector<vector<int> >*graph, vector<int> *local_mark) {
	(*local_mark)[x] = 1;
	int ret = 1;
	for(int temp : (*graph)[x]) {
		if((*local_mark)[temp] == 0) {
			ret += CountDfs(temp, graph, local_mark);
		}
	}
	return ret;
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
	{
		string temp(argv[4]);
		if(temp == "final") {
			Amazon::Global::final = true;
		} else {
			Amazon::Global::final = false;
		}
	}
	real_time = "RealTime";
	Global::NAMEOFDATASET = "DBLP";

	SIZE_OF_TOP_INNOVATIONS = 500;
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
	if(argc != 5) {
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
	map<string, int> innovation_words;
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
			innovation_words[s] = top_innovations.size();
			word_time_line.burst_start = x;
			top_innovations.push_back(word_time_line);
			if((int)top_innovations.size() == SIZE_OF_TOP_INNOVATIONS) {
				break;
			}
		}
	}
#if 0
	map<string, vector<Review>*> innovators_reviews;
	//	ofstream innovators_out(Amazon::Global::output_directory + "distribution.txt");
	vector<Review> reviews_have_word[SIZE_OF_TOP_INNOVATIONS + 10];
	vector<pair<int, int> > most_innovative(reviews.size());
	//	if(Amazon::Global::state_machine_doc_ratio == false) {
	//		Innovations::FindInnovationsBursts(&reviews, &top_innovations, &innovators_reviews);
	//	} else {
	for(int j = 0; j < (int)reviews.size(); j++) {
		most_innovative[j].first = 0;
		most_innovative[j].second = j;
		is_innovative.push_back(false);
	}
	for(int k = 0; k < (int)top_innovations.size(); k++) {
		WordTimeLine temp = top_innovations[k];
		reviews_have_word[k].clear();
		int index = temp.burst_start;
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
					reviews_have_word[k].push_back(reviews[j]);
					is_innovative[j] = true;
					break;
				}
			}
		}
		innovators_reviews.insert(make_pair(temp.word, &reviews_have_word[k]));
		k++;
	}
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
	for(int k = top_innovations.size() - 1; k < (int)top_innovations.size(); k += 20) {
		int num_of_innovation_reviews = 0;
		map<string, int> innovator_ids;
		map<int, int> pdf_current_experience;
		map<int,int> pdf_final_experience;
		map<string, int> innovation_at_cur_exp;
		int sum_cdf = 0;
		{
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

		}
		{ // Innovation present
			ofstream innovators_cdf_out(Amazon::Global::output_directory + "innovator_present_cdf.txt");
			innovators_cdf_out << (--pdf_current_experience.end())->first + 1 << " " << 0 << endl;
			for(map<int,int>::iterator it = pdf_current_experience.end(); it != pdf_current_experience.begin();) {
				it--;
				sum_cdf += it -> second;
				innovators_cdf_out << it->first << " " << sum_cdf / (double)num_of_innovation_reviews << endl;
			}
		}
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

	/*
	cerr << "STARTING FEYNMAN" << endl;
	// Richard Feynman trace back
	ofstream feynman_fout(Amazon::Global::output_directory + "Feynman.txt");
	for(int k = 0; k < (int)top_innovations.size(); k++) {
		WordTimeLine word_time_line = top_innovations[k];
		string word = word_time_line.word;
		cerr << "starting " << k << " " << word << endl;
		vector<int> indecies_contain_word;
		map<string, int> local_author_id;
		const int H = 1000 * 1000;
		vector<string> rev_local_author_id(H);
		int local_counter = 0;
		vector<vector<int> > graph(H);
		vector<int> local_earliest(H);
		vector<int> fin_time;
		vector<int> local_mark(H);
		vector<vector<int> > comp(H);
		cerr << "BUILDING GRAPH " << endl;
		for(int i = 0; i < (int)reviews.size(); i++) {
			if(reviews[i].authors[0].substr(0, 5) == "Dummy") {
				continue;
			}
			stringstream ss(reviews[i].text);
			bool check = false;
			while(!ss.eof()) {
				string s;
				ss >> s;
				if(s == word) {
					check = true;
					break;
				}
			}
			if(check == false) {
				continue;
			}
			indecies_contain_word.push_back(i);
			for(string author : reviews[i].authors) {
				if(local_author_id.find(author) == local_author_id.end()){
					rev_local_author_id[local_counter] = author;
					local_author_id[author] = local_counter++;
					local_earliest[local_counter - 1] = reviews[i].time.day;
				}
			}
			for(string author1 : reviews[i].authors) {
				for(string author2 : reviews[i].authors) {
					if(author1 == author2) {
						continue;
					}
					int x = local_author_id[author1];
					int y = local_author_id[author2];
					if(local_earliest[x] <= local_earliest[y] && local_earliest[y] == reviews[i].time.day) {
						graph[x].push_back(y);
					}
				}
			}
		}
		cerr << "STARTED SCC" << endl;
		// strongly connected component
		for(int i = 0 ; i < local_counter; i++) {
			if(local_mark[i] == 0) {
				StrongDfs(i, &graph, &local_mark, &fin_time);
			}
		}
		cerr << "FINISHING TIME IS DONE" << endl;
		fill(local_mark.begin(), local_mark.end(), 0);
		int comp_num = 1;
		set<int> comps_to_check;
		for(int i = 0; i < local_counter; i++) {
			if(local_mark[fin_time[i]] == 0) {
				comps_to_check.insert(comp_num);
				StrongDfsRev(fin_time[i], &graph, &local_mark, &comp[comp_num], comp_num);
				comp_num++;
			}
		}
		cerr <<"GOT THE COMPONENTS" << endl;
		for(int i = 0; i < local_counter; i++) {
			for(int x : graph[i]) {
				if(local_mark[x] != local_mark[i])
					comps_to_check.erase(local_mark[x]);
			}
		}
		cerr << "Num of candidate components to check: " << comps_to_check.size() << endl;
		cerr << "FIND THE BEST COMPONENT" << endl;
		int max_people = 0;
		int best_comp = 0;
		int second_max_people = 0;
		int second_best_comp = 0;
		for(int x : comps_to_check) {
			fill(local_mark.begin(), local_mark.end(), 0);
			int temp = comp[x][0];
			int num = CountDfs(temp, &graph, &local_mark);
			if(max_people < num) {
				max_people = num;
				best_comp = x;
			} else if (second_best_comp < num) {
				second_max_people = num;
				second_best_comp = x;
			}
		}
		// Find the intersection of two biggest sets.
		cerr << " Finding the intersection of two biggest sets" << endl;
		fill(local_mark.begin(), local_mark.end(), 0);
		int temp = comp[best_comp][0];
		CountDfs(temp, &graph, &local_mark);
		int intersection = second_max_people;
		if(local_mark[comp[second_best_comp][0]] == 0 ) {
			intersection -= CountDfs(comp[second_best_comp][0], &graph, &local_mark);
		}
		cerr << " LOOK INTO THE BEST COMPONENT" << endl;
		set<string> seed_authors;
		set<string> seed_authors_copy;
		set<int> seed_paper_indecies;
		for(int i = 0; i < (int)comp[best_comp].size() ; i++) {
			seed_authors.insert(rev_local_author_id[comp[best_comp][i]]);
			seed_authors_copy.insert(rev_local_author_id[comp[best_comp][i]]);
		}
		set<string> all_people;
		for(int i = 0; i < (int)indecies_contain_word.size(); i++) {
			int index = indecies_contain_word[i];
			for(string author : reviews[index].authors) {
				all_people.insert(author);
				if(seed_authors.find(author) != seed_authors.end()) {
					seed_paper_indecies.insert(index);
					seed_authors.erase(author);
				}
			}
		}
		int best_index = *seed_paper_indecies.begin();
		// word, number of people on biggest impact, number of people on second biggest impact, intersecion of the two biggest sets,  all the people used that word, a sample text in the biggest component
		feynman_fout << word << " :: " << max_people  << " " << second_max_people << " " << intersection << " " << all_people.size() << " " << reviews[best_index].text << endl;
		// When the best paper was published, when the best burst started
		feynman_fout << reviews[best_index].time.day + 1935 << " " << word_time_line.burst_start + 1935 << endl;
		for(string author : seed_authors_copy) {
			feynman_fout << author << ", ";
		}
		feynman_fout << endl << "________________________________________" << endl;
	}
	 */
	/*
	//Pairwise prediction, equal current exp, different #innovative reviews ->
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
#if 1
	{
		// [a,b]
		cerr << "STARTING  [a,b]" << endl;
		string output_count[30000];
		string output_bin[30000];
		const int SHIFTER = 100;
		const int REL_SIZE = SHIFTER * 2 + 10;
		const bool final = Amazon::Global::final;
		double average[REL_SIZE];
		int denominator = 4;
		for (int numerator = 1; numerator <= denominator; numerator++) {
			vector<int> num_of_innovative_reviews_relative_to_burst(210);
			vector<int> sum_of_innovative_reviews_relative_to_burst(210);
			vector<pair<int, int> > authors_exp_relative_to_burst(210); //(sum, number of authors)
			for(int i = 0; i < REL_SIZE; i++) {
				authors_exp_relative_to_burst[i] = make_pair(0, 0);
			}
			fill(num_of_innovative_reviews_relative_to_burst.begin(), num_of_innovative_reviews_relative_to_burst.end(), 0);
			fill(sum_of_innovative_reviews_relative_to_burst.begin(), sum_of_innovative_reviews_relative_to_burst.end(), 0);
			vector<vector<int> > binary(REL_SIZE, vector<int>(REL_SIZE));
			for(int i = 0; i < REL_SIZE; i++) {
				for(int j = 0; j < REL_SIZE; j++) {
					binary[i][j] = 0;
				}
			}
			int K;
			int alpha = 0;
			double fraction = 0;
			// find K, where K is the least final_exp which half the reviews come from them
			fraction = numerator / (double) denominator;
			fraction = 1 / fraction;
			cerr << "FINDING K for " << fraction << endl;
			{
				vector<int> counter_exp(2510);
				fill(counter_exp.begin(), counter_exp.end(), 0);
				for(int i = 0; i <(int) reviews.size(); i++) {
					counter_exp[reviews[i].final_experience_level]++; // EXP - final
				}
				int index = -1;
				cerr << "GOING INTO THE LOOP" << endl;
				while(fraction * alpha < (int)reviews.size()) {
					index++;
					alpha += counter_exp[index];
				}
				K = index;
			}
			cerr <<"------>" << K << " " << alpha << " " << reviews.size() << endl;
			cerr << "COMPUTING NUM" << endl;
			for(int i = 0; i < (int)reviews.size(); i++) {
				vector<int> relative_burst_times;
				stringstream ss(reviews[i].text);
				string s;
				if(reviews[i].final_experience_level > K) { // EXP - final
					continue;
				}
				int earliest = 300;
				int latest = -1;
				while(!ss.eof()) {
					ss >> s;
					if(innovation_words.find(s) == innovation_words.end()) {
						continue;
					}
					int start = top_innovations[innovation_words[s]].burst_start;
					num_of_innovative_reviews_relative_to_burst[reviews[i].time.day - start + SHIFTER]++;
					pair<int, int> p;
					/*	for(string author : reviews[i].authors) {
						p = authors_exp_relative_to_burst[reviews[i].time.day - start + SHIFTER];
						authors_exp_relative_to_burst[reviews[i].time.day - start + 100] = make_pair(p.first + experience_level[author], p.second + 1); // final exp counted
					}
					 */
					p = authors_exp_relative_to_burst[reviews[i].time.day - start + SHIFTER];
					if(final == true) {
						authors_exp_relative_to_burst[reviews[i].time.day - start + SHIFTER] = make_pair(p.first + reviews[i].final_experience_level, p.second + 1);
					} else {
						authors_exp_relative_to_burst[reviews[i].time.day - start + SHIFTER] = make_pair(p.first + reviews[i].current_experience_level, p.second + 1);
					}
					relative_burst_times.push_back(reviews[i].time.day - start + SHIFTER);
					latest = max(latest, reviews[i].time.day - start + SHIFTER);
					earliest = min(earliest, reviews[i].time.day - start + SHIFTER);
				}
				// at least has one year of intersection
				for(int s = 0; s < latest; s++) {
					for(int t = max(s + 1, earliest); t < 2 * SHIFTER; t++) {
						for(int l = 0; l < (int)relative_burst_times.size(); l++) {
							if(s <= relative_burst_times[l] && relative_burst_times[l] <= t) {
								binary[s][t]++;
								break;
							}

						}
					}
				}
			}

			//		cerr << "COMPUTING SUM" << endl;
			for(int i = 1; i < 2 * SHIFTER; i++) {
				sum_of_innovative_reviews_relative_to_burst[i] = sum_of_innovative_reviews_relative_to_burst[i - 1] + num_of_innovative_reviews_relative_to_burst[i];
			}
			// Does the smaller half side of the papers in the final experience create more than half of the innovation?
			int temp_counter = 0;
			for(int a = 1 ; a < 2 * SHIFTER; a++) {
				for(int b = a + 1; b < 2 * SHIFTER; b++) {
					if(numerator == 1 ) {
						output_count[temp_counter] = SimpleIntToString(a-100) + "\t" + SimpleIntToString(b - SHIFTER);
					}
					int sum_a_b = sum_of_innovative_reviews_relative_to_burst[b] - sum_of_innovative_reviews_relative_to_burst[a - 1];
					output_count[temp_counter] += "\t" + SimpleDoubleToString(sum_a_b / (double)alpha);
					output_bin[temp_counter] += "\t" + SimpleDoubleToString(binary[a][b]/ (double)alpha);
					temp_counter++;
				}
			}
			if(numerator == 4) {
				for(int i = 0; i < 2 * SHIFTER; i++) {
					average[i] = authors_exp_relative_to_burst[i].first / (double)authors_exp_relative_to_burst[i].second;
				}
				string filename;
				if(final == true) {
					filename = Amazon::Global::output_directory + "final_relative_year_usage_all_exp_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations.txt";
				} else {
					filename = Amazon::Global::output_directory + "present_relative_year_usage_all_exp_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations.txt";
				}
				ofstream rel_year_fout(filename.c_str());
				int temp1 = 0, temp2 = 1;
				for(int i = 0; i < 2 * SHIFTER ; i++) {
					temp1 += authors_exp_relative_to_burst[i].first; // adding up the sum of experiences
					temp2 += authors_exp_relative_to_burst[i].second; // number of authors for denominator to divide
					rel_year_fout << i - SHIFTER << " " << temp1 / (double) temp2 << " " << sum_of_innovative_reviews_relative_to_burst[i] << endl;
				}
			}
			// Bucketing weeks to have same size and then averaging over different weeks instead of accumulating the experience over time
			vector<int> median_finder[REL_SIZE];

			if(numerator == denominator) {
				authors_exp_relative_to_burst.clear();
				for(int i = 0; i < REL_SIZE; i++) {
					median_finder[i].clear();
				}
				int week[100 + 10];
				int first_non_empty = 1;
				week[0] = -1100;
				int each_bucket = 20;
				long long sum = 0;
				for(int j = 0; j < REL_SIZE; j++) {
					sum += num_of_innovative_reviews_relative_to_burst[j];
					if(sum > each_bucket) {
						week[first_non_empty++] = j + 1;
						sum = 0;
					}
				}
				for(int i = 0; i < (int)reviews.size(); i++) {
					if(final == true) {
						continue;
					}
					if(final == false) {
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
						int bucket = upper_bound(week, week + first_non_empty,reviews[i].time.day - start + SHIFTER) - week - 1;
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
				string filename;
				if(final == true) {
					filename = Amazon::Global::output_directory + "final_relative_year_usage_bucketed_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
				} else {
					filename = Amazon::Global::output_directory + "current_relative_year_usage_bucketed_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations_coeff" + SimpleDoubleToString(Amazon::Global::state_coeffecient) + ".txt";
				}
				ofstream fout_bucket(filename.c_str());
				fout_bucket << "Bucket_number\tStart_week\tAverage_experience\tMedian_experience" << endl;
				for(int i = 0; i < first_non_empty; i++) {
					if(median_finder[i].size() == 0) {
						median_finder[i].push_back(0);
					}
					fout_bucket << i << "\t" << week[i] << "\t" << authors_exp_relative_to_burst[i].first / (double)authors_exp_relative_to_burst[i].second << " " << median_finder[i][median_finder[i].size() / 2] << endl;
				}
				/**/
			}
		}

		/*
		string filename = Amazon::Global::output_directory + "current_[a,b]_top_" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations.txt";
		ofstream ab_fout(filename.c_str());
		int temp_counter = 0;
		ab_fout << "a\tb\tcq1\tcq2\tcq3\tcq4\tbinq1\tbinq2\tbinq3\binq4" << endl; //cq = count on quartile, binq = binary on quarter
		while(output_count[temp_counter].size() != 0) {
			ab_fout << output_count[temp_counter] << output_bin[temp_counter]<< endl;
			temp_counter++;
		}
		 */
	}
#endif
#if 0
	{
		// [gamma,delta]
		cerr << "STARTING  [gamma,delta]" << endl;
		string output_count[30000];
		string output_bin[30000];
		for(int numerator = 1; numerator <= 4; numerator++) {
			vector<int> num_of_innovative_reviews_relative_to_burst(2510);
			vector<int> sum_of_innovative_reviews_relative_to_burst(2510);
			fill(num_of_innovative_reviews_relative_to_burst.begin(), num_of_innovative_reviews_relative_to_burst.end(), 0);
			fill(sum_of_innovative_reviews_relative_to_burst.begin(), sum_of_innovative_reviews_relative_to_burst.end(), 0);
			vector<int> count_containing_innovation[500 + 10]; // in vector[1] we have the number of occurances of word1 up to a certain year so that we can do a binary search and find for gamma alpha what is happening
			int K;
			int alpha = 0;
			double fraction = 0;
			// find K, where K is the least final_exp which half the reviews come from them
			fraction = numerator / 4.0;
			fraction = 1 / fraction;
			cerr << "FINDING K for " << fraction << endl;
			{
				vector<int> counter_exp(2510);
				fill(counter_exp.begin(), counter_exp.end(), 0);
				for(int i = 0; i <(int) reviews.size(); i++) {
					counter_exp[reviews[i].current_experience_level]++; // EXP - current
				}
				int index = -1;
				cerr << "GOING INTO THE LOOP" << endl;
				while(fraction * alpha < (int)reviews.size()) {
					index++;
					alpha += counter_exp[index];
				}
				K = index;
			}
			cerr <<"------>" << K << " " << alpha << " " << reviews.size() << endl;
			cerr << "COMPUTING NUM" << endl;
			vector<int> bef[500 + 10];
			for(int i = 0; i <= (int)top_innovations.size(); i++) {
				bef[i].push_back(0);
				count_containing_innovation[i].push_back(0);
			}
			for(int i = 0; i < (int)reviews.size(); i++) {
				if(reviews[i].current_experience_level > K) { // EXP - current
					continue;
				}
				stringstream ss(reviews[i].text);
				string s;
				while(!ss.eof()) {
					ss >> s;
					if(innovation_words.find(s) == innovation_words.end()) {
						continue;
					}
					int innovation_index = innovation_words[s];
					if(reviews[i].time.day != bef[innovation_index].back()) {
						count_containing_innovation[innovation_index].push_back(count_containing_innovation[innovation_index].back());
						bef[innovation_index].push_back(reviews[i].time.day);
					}
					count_containing_innovation[innovation_index].back()++;
				}
			}
			vector<vector<int> > num(110, vector<int>(110));
			vector<vector<int> > bin(110, vector<int>(110));
			for(int i = 0; i < (int)top_innovations.size(); i++) {
				int word_size = count_containing_innovation[i].back();
				for(int gamma = 0 ; gamma < 100; gamma++) {
					for(int delta = gamma + 1; delta <= 100; delta++) {
						int lb = (gamma / 100.0) * word_size;
						int ub = (delta / 100.0) * word_size;
						int lb_index = upper_bound(count_containing_innovation[i].begin(), count_containing_innovation[i].end(), lb) - count_containing_innovation[i].begin();
						lb_index --;
						int ub_index = lower_bound(count_containing_innovation[i].begin(), count_containing_innovation[i].end(), ub) - count_containing_innovation[i].begin();
						num[gamma][delta] += count_containing_innovation[i][ub_index] - count_containing_innovation[i][lb_index];
					}
				}
			}
			int count[510];
			for(int i = 0; i < (int)reviews.size(); i++) {
				if(reviews[i].current_experience_level > K) { // EXP - current
					continue;
				}
				stringstream ss(reviews[i].text);
				string s;
				set<pair<int, int> > fraction_windows;
				while(!ss.eof()) {
					ss >> s;
					if(innovation_words.find(s) == innovation_words.end()) {
						continue;
					}
					int innovation_index = innovation_words[s];
					count[innovation_index]++;
					int ub = lower_bound(count_containing_innovation[innovation_index].begin(), count_containing_innovation[innovation_index].end(), count[innovation_index]) - count_containing_innovation[innovation_index].begin();
					int e = 100 * (count_containing_innovation[innovation_index][ub] / (double)count_containing_innovation[innovation_index].back());
					int st = 100 * (count_containing_innovation[innovation_index][ub - 1] / (double)count_containing_innovation[innovation_index].back());
					for(int gamma = 0 ; gamma <= e; gamma++) {
						for(int delta = max(gamma + 1, st); delta <= 100; delta++) {
							fraction_windows.insert(make_pair(gamma, delta));
						}
					}
				}
				for(pair<int, int> p : fraction_windows) {
					bin[p.first][p.second]++;
				}
			}
			// Does the smaller half side of the papers in the final experience create more than half of the innovation?
			int temp_counter = 0;
			for(int gamma = 0 ; gamma < 100; gamma++) {
				for(int delta = gamma + 1; delta <= 100; delta++) {
					if(numerator == 1) {
						output_count[temp_counter] = SimpleDoubleToString(gamma / 100.0) + "\t" + SimpleDoubleToString(delta / 100.0);
					}
					output_count[temp_counter] += "\t" + SimpleDoubleToString(num[gamma][delta] / (double)alpha);
					output_bin[temp_counter] += "\t" + SimpleDoubleToString(bin[gamma][delta]/ (double)alpha);
					temp_counter++;
				}
			}
		}
		string filename = Amazon::Global::output_directory + "current_[gamma,delta]_top_" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations.txt";
		ofstream ab_fout(filename.c_str());
		int temp_counter = 0;
		ab_fout << "gamma\tdelta\tcq1\tcq2\tcq3\tcq4bq1\tbq2\tbq3\tbq4" << endl; //cq = count on quartile, binq = binary on quarter
		while(output_count[temp_counter].size() != 0) {
			ab_fout << output_count[temp_counter] << output_bin[temp_counter] << endl;
			temp_counter++;
		}
	}
#endif
#if 0
	{
		// Comparison with "No country for old men"
		int K;
		int K_bef = 0;
		int num[5][2000];
		int denom[5][2000];
		memset(num, 0, sizeof num);
		for(int numerator = 1; numerator <= 4; numerator++) {
			map<string, int> cur_exp;

			int alpha = 0;
			double fraction = 0;
			// find K, where K is the least final_exp which half the reviews come from them
			fraction = numerator / 4.0;
			fraction = 1 / fraction;
			cerr << "FINDING K for " << fraction << endl;
			{
				vector<int> counter_exp(2510);
				fill(counter_exp.begin(), counter_exp.end(), 0);
				for(int i = 0; i <(int) reviews.size(); i++) {
					counter_exp[reviews[i].final_experience_level]++; // EXP - final
				}
				int index = -1;
				cerr << "GOING INTO THE LOOP" << endl;
				while(fraction * alpha < (int)reviews.size()) {
					index++;
					alpha += counter_exp[index];
				}
				K = index;
			}
			cerr <<"------>" << K_bef << " " << K << " " << alpha << " " << reviews.size() << endl;
			cerr << "COMPUTING NUM" << endl;
			for(int i = 0; i < (int)reviews.size(); i++) {
				vector<int> relative_burst_times;
				stringstream ss(reviews[i].text);
				string s;
				for(string author : reviews[i].authors) {
					cur_exp[author]++;
				}
				//				if(reviews[i].final_experience_level > K || reviews[i].final_experience_level <= K_bef) { // Only the max final_exp added
				//					continue;
				//				}
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
					//					num[numerator][cur_exp[reviews[i].final_exp_best_author]]++; // Only the max final_exp added
					for(string author : reviews[i].authors) { // Adding all the authors effect
						if(experience_level[author] <= K && experience_level[author] > K_bef) {
							num[numerator][cur_exp[author]]++;
						}
					}
					break;
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

		string filename = Amazon::Global::output_directory + "cristian_probability_binary_comparison_top" + SimpleIntToString(SIZE_OF_TOP_INNOVATIONS) + "_innovations.txt";
		ofstream cristian_fout(filename.c_str());
		for(int i = 1; i <= 4; i++) {
			for(int j = 0; j < 1000; j++) {
				cristian_fout << i << " " << j << " " << num[i][j] / ((double)denom[i][j] + 1) << endl;
			}
		}
	}
#endif
	return 0;
}

