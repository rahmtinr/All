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
int SIZE_OF_TOP_INNOVATIONS;
bool CREATE_RANDOM_BASELINE;
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
	for(int i = 0; i < (int)reviews.size(); i++) {
		for(int j = 0; j < (int)reviews[i].authors.size(); j++) {
			if(fake_nodes.find(author_id[reviews[i].authors[j]]) != fake_nodes.end()) {
				reviews[i].authors.erase(reviews[i].authors.begin() + j);
				j--;
			}
		}
	}
	for(int i = 1; i < (int) reviews.size(); i++) {
		if(reviews[i].time == reviews[i-1].time) {
			reviews[i].index = reviews[i-1].index;
		} else {
			reviews[i].index = i;
		}
	}
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
		swap(reviews[i].authors[0], reviews[i].authors[nominated_author]);
	}
	ofstream entire_dataset_distribution_fout("../Output_All/DBLP/Bursts/DocRatio/distribution.txt");
	for(int i = 0; i < (int) reviews.size(); i++) {
		reviews[i].final_experience_level = experience_level[reviews[i].authors[0]];
		entire_dataset_distribution_fout << reviews[i].current_experience_level << " "
				<< reviews[i].final_experience_level << endl;
	}
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
		if(Amazon::Global::state_machine_doc_ratio == true && longest_one > 3){
			burst_innovation.insert(word_states);
		}
	}
	ofstream fout(Amazon::Global::output_directory + "timeline.txt");
	cerr << "Size of innovations found: " << burst_innovation.size() << endl;
	int x = 0;
	for(WordTimeLine word_time_line : burst_innovation) {
		//		cerr << word_time_line.word << " " << word_time_line.difference << endl;
		x++;
		if (x == SIZE_OF_TOP_INNOVATIONS) {
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
	ofstream innovation_burst_year_out("./DBLPparser/words_start_burst.txt");
	for(WordTimeLine word_time_line : top_innovations) {
		innovation_burst_year_out << word_time_line.word << "   " << word_time_line.burst_start << endl;
	}
	cerr << "trying to finish it!" << endl;
	return 0;
}
