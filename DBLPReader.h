/*
 * ReadDBLP.cpp
 *
 *  Created on: Dec 22, 2014
 *      Author: Rahmtin
 */


#ifndef DBLPREADER_H_
#define DBLPREADER_H_

#include<ctime>
#include<cstdlib>
#include<fstream>
#include<string>

#include "AmazonUtility.h"
#include "MyUtility.h"
#include "ReviewReader.h"

using namespace std;


class DBLPRecord {
public:
	string title;
	vector<string> authors;
	string venue;
	int year;
};

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

bool ReadOneRecord(std::ifstream& fin, vector<DBLPRecord> *Records) {
	string raw_input;
	DBLPRecord record;
	if (getline(fin, raw_input)) {
		record.title = RemoveStopWords(RemoveAllSymbols(SimpleToLower(GetField(raw_input))));
		getline(fin, raw_input);
		record.venue = RemoveAllSymbols(SimpleToLower(GetField(raw_input)));
		getline(fin, raw_input);
		record.year = SimpleStringToInt(GetField(raw_input));
		while(true) {
			getline(fin, raw_input);
			if(raw_input == "") {
				break;
			}
			record.authors.push_back(GetField(raw_input));
		}
		Records->push_back(record);
		return SUCCESS;
	}
	return FAIL;
}

void ReadAllRecords(std::ifstream& fin, vector<DBLPRecord> *records,
		vector<Review> *reviews, set<pair<int, int> > *edges, map<string, int> *author_id,
		map<int, string> *rev_author_id) {
	int counter = 1;
	// Read input.
	while (true) {
		if(!ReadOneRecord(fin, records)) {
			break;
		}
		if(records->size() == 0) {
			continue;
		}
		Review review;
		DBLPRecord record = records->back();
		review.time.year = 0;
		review.time.month = 0;
		// Changing years to days
		review.time.day = (record.year - 1935); // 1936 is the first paper so we change

		review.time.epoch_time = (record.year - 1935) * 60 * 60 * 24;
		review.text = record.title;
		review.product_title = record.venue;
		review.product_id = record.venue;
		review.helpfulness = "0/1";
		for(string author : record.authors) {
			review.authors.push_back(author);
		}
		reviews->push_back(review);
		Amazon::Global::min_year = min(Amazon::Global::min_year, record.year);
		Amazon::Global::max_year = max(Amazon::Global::max_year, record.year);

		for(int i = 0 ; i < (int)record.authors.size(); i++) {
			if(author_id->find(record.authors[i]) == author_id->end()) {
				(*author_id)[record.authors[i]] = counter ++;
				(*rev_author_id)[counter - 1] = record.authors[i];
			}
			int x = (*author_id)[record.authors[i]];
			for (int j = i + 1; j < (int)record.authors.size(); j++) {
				if(author_id->find(record.authors[j]) == author_id->end()) {
					(*author_id)[record.authors[j]] = counter++;
					(*rev_author_id)[counter - 1] = record.authors[j];
				}
				int y = (*author_id)[record.authors[j]];
				if(y < x) {
					swap(x,y);
				}
				edges->insert(make_pair(x,y));
			}
		}
	}
	cerr << "Min year: " << Amazon::Global::min_year << endl;
	cerr << "Max year: " << Amazon::Global::max_year << endl;
	cerr << "Number of edges: " << edges->size() << endl;
	cerr << "Number of authors: " << author_id->size() << endl;

}

#endif /* DBLPREADER_H_ */

