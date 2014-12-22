/*
 * Parser.cpp
 *
 *  Created on: Dec 18, 2014
 *      Author: rahmtin
 */

#include<algorithm>
#include<fstream>
#include<iostream>
#include<map>
#include<set>
#include<sstream>
#include<vector>
#include "../MyUtility.h"

using namespace std;

map<string, int> authors_id;
map<int, string> rev_authors_id;



struct Paper {
	vector<string> authors;
	string title;
	string year;
	string venue;

};

vector<Paper> papers;


int main() {
	ifstream fin("../../2dblp.xml");
	string s;
	getline(fin, s);
//	cerr << s << endl;
	getline(fin, s);
//	cerr << s << endl;
	getline(fin, s);
//	cerr << s << endl;
	int counter = 3;
	while(getline(fin, s)) {
	/*	int mod = 0;
		bool dirty = false;
		for( int i = 0; i <s.length();i++){
			cerr<<s[i];
			dirty = true;
			if(s[i]=='/') {
				mod = 1;
			}
			if(s[i]=='>') {
				if(mod==1) {
					mod = 0;
					dirty = false;
					cerr << endl;
				}
			}
		}
		if(dirty) {
			cerr<< endl;
		}
		*/
		counter++;
		if(s=="" || s.length() < 2) {
			continue;
		}
		if(s == "</dblp>") {
			break;
		}
		string temp = s.substr(0,4);
		if(temp == "<www" || temp == "<mas" || temp == "<phd" || temp == "<inc" || temp == "<boo") {
			temp.insert(++temp.begin(), '/');
			while(s.substr(0,5) != temp) {
				counter++;
				getline(fin, s);
			}
			continue;
		}
		Paper paper;
		temp.insert(++temp.begin(), '/');
//		cerr << counter << endl;
//		cerr << "--------------------------------------------" << s << endl;
		int temp_c = 0;
		while(s.substr(0,5) != temp) {
			temp_c ++;
	//		cerr <<" ---- > " << s << endl;
			if(s.substr(0, 11) == "<booktitle>") {
				int loc = 11;
				for(; loc < s.length(); loc++) {
					if(s[loc] == '<') {
						paper.venue = s.substr(11, loc - 11);
						break;
					}
				}
			}
			if(s.substr(0, 6) == "<year>") {
				int loc = 6;
				for(; loc < s.length(); loc++) {
					if(s[loc] == '<') {
						paper.year = s.substr(6, loc - 6);
						break;
					}
				}
			}
			if(s.substr(0, 8) == "<author>" || s.substr(0, 8) == "<editor>") {
				int loc = 8;
				for(; loc < s.length(); loc++) {
					if(s[loc] == '<') {
						paper.authors.push_back(s.substr(8, loc - 8));
						break;
					}
				}
			}
			if(s.substr(0, 7) == "<title>") {
				int loc = 7;
				for(; loc < s.length(); loc++) {
					if(s[loc] == '<') {
						paper.title = s.substr(7, loc - 7);
						break;
					}
				}
			}
			getline(fin, s);
			counter ++;
		}
		cout << "Title: " << paper.title << endl;
		cout << "Venue: " << paper.venue << endl;
		cout << "Year: "  << paper.year << endl;
		for(string author : paper.authors) {
			cout << "Author: " << author << endl;
		}
		cout << endl;
		papers.push_back(paper);
	}

	return 0;
}
