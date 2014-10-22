/*
 * UserRelated.h
 *
 *  Created on: Oct 3, 2014
 *      Author: rahmtin
 */

#ifndef USERRELATED_H_
#define USERRELATED_H_

#include<algorithm>
#include<fstream>
#include<iostream>
#include<map>
#include<set>
#include<string>
#include<vector>

#include "MyUtility.h"
#include "Reviews.h"
using namespace std;

class UserInfo {
public:
	string user_id;
	int num_of_reviews;
	double sum_of_score;

	bool operator < (const UserInfo &other) const {
		return user_id < other.user_id;
	}

	UserInfo() {
		num_of_reviews = 0;
		sum_of_score = 0;
	}
};

set<UserInfo> users_info;
vector<UserInfo> users_info_vec;
void FindUserInfo(vector<Review> *reviews) {
	for (int i = 0; i < (int)reviews->size(); i++) {
		UserInfo temp;
		temp.user_id = (*reviews)[i].user_id;
		if (users_info.find(temp) != users_info.end()) {
			temp = *(users_info.find(temp));
			users_info.erase(temp);
		}
		temp.num_of_reviews++;
		temp.sum_of_score += SimpleStringToDouble((*reviews)[i].score);
		users_info.insert(temp);
	}
}


bool CompareUsersOnNumberOfReviews(const UserInfo &x,const UserInfo &y) {
	if(x.num_of_reviews == y.num_of_reviews) {
		return x.user_id < y.user_id;
	}
	return x.num_of_reviews < y.num_of_reviews;
}

void UserAngrinessBasedOnNumberOfReviews(vector<Review> *reviews) {
	FindUserInfo(reviews);
	users_info_vec = vector<UserInfo>(users_info.begin(), users_info.end());
	sort(users_info_vec.begin(), users_info_vec.end(), CompareUsersOnNumberOfReviews);
	double current_sum = 0;
	int current_num = 0;
	ofstream fout("../Output_All/user_angriness_based_on_number_of_reviews_" + Global::NAMEOFDATASET +".txt");
	for (int i = 0; i < (int)users_info_vec.size(); i++) {
		if ( i == 0 || users_info_vec[i].num_of_reviews == users_info_vec[i-1].num_of_reviews) {
			current_num++;
			current_sum += users_info_vec[i].sum_of_score / users_info_vec[i].num_of_reviews;
		} else {
			fout << users_info_vec[i-1].num_of_reviews << " " <<  current_sum / current_num << endl;
			current_num = 1;
			current_sum = users_info_vec[i].sum_of_score / users_info_vec[i].num_of_reviews;
		}
	}
	fout << users_info_vec[users_info_vec.size() - 1].num_of_reviews << " " <<  current_sum / current_num << endl;
}

void UserDistributionBasedOnNumberOfReviews(vector<Review> *reviews, map<int, double> *distribution_for_entire_data_set) {
	map<string, int> counter_for_reviewer;
	map<int, int> distribution_for_count_of_reviewer;
	counter_for_reviewer.clear();
	distribution_for_count_of_reviewer.clear();
	for (Review review : *reviews) {
		counter_for_reviewer[review.user_id]++;
	}
	int num_of_users = counter_for_reviewer.size();
	for (pair<string, int> user : counter_for_reviewer) {
		(*distribution_for_entire_data_set)[user.second]++;
	}
	ofstream fout("../Output_All/all_reviewers_distribution_" + Global::NAMEOFDATASET +".txt");
	for (pair<int, int> num_of_review_to_num_of_people : *distribution_for_entire_data_set){
		(*distribution_for_entire_data_set)[num_of_review_to_num_of_people.first] = num_of_review_to_num_of_people.second/(double)num_of_users;
		fout << num_of_review_to_num_of_people.first << " " << num_of_review_to_num_of_people.second << endl;
	}
}


#endif /* USERRELATED_H_ */
