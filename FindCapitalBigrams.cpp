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
// Langauge innovations
string MODE;

map<string, int> capital_bigrams, all_bigrams_lowered;
string filename;
string burst_mode;
string real_time;

string RemoveStopWords2(string s) {
	string ret = "";
	stringstream ss(s);
	while(!ss.eof()){
		ss >> s;
		if(s == "") {
			continue;
		}
		if(stop_words.find(SimpleToLower(s)) != stop_words.end()) {
			continue;
		}
		ret += s + " ";
		s = "";
	}
	return ret;
}

string MakeBigram2(string raw_input) {
	string ret = "";
	stringstream ss(raw_input);
	string prev, current;
	ss >> prev;
	while(ss >> current) {
		ret += prev + current + " ";
		all_bigrams_lowered[SimpleToLower(prev + current)]++;
		if(isupper(prev[0]) && isupper(current[0])) {
			capital_bigrams[prev[0] + SimpleToLower(prev.substr(1)) + current[0] + SimpleToLower(current.substr(1))]++;
		}
		prev = current;
	}
	return ret;
}

bool ReadOneReview2(std::ifstream& fin, vector<Review> *reviews) {
	string raw_input;
	Review review;
	if (getline(fin, raw_input)) {
		review.product_id = GetField(raw_input);
		getline(fin, raw_input);
		review.product_title = RemoveStopWords2(RemoveAllSymbols(GetField(raw_input)));
		getline(fin, raw_input);
		review.price = GetField(raw_input);
		getline(fin, raw_input);
		review.user_id = GetField(raw_input);
		getline(fin, raw_input);
		review.profile_name = GetField(raw_input);
		do {
			getline(fin, raw_input);
			review.helpfulness = GetField(raw_input);
		} while (review.helpfulness == "THIS INPUT IS TRASH");
		getline(fin, raw_input);
		review.score = GetField(raw_input);
		getline(fin, raw_input);
		int time_int = atoi((GetField(raw_input)).c_str());
		time_t review_time(time_int);
		if (review_time == -1) {
			getline(fin, raw_input);
			getline(fin, raw_input);
			getline(fin, raw_input);
			return SUCCESS;
		}
		review.time = MyTime(localtime(&review_time));
		getline(fin, raw_input);
		review.summary = RemoveStopWords2(RemoveAllSymbols(SimpleToLower(GetField(raw_input))));
		getline(fin, raw_input);
		review.text = review.product_title + " " + RemoveStopWords2(RemoveAllSymbols(GetField(raw_input))) + " ";
		review.text = MakeBigram2(review.text);
		getline(fin, raw_input);
		if(Amazon::Global::remove_unknown == false ||
				(Amazon::Global::remove_unknown == true && review.user_id != "unknown")) {
			reviews->push_back(review);
		}
		return SUCCESS;
	}
	return FAIL;
}

bool ReadOneRedditReview2(std::ifstream& fin, vector<Review> *reviews) {
	string raw_input;
	Review review;
	if (getline(fin, raw_input)) {
		review.product_id = GetField(raw_input);

		getline(fin, raw_input);
		review.profile_name = GetField(raw_input);
		getline(fin, raw_input);
		review.user_id = GetField(raw_input);

		getline(fin, raw_input);
		int time_int = atoi((GetField(raw_input)).c_str());
		time_t review_time(time_int);
		if (review_time == -1) {
			getline(fin, raw_input);
			getline(fin, raw_input);
			getline(fin, raw_input);
			return SUCCESS;
		}
		review.time = MyTime(localtime(&review_time));
		getline(fin, raw_input);
		review.text = RemoveStopWords2(RemoveAllSymbols(GetField(raw_input))) + " ";
		review.text = MakeBigram2(review.text);
		getline(fin, raw_input);
		if(Amazon::Global::remove_unknown == false ||
				(Amazon::Global::remove_unknown == true && review.user_id != "[deleted]")) {
			reviews->push_back(review);
		}
	/*	cerr << review.product_id << endl;
		cerr << review.profile_name << endl;
		cerr << review.user_id << endl;
		cerr << review.time.ToString() << endl;
		cerr << review.text << endl;
		exit(0);*/
		return SUCCESS;
	}
	return FAIL;
}

void initialize(char *argv[]) {
	filename = argv[1];
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


	cerr << Global::NAMEOFDATASET <<endl;


	//Remove unknown reviews
	Amazon::Global::remove_unknown = true;

	//Output Directory

	Amazon::Global::output_directory = "../Output_All/"  + Global::NAMEOFDATASET + "_bursts/RealTime/MaxBenefit/" + Global::NAMEOFDATASET + "_";

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
	if(argc != 2) {
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
			if(!ReadOneRedditReview2(fin, &reviews)) {
				break;
			}
		} else {
			if (!ReadOneReview2(fin, &reviews)) {
				break;
			}
		}
	}
	string filename = Amazon::Global::output_directory + "capital_bigrams2.txt";
	ofstream fout(filename.c_str());
	for (pair<string, int> my_pair : capital_bigrams) {
		string temp = SimpleToLower(my_pair.first);
		int total_usage = all_bigrams_lowered[temp];
		double ratio = (double)my_pair.second / total_usage;
		if(ratio > 0.6 && total_usage > 30) {
			fout << my_pair.first << endl;
		}
	}
	cerr << "trying to finish it!" << endl;
	return 0;
}
