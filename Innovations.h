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
#include<sstream>

#include "AmazonUtility.h"
#include "MyUtility.h"
#include "Reviews.h"
#include "UserRelated.h"

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
	static map<int, int> distribution_for_experience_level;
	static map<int, int> distribution_for_num_of_reviews;
	static vector<int> numbers_for_appearances;
	static vector<int> numbers_for_products;
	static vector<int> numbers_for_authors;

	//Learn Dictionary from the first len reviews
	static void LearnDictionary(int start, int end, vector<Review> *reviews) {
		dictionary.clear();
		string word;
		for (int i = start; i < end; i++ ) {
			stringstream ss((*reviews)[i].text);
			while (ss.good()) {
				ss >> word;
				dictionary.insert(word);
			}
		}
	}

	static void PopWordAddToDictionary(string word, vector<Review> *review_history,
			vector<pair<string, vector<Review> > > *innovations) {
		for(int i = 0; i < (int) numbers_for_appearances.size(); i++) {
			for(int j = 0; j < (int) numbers_for_products.size(); j++) {
				for(int k = 0; k < (int) numbers_for_authors.size(); k++) {
					set<string> products_that_have_this_word;
					set<string> users_that_have_used_this_word;
					if ((int) review_history->size() < numbers_for_appearances[i]) {
						continue;
					}
					for (int i = 0; i < (int)review_history->size(); i++) {
						products_that_have_this_word.insert((*review_history)[i].product_id);
						users_that_have_used_this_word.insert((*review_history)[i].user_id);
					}
					if ((int) products_that_have_this_word.size() < numbers_for_products[j]) {
						continue;
					}
					if ((int) users_that_have_used_this_word.size() < numbers_for_authors[k]) {
						continue;
					}
					innovations[(numbers_for_authors.size()*numbers_for_products.size())*i + numbers_for_authors.size()*j + k]
					               .push_back(make_pair(word, *review_history));
				}
			}
		}
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
						PopWordAddToDictionary(word, &temp, innovations);
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
		for(int k = 0; k < 100; k++) {
			for (int i = 0; i < (int)reviews->size(); i++) {
				for (int j = 0; j < (int)innovations[k].size(); j++) {
					if((*reviews)[i].user_id == innovations[k][j].second[0].user_id) {
						Innovator temp;
						temp.user_id = (*reviews)[i].user_id;
						if(innovators->find(temp) != innovators->end()) {
							temp = *(innovators->find(temp));
							innovators->erase(temp);
						}
						temp.num_of_reviews++;
						if(innovations[k][j].second[0] == (*reviews)[i]) {
							temp.experience_level = temp.num_of_reviews;
						}
						temp.word = innovations[k][j].first;
						innovators->insert(temp);
					}
				}
			}
		}
	}

	static void AnalyseInnovation(vector<pair<string, vector<Review> > > *innovations, vector<Review> *reviews) {
		set<Innovator> innovators;
		FindNumOfReviews(innovations, &innovators, reviews);
		counter_for_reviewer.clear();
		distribution_for_num_of_reviews.clear();
		distribution_for_experience_level.clear();
		map<int, double> distribution_for_entire_data_set;
		UserDistributionBasedOnNumberOfReviews(reviews, &distribution_for_entire_data_set);
		for(int i = 0; i < (int) numbers_for_appearances.size(); i++) {
			for(int j = 0; j < (int) numbers_for_products.size(); j++) {
				for(int k = 0; k < (int) numbers_for_authors.size(); k++) {
					if(innovations[(numbers_for_authors.size()*numbers_for_products.size())*i + numbers_for_authors.size()*j + k].size() == 0){
						cerr << " CONTINUE" << endl;
						continue;
					}
					ofstream fout("../Output_All/innovators_distribution_" + ToString(numbers_for_appearances[i]) + "_"
							+ ToString(numbers_for_products[j]) + "_"
							+ ToString(numbers_for_authors[k]) + "_" + Global::NAMEOFDATASET + ".txt");
					cerr << "------------------------>   ../Output_All/innovators_distribution_" + ToString(numbers_for_appearances[i]) + "_"
							+ ToString(numbers_for_products[j]) + "_"
							+ ToString(numbers_for_authors[k]) + "_" + Global::NAMEOFDATASET + ".txt" << endl;
					int num_of_innovators = innovators.size();
					for ( Innovator innovator : innovators) {
						distribution_for_num_of_reviews[innovator.num_of_reviews]++;
						distribution_for_experience_level[innovator.experience_level]++;
					}
					fout << "#reviewers \t innovators #reviews \t innovators' xp level \t entire data" << endl;
					for (pair<int, int> num_of_review_to_num_of_people : distribution_for_num_of_reviews ){
						int number_of_reviews = num_of_review_to_num_of_people.first;
						fout << number_of_reviews << "\t " << distribution_for_num_of_reviews[number_of_reviews]/(double)num_of_innovators << "\t" <<
								distribution_for_experience_level[number_of_reviews] / (double)num_of_innovators << "\t" <<distribution_for_entire_data_set[number_of_reviews] << endl;
					}
					fout.close();
					ofstream innovators_out("../Output_All/innovations_" + ToString(numbers_for_appearances[i]) + "_"
							+ ToString(numbers_for_products[j]) + "_"
							+ ToString(numbers_for_authors[k]) + "_" + Global::NAMEOFDATASET + ".txt");
					for (pair<string, vector<Review> > innovation :
							innovations[(numbers_for_authors.size()*numbers_for_products.size())*i + numbers_for_authors.size()*j + k] ) {
						innovators_out << innovation.first << ":" <<endl;
						int bound = 0;
						for(Review review : innovation.second) {
							innovators_out << review.score << endl;
							innovators_out << review.time << endl;
							if(bound < 1) {
								innovators_out << review.text << endl;
							}
							bound++;
							if(bound == 5){
								break;
							}
						}
						innovators_out << " __________________________ " <<endl;
					}
				}
			}
		}
	}

	static void FindBursts(map<string, vector<int>* > *word_states, vector<Review> *reviews) {
		map<string, vector<int>* > occurances;
		string text, word;
		for(Review review : *reviews) {
			text = review.text;
			stringstream ss(text);
			while(!ss.eof()){
				ss >> word;
				if(occurances.find(word) == occurances.end()){
					vector<int> *temp = new vector<int>();
					temp->push_back(review.time.Day(Amazon::Global::earliest));
					occurances[word] = temp;
				} else {
					vector<int> *temp = occurances[word];
					temp->push_back(review.time.Day(Amazon::Global::earliest));
					occurances[word] = temp;
				}
			}
		}
		// int max_size = 0;
		// int sum = 0;
		vector<int> *states;
		for(auto word_occurance : occurances) {
			string word = word_occurance.first;
			if(word != "malware"){
				continue;
			}
			vector<int> word_time_of_occurance = *(word_occurance.second);
			sort(word_time_of_occurance.begin(), word_time_of_occurance.end());
			for(int i = 0; i + 1 < word_time_of_occurance.size(); i++) {
				cerr << word_time_of_occurance[i] << " ";
				word_time_of_occurance[i] = word_time_of_occurance[i+1] - word_time_of_occurance[i];
			}
			cerr << endl;
			word_time_of_occurance.pop_back();
			states = new vector<int>();
		//	max_size = max(max_size, (int)word_time_of_occurance.size());
		//	sum += word_time_of_occurance.size();
			FindBurstsForWords(word_time_of_occurance, word, states);
			(*word_states)[word] = states;
		}
		// cerr << "--------->" << max_size << " " << sum << endl;

	}


	static double ProbabilityFinder(double alpha, int gap){
		return (-1) * (log(alpha) - 1 * alpha * gap);
	}

	static void FindPar(vector<int> *par , int x, int y, vector<int> *states) {
		while(x >= 0) {
			states->push_back(par[y][x]);
			y = par[y][x];
			x--;
		}
		reverse(states->begin(),states->end());
	}

	static void FindBurstsForWords(const vector<int> &time_gaps, string word, vector<int> *states) {
		if(word!="malware"){
			return;
		}
		vector<int> par[2];
		double alpha[2];
		int T = 0;
		for (int x : time_gaps) {
			T += x;
		}
		cerr << T << endl;
		cerr << Amazon::Global::latest.Day(Amazon::Global::earliest) << endl;;
		alpha[0] = time_gaps.size() / (double)T;
		cerr << "------>" << alpha[0] << endl;
		alpha[1] = alpha[0] * Amazon::Global::state_coeffecient;
		double viterbi[2];
		viterbi[0] = 0;
		viterbi[1] = 2000 * 1000 * 1000;
		double v[2];
		double p;
		for(int i = 1; i < (int)time_gaps.size(); i++) {
			v[0] = viterbi[0];
			v[1] = viterbi[1];
			p = Amazon::Global::probability_of_state_change;
			for(int j = 0; j < 2; j++) {
			//	cerr << j << ":::::" << v[j] + ProbabilityFinder(alpha[j],time_gaps[i]) << " " << v[1-j] + log((1-p)/p) + ProbabilityFinder(alpha[j],time_gaps[i]) << endl;
				if(v[j] + ProbabilityFinder(alpha[j],time_gaps[i])  <
						v[1-j] + log((1-p)/p) + ProbabilityFinder(alpha[j],time_gaps[i])) {
					viterbi[j] = v[j] + ProbabilityFinder(alpha[j],time_gaps[i]);
					par[j].push_back(j);
				} else {
					viterbi[j] = v[1-j] + log((1-p)/p) + ProbabilityFinder(alpha[j],time_gaps[i]);
					par[j].push_back(1-j);
				}
			}
		}
		if(viterbi[0] < viterbi[1]){
			FindPar(par, par[0].size() - 1, 0, states);
			states->push_back(0);
		} else {
			FindPar(par, par[1].size() - 1, 1, states);
			states->push_back(1);
		}
	}
};

set<string> Innovations::dictionary;
map<string, int> Innovations::counter_for_reviewer;
map<int, int> Innovations::distribution_for_experience_level;
map<int, int> Innovations::distribution_for_num_of_reviews;
vector<int> Innovations::numbers_for_appearances;
vector<int> Innovations::numbers_for_products;
vector<int> Innovations::numbers_for_authors;
#endif /* INNOVATIONS_H_ */

