/*
 * Videos.h
 *
 *  Created on: Oct 2, 2014
 *      Author: rahmtin
 */

#ifndef VIDEOS_H_
#define VIDEOS_H_

#include<algorithm>
#include<iostream>
#include<fstream>
#include<map>
#include<sstream>
#include<string>
#include<vector>

#include "Reviews.h"
#include "MyUtility.h"


// All the data that is analyzed have at least one vote.
void ReviewsWithVideo(vector<Review> *reviews) {
	// Assuming 30 min is the longest video
	vector<double> video_votes[60*10+10];
	double video_ratio = 0, data_ratio = 0;
	double video_average, data_average;
	double video_error = 0, data_error = 0;
	int length_of_video, minute, second;
	char ch;
	const int time_bucket = 60;
	int number_of_videos = 0;

	for (int i = 0; i < (int)reviews->size(); i++) {
		double helpfulness = SimpleStringFractionToDouble((*reviews)[i].helpfulness);
		if (EqDouble(helpfulness,-1)){
			continue;
		}
		data_ratio += helpfulness;
		if ((*reviews)[i].text.find("Length::") != string::npos) {
			video_ratio += (double)helpfulness;
			number_of_videos++;
			int position = (*reviews)[i].text.find("Length::");
			string has_the_length = (*reviews)[i].text.substr(position+8);
			stringstream get_length(has_the_length);
			get_length >> minute >> ch >> second;
			length_of_video = minute * 60 + second;
			video_votes[length_of_video / time_bucket].push_back(helpfulness);
		}
	}
	data_average = data_ratio / reviews->size();
	video_average = video_ratio / number_of_videos;
	map<string, vector<Review> > user_to_reviews;
	user_to_reviews.clear();
	for (int i = 0; i < (int)reviews->size(); i++) {
		double helpfulness = SimpleStringFractionToDouble((*reviews)[i].helpfulness);
		if (EqDouble(helpfulness,-1)){
			continue;
		}
		data_error += (helpfulness - data_average) * (helpfulness - data_average);
		if ((*reviews)[i].text.find("Length::") != string::npos) {
			video_error += (helpfulness - video_average) * (helpfulness - video_average);
			if(user_to_reviews.find((*reviews)[i].user_id) == user_to_reviews.end()) {
				user_to_reviews[(*reviews)[i].user_id] = vector<Review>();
			}
			vector<Review> current = user_to_reviews[(*reviews)[i].user_id];
			current.push_back((*reviews)[i]);
			user_to_reviews[(*reviews)[i].user_id] = current;
		}
	}

	data_error /= reviews->size();
	video_error /= number_of_videos;

	ofstream video_correlation_to_helpfulness("../Output_All/video_correlation_to_helpfulness_" + Global::NAMEOFDATASET +".out");
	video_correlation_to_helpfulness << "Number of videos " << number_of_videos << endl;
	video_correlation_to_helpfulness << "Average of video and data: " << video_average << " " << data_average <<endl;
	video_correlation_to_helpfulness << "Error of video and data: " << video_error << " " << data_error <<endl;

	ofstream video_length_correlation_to_helpfulness(
			"../Output_All/video_correlation_to_helpfulness_" + Global::NAMEOFDATASET +"_has_error.txt");
	for (int i = 0; i < 600/time_bucket ; i++) {
		double average, error;
		average = FindAverage(&video_votes[i]);
		error = FindConfidenceInterval95(&video_votes[i], average);
		video_length_correlation_to_helpfulness << "[" << i*60 << "," << i*60+60 << "] " << average << " " << error <<endl;
	}

	vector<double> people_who_ended[30];
	vector<double> people_who_continued[30];
	for (map<string, vector<Review> > :: iterator it = user_to_reviews.begin(); it != user_to_reviews.end(); it++) {
		vector<Review> reviews_of_one_user = it->second;
		sort(reviews_of_one_user.begin(), reviews_of_one_user.end());
		for (int i = 0; i < (int)reviews_of_one_user.size(); i++) {
			if (i != (int)reviews_of_one_user.size() - 1) {
				people_who_continued[i+1].push_back(SimpleStringFractionToDouble(reviews_of_one_user[i].helpfulness));
			} else {
				people_who_ended[i+1].push_back(SimpleStringFractionToDouble(reviews_of_one_user[i].helpfulness));
			}
		}
	}
	for (int i = 0; i < 20; i++){
		double average_continued = 0;
		double average_gave_up = 0;
		for (int j = 0; j < (int)people_who_continued[i].size(); j++) {
			average_continued += people_who_continued[i][j];
		}
		for (int j = 0; j < (int)people_who_ended[i].size(); j++) {
			average_gave_up += people_who_ended[i][j];
		}
		cout << average_continued / people_who_continued[i].size() << " " << average_gave_up / people_who_ended[i].size() << endl;
	}
}


#endif /* VIDEOS_H_ */
