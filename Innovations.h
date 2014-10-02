/*
 * Innovations.h
 *
 *  Created on: Oct 1, 2014
 *      Author: rahmtin
 */
#ifndef INNOVATIONS_H
#define INNOVATIONS_H

#include<fstream>
#include<map>
#include<set>
#include<sstream>
#include<string>
#include<vector>

#include "MyUtility.h"
#include "Reviews.h"

using namespace std;

class Innovator {
public:
	string word;
	string user_id;
	int num_of_reviews;
	int experience_level;
	Innovator() {
		num_of_reviews = 0;
		experience_level = 0;
	}
	bool operator < (const Innovator &other) const {
		return user_id < other.user_id;
	}
};


class Innovations {
public:
	static set<string> dictionary;
	static map<string, int> counter_for_reviewer;
	static string NAMEOFTHEDATASET;
	static map<int, int> distribution_for_count_of_reviewer;

	//Learn Dictionary from the first len reviews
	static void LearnDictionary(int start, int end, vector<Review> *reviews) {
		string word;
		for (int i = start; i < end; i++ ) {
			stringstream ss((*reviews)[i].text);
			while (ss.good()) {
				ss >> word;
				dictionary.insert(word);
			}
		}
	}

	static bool PopWordAddToDictionary(string word, vector<Review> *review_history,
			vector<pair<string, vector<Review> > > *innovations) {
		set<string> products_that_have_this_word;
		set<string> users_that_have_used_this_word;
		if (review_history->size() < 8) {
			return false;
		}
		for (int i = 0; i < (int)review_history->size(); i++) {
			products_that_have_this_word.insert((*review_history)[i].product_id);
			users_that_have_used_this_word.insert((*review_history)[i].user_id);
		}
		if (products_that_have_this_word.size() < 8) {
			return false;
		}
		if (users_that_have_used_this_word.size() < 8) {
			return false;
		}
		innovations->push_back(make_pair(word, *review_history));
		return true;
	}

	static void FindInnovations(int start, vector<Review> *reviews, vector<pair<string, vector<Review> > > *innovations) {
		map<string, vector<Review> > new_words;
		string word;
		vector<Review> temp;
		for (int i = start; i < (int)reviews->size(); i++) {
			stringstream ss((*reviews)[i].text);
			while (ss.good()) {
				ss >> word;
				if(dictionary.find(word) != dictionary.end()) {
					continue;
				}
				if (new_words.find(word) == new_words.end()) {
					temp.clear();
					temp.push_back((*reviews)[i]);
					new_words[word] = temp;
				} else {
					temp = new_words.find(word)->second;
					if (temp[temp.size() - 1] == (*reviews)[i]) {
						continue;
					}
					if (TimeDifferenceInMonth(temp[0].time, (*reviews)[i].time) > 6) {
						if (PopWordAddToDictionary(word, &temp, innovations) == true) {
							innovations->push_back(make_pair(word, temp));
						}
						new_words.erase(word);
						dictionary.insert(word);
					} else {
						temp.push_back((*reviews)[i]);
						new_words[word] = temp;
					}
				}
			}
		}
		for (auto x : new_words) {
			word = x.first;
			temp = x.second;
			if (TimeDifferenceInMonth(temp[0].time, temp[temp.size()-1].time) > 6) {
				PopWordAddToDictionary(word, &temp, innovations);
			}
		}
		dictionary.clear();
	}



	static void FindNumOfReviews(vector<pair<string, vector<Review> > > *innovations,
			set<Innovator> *innovators, vector<Review> *reviews) {
		for (int i = 0; i < (int)reviews->size(); i++) {
			for (int j = 0; j < (int)innovations->size(); j++) {
				if((*reviews)[i].user_id == (*innovations)[j].second[0].user_id) {
					Innovator temp;
					temp.user_id = (*reviews)[i].user_id;
					if(innovators->find(temp) != innovators->end()) {
						temp = *(innovators->find(temp));
						innovators->erase(temp);
					}
					temp.num_of_reviews++;
					if((*innovations)[j].second[0] == (*reviews)[i]) {
						temp.experience_level = temp.num_of_reviews;
					}
					temp.word = (*innovations)[j].first;
					innovators->insert(temp);
				}
			}
		}
	}

	static void AnalyseInnovation(vector<pair<string, vector<Review> > > *innovations, vector<Review> *reviews) {
		set<Innovator> innovators;
		FindNumOfReviews(innovations, &innovators, reviews);
		counter_for_reviewer.clear();
		distribution_for_count_of_reviewer.clear();
		ofstream fout("../Output_All/innovators_distribution_" + NAMEOFTHEDATASET + ".txt");
		int num_of_innovators = innovators.size();
		for ( Innovator innovator : innovators) {
			distribution_for_count_of_reviewer[innovator.num_of_reviews]++;
		}
		for (pair<int, int> num_of_review_to_num_of_people : distribution_for_count_of_reviewer ){
			fout << num_of_review_to_num_of_people.first << " " << num_of_review_to_num_of_people.second/(double)num_of_innovators << endl;
		}
		fout.close();
		ofstream innovators_out("../Output_All/innovators_" + NAMEOFTHEDATASET +".txt");
		for (pair<string, vector<Review> > innovation : *innovations ) {
			innovators_out << innovation.first << ":" <<endl;
			int bound = 0;
			for(Review review : innovation.second) {
				innovators_out << review.score << endl;
				innovators_out << review.time.month + 1 << "/" <<review.time.day << "/" << review.time.year << endl;
				innovators_out << review.text << endl;
				bound++;
				if(bound == 5){
					break;
				}
			}
			innovators_out << " __________________________ " <<endl;
		}
	}


};

set<string> Innovations::dictionary;
map<string, int> Innovations::counter_for_reviewer;
string Innovations::NAMEOFTHEDATASET;
map<int, int> Innovations::distribution_for_count_of_reviewer;

#endif /* INNOVATIONS_H_ */

