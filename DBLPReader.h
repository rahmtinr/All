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

bool ReadOneRecord(std::ifstream& fin, vector<DBLPRecord> *Records) {
	string raw_input;
	DBLPRecord record;
	if (getline(fin, raw_input)) {
		record.title = RemoveAllSymbols(SimpleToLower(GetField(raw_input)));
		getline(fin, raw_input);
		record.venue = RemoveAllSymbols(SimpleToLower(GetField(raw_input)));
		getline(fin, raw_input);
		record.year = SimpleStringToInt(raw_input);

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


#endif /* DBLPREADER_H_ */

