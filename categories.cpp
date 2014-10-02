/*
 * categories.cpp
 *
 *  Created on: Sep 15, 2014
 *      Author: rahmtin
 */

#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdio>
#include<cstdlib>
#include<vector>
#include<cstring>
#include<string>
#include<set>
#include<map>
#include<algorithm>

using namespace std;

map<string, int> num_of_items_that_have_label;
vector<pair<int, string>> vec;

string trim(std::string const& str)
{
	if(str == ""){
		return str;
	}
    std::size_t first = str.find_first_not_of(' ');
    if(first==string::npos){
    	return "";
    }
    std::size_t last  = str.find_last_not_of(' ');
    return str.substr(first, last-first+1);
}

int main() {
	ifstream fin("categories.txt");
	string s;
	set<string> labels;
	string label;
	getline(fin,s);
	while(fin.good()) {
		if(s[0] != ' ') {
			for (auto x : labels) {
				num_of_items_that_have_label[x]++;
			}
			labels.clear();
			if(!fin.good())
				break;
			getline(fin,s);
			cerr << s <<endl;
		}
		stringstream ss(s);
		while (ss.good()) {
			getline(ss, label, ',');
			labels.insert(trim(label));
		}
		getline(fin, s);
		cerr << fin.good() <<endl;
	}
	ofstream fout("../Output_All/num_in_categories.txt");
	for (auto x : num_of_items_that_have_label) {
		vec.push_back(make_pair(x.second, x.first));
	}
	sort(vec.begin(), vec.end());
	for ( int i = vec.size() -1 ; i >=0; i--) {
		fout << vec[i].second << " " << vec[i].first <<endl;
	}
}
