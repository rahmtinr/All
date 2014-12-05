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
#include<string>

#include "AmazonUtility.h"

using namespace std;

string GetField(string raw_input) {
	int delimeter = raw_input.find(":");
	if (delimeter == std::string::npos) {
		return "THIS INPUT IS TRASH";
	}
	return raw_input.substr(delimeter+2);
}

string RemoveAllSymbols(string raw_input) {
	string ret = "";
	for (int i = 0; i < (int)raw_input.size(); i++) {
		if(isalpha(raw_input[i]) || isspace(raw_input[i]) || isdigit(raw_input[i])) {
			ret += raw_input[i];
		}
		if(raw_input[i] == ',' || raw_input[i]=='.') {
			ret += ' ';
		}
	}
	return ret;
}

bool ReadOneReview(std::ifstream& fin, vector<Review> *reviews) {
	string raw_input;
	Review review;
	if (getline(fin, raw_input)) {
		review.product_id = GetField(raw_input);
		getline(fin, raw_input);
		review.product_title = RemoveAllSymbols(SimpleToLower(GetField(raw_input)));
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
		review.summary = RemoveAllSymbols(SimpleToLower(GetField(raw_input)));
		getline(fin, raw_input);
		review.text = review.product_title + " " + RemoveAllSymbols(SimpleToLower(GetField(raw_input))) + " ";
		getline(fin, raw_input);
		if(Amazon::Global::remove_unknown == false ||
				(Amazon::Global::remove_unknown == true && review.user_id != "unknown")) {
			reviews->push_back(review);
		}
		return SUCCESS;
	}
	return FAIL;
}


#endif /* REVIEWREADER_H_ */
