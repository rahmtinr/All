/*
 * Aggregation.h
 *
 *  Created on: Oct 3, 2014
 *      Author: rahmtin
 */

#ifndef AGGREGATION_H_
#define AGGREGATION_H_

#include<fstream>
#include<iostream>
#include<map>
#include<set>
#include<sstream>
#include<vector>

#include "AmazonUtility.h"
#include "MyUtility.h"
#include "Reviews.h"

using namespace std;


// Number of reviews written in different months.
// The number for a month is accumulated over all the years.
void CountMonthlyAccumulatedReviews(vector<Review> *reviews) {
	// Number of items reviewed from Amazon with different months of years
	// Same months over different years are accumulated.
	int overall_count_month[12];
	for (int i = 0; i < (int)reviews->size(); i++) {
		overall_count_month[(*reviews)[i].time.month]++;
	}
	ofstream overall_outputs_monthly_accumulated_out("../Output_All/overall_monthly_accumulated_" + Global::NAMEOFDATASET +".txt");
	for (int i = 0; i < 12; i++) {
		overall_outputs_monthly_accumulated_out << month[i] << " " << overall_count_month[i] << endl;
	}
}

// Number of reviews written in different years.
void CountYearlyReviews(vector<Review> *reviews) {
	// Number of items reviewed from Amazon with different years
	int overall_count_year[2020];
	for (int i = 0; i < (int)reviews->size(); i++) {
		overall_count_year[(*reviews)[i].time.year]++;
	}
	ofstream overall_outputs_yearly_out("../Output_All/overall_yearly_" + Global::NAMEOFDATASET +".txt");
	for (int i = 1998; i < 2015; i++) {
		overall_outputs_yearly_out << i << " " << overall_count_year[i] << endl;
	}
}




// Number of reviews written in different months.
// The number for a month is accumulated over all the years.
void StarAveragePerMonthAccumulatedOverYears(vector<Review> *reviews) {
	vector<double> star_rating[12];
	double average;
	double error;
	for (int i = 0; i < (int)reviews->size(); i++) {
		stringstream ss((*reviews)[i].score);
		if((*reviews)[i].time.year < 2006 && (*reviews)[i].time.year > 2002) {
			continue;
		}
		double score;
		ss >> score;
		star_rating[(*reviews)[i].time.month].push_back(score);
	}
	ofstream overall_outputs_monthly_accumulated_out_star_rating("../Output_All/overall_monthly_accumulated_star_rating_" + Global::NAMEOFDATASET +"_removing_2003_to_2006_has_error.txt");
	for (int i = 0; i < 12; i++) {
		average = FindAverage(&star_rating[i]);
		error = FindConfidenceInterval95(&star_rating[i], average);
		overall_outputs_monthly_accumulated_out_star_rating << month[i] << " " << average << " " << error << endl;
	}
}

// Number of reviews written in different months.
void StarAveragePerMonth(vector<Review> *reviews) {
	vector<double> star_rating[12*30];
	double average;
	double error;
	for (int i = 0; i < (int)reviews->size(); i++) {
		stringstream ss((*reviews)[i].score);
		double score;
		ss >> score;
		star_rating[(*reviews)[i].time.month+12*((*reviews)[i].time.year-Amazon::Global::min_year)].push_back(score);
	}
	for(int j = 0; j < 12; j++) {
		ofstream overall_outputs_monthly_accumulated_out_star_rating("../Output_All/" + month[j] + "_star_rating_" + Global::NAMEOFDATASET +"_has_error.txt");
		for (int i = j; i < 12*30; i += 12) {
			if((int)star_rating[i].size() == 0){
				continue;
			}
			average = FindAverage(&star_rating[i]);
			error = FindConfidenceInterval95(&star_rating[i], average);
			overall_outputs_monthly_accumulated_out_star_rating << month[i%12] << "/" << i/12+Amazon::Global::min_year << " " << average << " " << error << endl;
		}
	}
}


// Number of reviews written in different years.
void StarAveragePerYear(vector<Review> *reviews) {
	vector<double> star_rating[30];
	double average;
	double error;
	int min_year = 3000;
	for (int i = 0; i < (int)reviews->size(); i++) {
		stringstream ss((*reviews)[i].score);
		min_year = min(min_year, (*reviews)[i].time.year);
		double score;
		ss >> score;
		star_rating[(*reviews)[i].time.year-min_year].push_back(score);
	}
	ofstream overall_outputs_yearly_out_star_rating("../Output_All/overall_yearly_star_rating_" + Global::NAMEOFDATASET +"_has_error.txt");
	for (int i = min_year; i < 2015; i++) {
		average = FindAverage(&star_rating[i - min_year]);
		error = FindConfidenceInterval95(&star_rating[i - min_year], average);
		overall_outputs_yearly_out_star_rating << i << " " << average  << " " << error << endl;
	}
}

void StarAveragePerTimeInTheDay(vector<Review> *reviews) {
	vector<double> star_rating[30];
	double average;
	double error;
	for (int i = 0; i < (int)reviews->size(); i++) {
		stringstream ss((*reviews)[i].score);
		double score;
		ss >> score;
		star_rating[(*reviews)[i].time.hour].push_back(score);
	}
	ofstream overall_outputs_hourly_accumulated_out_star_rating("../Output_All/overall_hourly_accumulated_star_rating_" + Global::NAMEOFDATASET +"has_error_.txt");
	for (int i = 0; i < 24; i++) {
		average = FindAverage(&star_rating[i]);
		error = FindConfidenceInterval95(&star_rating[i], average);
		overall_outputs_hourly_accumulated_out_star_rating << i << " " << average  << " " << error << endl;
	}
}



#endif /* AGGREGATION_H_ */
