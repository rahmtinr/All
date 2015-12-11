/*
 * ReviewReader.h
 *
 *  Created on: Oct 1, 2014
 *      Author: rahmtin
 */

#ifndef REVIEWREADER_H_
#define REVIEWREADER_H_

#include<ctime>
#include<cstdlib>
#include<fstream>
#include<set>
#include<string>

#include "AmazonUtility.h"

using namespace std;

set<string> stop_words;

string RemoveStopWords(string s) {
	string ret = "";
	stringstream ss(s);
	while(!ss.eof()){
		ss >> s;
		if(s == "") {
			continue;
		}
		if(stop_words.find(s) != stop_words.end()) {
			continue;
		}
		ret += s + " ";
		s = "";
	}
	return ret;
}

string GetField(string raw_input) {
	int delimeter = raw_input.find(":");
	if (delimeter == std::string::npos) {
		return "THIS INPUT IS TRASH";
	}
	if(delimeter + 2 == raw_input.size()) {
		return "";
	}
	return raw_input.substr(delimeter + 2);
}

string RemoveAllSymbols(string raw_input) {
	string ret = "";
	for (int i = 0; i < (int)raw_input.size(); i++) {
		if(raw_input[i] == '&' && i+5 < (int)raw_input.size() && raw_input.substr(i,6) == "&quot;") {
			i += 5;
			continue;
		}

		if(isalpha(raw_input[i]) || isspace(raw_input[i]) || isdigit(raw_input[i])) {
			ret += raw_input[i];
		}
		if(raw_input[i] == ',' || raw_input[i]=='.') {
			ret += ' ';
		}
	}
	return ret;
}

string MakeBigram(string raw_input) {
	string ret = "";
	stringstream ss(raw_input);
	string prev, current;
	ss >> prev;
	while(ss >> current) {
		ret += prev + "-" + current + " ";
		prev = current;
	}
	return ret;
}

bool ReadOneReview(std::ifstream& fin, vector<Review> *reviews) {
	string raw_input;
	Review review;
	if (getline(fin, raw_input)) {
		review.product_id = GetField(raw_input);
		getline(fin, raw_input);
		review.product_title = RemoveStopWords(RemoveAllSymbols(SimpleToLower(GetField(raw_input))));
		getline(fin, raw_input);
		review.price = GetField(raw_input);
		if(Amazon::Global::brand == true) {
			getline(fin, raw_input);
			review.product_brand = RemoveStopWords(RemoveAllSymbols(SimpleToLower(GetField(raw_input))));
		}
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
            if(Amazon::Global::brand == false) {
    			getline(fin, raw_input);
            }
			return SUCCESS;
		}
		review.time = MyTime(localtime(&review_time));
		getline(fin, raw_input);
		review.summary = RemoveStopWords(RemoveAllSymbols(SimpleToLower(GetField(raw_input))));
		getline(fin, raw_input);
		review.text = review.product_title + " " + RemoveStopWords(RemoveAllSymbols(SimpleToLower(GetField(raw_input)))) + " ";
		if(Amazon::Global::bigram == true) {
			review.text = MakeBigram(review.text);
		}
		if(Amazon::Global::brand == false) {
			if(Amazon::Global::remove_unknown == false ||
					(Amazon::Global::remove_unknown == true && review.user_id != "unknown")) {
				getline(fin, raw_input);
				reviews->push_back(review);
			}
		} else {
			if(review.product_brand != "") {
				review.user_id = review.product_brand;
				review.text = review.product_title;
				reviews->push_back(review);
			}
		}
		return SUCCESS;
	}
	return FAIL;
}

bool ReadOneRedditReview(std::ifstream& fin, vector<Review> *reviews) {
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
		review.text = RemoveStopWords(RemoveAllSymbols(SimpleToLower(GetField(raw_input)))) + " ";
		if(Amazon::Global::bigram == true) {
			review.text = MakeBigram(review.text);
		}
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
#endif /* REVIEWREADER_H_ */
