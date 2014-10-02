#include<algorithm>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cmath>
#include<ctime>
#include<fstream>
#include<iostream>
#include<map>
#include<set>
#include<sstream>
#include<string>
#include<vector>

#include "AmazonUtility.h"
#include "Innovations.h"
#include "MyUtility.h"
#include "ReviewReader.h"
#include "Reviews.h"

using namespace std;

typedef map<pair<string, int>, int> ProductTimeCount;

string NAMEOFTHEDATASET;
map<string, int> product_count;

map<string, int> counter_for_reviewer;
map<int, int> distribution_for_count_of_reviewer;

vector<Review> reviews;
vector<Product> products;
// Number of fine food items purchased from Amazon is different years and months of years
// Same months over different years are accumulated.
int overall_count_month[12];
int overall_count_year[2020];

// item_count_per_month has year*100+month as the time window.
// map from (product_id, time range) -> count
map<pair<string, int>, int> item_count_per_year, item_count_per_month;

// Map from user_id to the number of reviews they have
map<string, int> count_review_per_user;

// Map from the length of video to the average of helpfulness for reviews which have video
map<int, double> length_to_rating;


// Seasonal variations, string to count of the word in that season
map<string, int> words_in_winter, words_in_summer;
// Count to the word so we can sort them
vector<pair<int, string> > words_in_winter_vec, words_in_summer_vec;
// Number of words in different seasons
int summer_count, winter_count;
// Langauge innovations
vector<pair<string, vector<Review>> > innovations;


//////////////////////////////// End of Utility functions

// Number of reviews written in different months.
// The number for a month is accumulated over all the years.
void CountMonthlyAccumulatedReviews() {
	for (int i = 0; i < (int)reviews.size(); i++) {
		overall_count_month[reviews[i].time.month]++;
	}
	ofstream overall_outputs_monthly_accumulated_out("../Output_All/overall_monthly_accumulated_" + NAMEOFTHEDATASET +".txt");
	for (int i = 0; i < 12; i++) {
		overall_outputs_monthly_accumulated_out << month[i] << " " << overall_count_month[i] << endl;
	}
}

// Number of reviews written in different years.
void CountYearlyReviews() {
	for (int i = 0; i < (int)reviews.size(); i++) {
		overall_count_year[reviews[i].time.year]++;
	}
	ofstream overall_outputs_yearly_out("../Output_All/overall_yearly_" + NAMEOFTHEDATASET +".txt");
	for (int i = 1998; i < 2015; i++) {
		overall_outputs_yearly_out << i << " " << overall_count_year[i] << endl;
	}
}

// Number of reviews in different months for a particular item.
// Separate years are not accumulated.
void PerItemPerMonth() {
	for (int i = 0; i < (int)reviews.size(); i++) {
		pair<string, int> product_id_time_range;
		product_id_time_range = make_pair(reviews[i].product_id, reviews[i].time.year);
		item_count_per_year[product_id_time_range]++;
	}
	ProductTimeCount::iterator before;
	ofstream fout;
	for ( ProductTimeCount :: iterator current = item_count_per_month.begin();
			current != item_count_per_month.end(); current++){
		if (current == item_count_per_year.begin() ||
				before->first.first != current->first.first) {
			fout.close();
			fout.open(("../Output_All/PerItem/" + current->first.first + "_monthly_" + NAMEOFTHEDATASET +".txt").c_str(),std::ofstream::out);
			//	fout << current ->first.first << endl;
		}
		fout << month[current->first.second%100] << "/" <<
				current->first.second/100 << " " << current->second <<endl;
		before = current;
	}
}

// Number of reviews in different years for a particular item.
void PerItemPerYear() {
	for (int i = 0; i < (int)reviews.size(); i++) {
		pair<string, int> product_id_time_range;
		product_id_time_range = make_pair(reviews[i].product_id,
				reviews[i].time.year*100 + reviews[i].time.month);
		item_count_per_month[product_id_time_range]++;
	}
	ProductTimeCount::iterator before;
	ofstream fout;
	for ( ProductTimeCount :: iterator current = item_count_per_year.begin();
			current != item_count_per_year.end(); current++){
		if (current == item_count_per_year.begin() ||
				before->first.first != current->first.first) {
			fout.close();
			fout.open(("../Output_All/PerItem/" + current->first.first + "_yearly.txt").c_str(),std::ofstream::out);
			//	fout << current ->first.first << endl;
		}
		fout << current->first.second << " " << current->second <<endl;
		before = current;
	}

}

// Outputs the top size_of_list products that have been reviewed more than others.
void TopProducts(int size_of_list) {
	for (int i = 0; i <(int) reviews.size(); i++) {
		product_count[reviews[i].product_id]++;
	}
	Product product;
	ofstream top_products_out("../Output_All/top_products_" + NAMEOFTHEDATASET +".txt");
	for (map<string, int>::iterator it = product_count.begin(); it!=product_count.end(); it++) {
		product.product_id = it->first;
		product.count = it->second;
		products.push_back(product);
	}
	sort (products.begin(), products.end());
	for (int i = 0 ; i < size_of_list; i++) {
		top_products_out << products[i].product_id << " " << products[i].count << endl;
	}

	// Number of reviewers with certain number of reviews
	for (int i = 0; i < (int)reviews.size(); i++) {
		count_review_per_user[reviews[i].user_id]++;
	}
}



// Number of reviews written in different months.
// The number for a month is accumulated over all the years.
void StarAveragePerMonth() {
	vector<double> star_rating[12];
	double average;
	double error;
	for (int i = 0; i < (int)reviews.size(); i++) {
		stringstream ss(reviews[i].score);
		double score;
		ss >> score;
		star_rating[reviews[i].time.month].push_back(score);
	}
	ofstream overall_outputs_monthly_accumulated_out_star_rating("../Output_All/overall_monthly_accumulated_star_rating_" + NAMEOFTHEDATASET +"_has_error.txt");
	for (int i = 0; i < 12; i++) {
		average = FindAverage(&star_rating[i]);
		error = FindConfidenceInterval95(&star_rating[i], average);
		overall_outputs_monthly_accumulated_out_star_rating << month[i] << " " << average << " " << error << endl;
	}
}

// Number of reviews written in different years.
void StarAveragePerYear() {
	vector<double> star_rating[30];
	double average;
	double error;
	int min_year = 3000;
	for (int i = 0; i < (int)reviews.size(); i++) {
		stringstream ss(reviews[i].score);
		min_year = min(min_year, reviews[i].time.year);
		double score;
		ss >> score;
		star_rating[reviews[i].time.year-min_year].push_back(score);
	}
	ofstream overall_outputs_yearly_out_star_rating("../Output_All/overall_yearly_star_rating_" + NAMEOFTHEDATASET +"_has_error.txt");
	for (int i = min_year; i < 2015; i++) {
		average = FindAverage(&star_rating[i - min_year]);
		error = FindConfidenceInterval95(&star_rating[i - min_year], average);
		overall_outputs_yearly_out_star_rating << i << " " << average  << " " << error << endl;
	}
}

void StarAveragePerTimeInTheDay() {
	vector<double> star_rating[30];
	double average;
	double error;
	for (int i = 0; i < (int)reviews.size(); i++) {
		stringstream ss(reviews[i].score);
		double score;
		ss >> score;
		star_rating[reviews[i].time.hour].push_back(score);
	}
	ofstream overall_outputs_hourly_accumulated_out_star_rating("../Output_All/overall_hourly_accumulated_star_rating_" + NAMEOFTHEDATASET +"has_error_.txt");
	for (int i = 0; i < 24; i++) {
		average = FindAverage(&star_rating[i]);
		error = FindConfidenceInterval95(&star_rating[i], average);
		overall_outputs_hourly_accumulated_out_star_rating << i << " " << average  << " " << error << endl;
	}
}

void SeasonalTopWordsTitle() {
	for (Review review : reviews) {
		string word;
		if (review.time.month == 0 || review.time.month == 1 || review.time.month == 11) { // winter
			winter_count++;
			stringstream ss(review.product_title);
			while (ss.good()) {
				ss >> word;
				words_in_winter[SimpleRemoveAnySymbol(word)]++;
			}
		}
		if (review.time.month == 5 || review.time.month == 6 || review.time.month == 7) { // summer
			summer_count++;
			stringstream ss(review.product_title);
			while (ss.good()) {
				ss >> word;
				words_in_summer[SimpleRemoveAnySymbol(word)]++;
			}
		}
	}
	cout << winter_count << "  " << summer_count << endl;
	for ( auto x : words_in_summer) {
		words_in_summer_vec.push_back(make_pair(x.second, x.first));
	}
	for ( auto x : words_in_winter) {
		words_in_winter_vec.push_back(make_pair(x.second, x.first));
	}
	sort(words_in_summer_vec.begin(), words_in_summer_vec.end());
	sort(words_in_winter_vec.begin(), words_in_winter_vec.end());
	reverse(words_in_summer_vec.begin(), words_in_summer_vec.end());
	reverse(words_in_winter_vec.begin(), words_in_winter_vec.end());
	ofstream summer_top_words("../Output_All/summer_top_words_" + NAMEOFTHEDATASET +".txt");
	ofstream winter_top_words("../Output_All/winter_top_words_" + NAMEOFTHEDATASET +".txt");
	for (auto x : words_in_summer_vec ) {
		summer_top_words << x.first << " " << x.second <<endl;
	}

	for (auto x : words_in_winter_vec ) {
		winter_top_words << x.first << " " << x.second <<endl;
	}
}

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
void FindUserInfo() {
	for (int i = 0; i < (int)reviews.size(); i++) {
		UserInfo temp;
		temp.user_id = reviews[i].user_id;
		if (users_info.find(temp) != users_info.end()) {
			temp = *(users_info.find(temp));
			users_info.erase(temp);
		}
		temp.num_of_reviews++;
		temp.sum_of_score += SimpleStringToDouble(reviews[i].score);
		users_info.insert(temp);
	}
}


bool CompareUsersOnNumberOfReviews(const UserInfo &x,const UserInfo &y) {
	if(x.num_of_reviews == y.num_of_reviews) {
		return x.user_id < y.user_id;
	}
	return x.num_of_reviews < y.num_of_reviews;
}

void UserAngrinessBasedOnNumberOfReviews() {
	FindUserInfo();
	users_info_vec = vector<UserInfo>(users_info.begin(), users_info.end());
	sort(users_info_vec.begin(), users_info_vec.end(), CompareUsersOnNumberOfReviews);
	double current_sum = 0;
	int current_num = 0;
	ofstream fout("../Output_All/user_angriness_based_on_number_of_reviews_" + NAMEOFTHEDATASET +".txt");
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

void UserDistributionBasedOnNumberOfReviews() {
	counter_for_reviewer.clear();
	distribution_for_count_of_reviewer.clear();
	for (Review review : reviews) {
		counter_for_reviewer[review.user_id]++;
	}
	int num_of_users = counter_for_reviewer.size();
	for (pair<string, int> user : counter_for_reviewer) {
		distribution_for_count_of_reviewer[user.second]++;
	}
	ofstream fout("../Output_All/all_reviewers_distribution_" + NAMEOFTHEDATASET +".txt");
	for (pair<int, int> num_of_review_to_num_of_people : distribution_for_count_of_reviewer){
		fout << num_of_review_to_num_of_people.first << " " << num_of_review_to_num_of_people.second/(double)num_of_users << endl;
	}
}

int main(int argc, char *argv[]) {
	string MODE = "_Top3TaxRemoved";
	NAMEOFTHEDATASET = (string(argv[1])).substr(0,string(argv[1]).find(".")) + MODE;
	ifstream fin(argv[1]);
	// Read input.
	while (true) {
		if (!ReadOneReview(fin, &reviews)) {
				break;
		}
		//		reviews[reviews.size() - 1].print();
	}
	cout << reviews[0] <<endl;
//	cerr << reviews.size() <<endl;
//	MyFilter("text", "turbotax");
	MyFilter("text", "hr block", &reviews);
	MyFilter("text", "taxact", &reviews);
	cerr << reviews.size() << endl;
	sort(reviews.begin(), reviews.end(), CompareReviewOnUserId);
	reviews.erase(unique( reviews.begin(), reviews.end(), ReviewEquality), reviews.end());
	cerr << reviews.size() << endl;
	/**/
	CountMonthlyAccumulatedReviews();
	CountYearlyReviews();

//	PerItemPerMonth();
//	PerItemPerYear();
	// Top products.
//	int size_of_list = 40;
//	TopProducts(size_of_list);
	// Video Average vs All average.
	ReviewsWithVideo(&reviews);
//	StarAveragePerMonth();
//	StarAveragePerYear();
	// Time in Day is useless! The timestamp is on a daily basis
//	StarAveragePerTimeInTheDay();
	 /**/

	/**/
	sort(reviews.begin(), reviews.end());
	Innovations::LearnDictionary(0, reviews.size() / 2, &reviews);
	Innovations::FindInnovations(reviews.size() / 2, &reviews, &innovations); // returns pair of word and review it was started.
	Innovations::AnalyseInnovation(&innovations, &reviews);
	UserDistributionBasedOnNumberOfReviews();
	/**/
//	UserAngrinessBasedOnNumberOfReviews();
	return 0;
}
