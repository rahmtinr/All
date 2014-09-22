#include<iostream>
#include<ctime>
#include<vector>
#include<string>
#include<algorithm>
#include<cstdio>
#include<cstdlib>
#include<fstream>
#include<map>
#include<cstring>
#include<cmath>
#include<sstream>
#include<set>

#define FAIL false
#define SUCCESS true

using namespace std;

typedef map<pair<string, int>, int> ProductTimeCount;

const string month[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};

map<string, int> product_count;

class MyTime { 
public:
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int weekday;

	MyTime(){}
	void print() {
		cout<< "Time: " << month << "/" << day << "/" << year<< "   ";
		cout<< hour << ":" << minute << ":" << second <<endl;
	}
	MyTime(struct tm* time){
		year = time->tm_year+1900;
		month = time->tm_mon;
		day = time->tm_mday;
		hour = time->tm_hour;
		minute = time->tm_min;
		second = time->tm_sec;
		weekday = time->tm_wday;
	}
};

class Review {
public:
	string product_id;
	string product_title;
	string price;
	string user_id;
	string profile_name;
	string helpfulness;
	string score;
	MyTime time;
	string summary;
	string text;

	void print() {
		cout<< "ProductId: " << product_id << endl;
		cout<< "UserId: " << user_id << endl;
		cout<< "ProfileName: " << profile_name << endl;
		cout<< "Helpfulness: " << helpfulness << endl;
		cout<< "Score: " << score << endl;
		time.print();
		cout<< "Summary: " << summary << endl;
		cout<< "Text: " << text << endl;
	}
	bool operator < (const Review &other) const {
		if (time.year != other.time.year) {
			return time.year < other.time.year;
		}
		if (time.month != other.time.month) {
			return time.month < other.time.month;
		}
		if (time.year != other.time.year) {
			return time.day < other.time.day;
		}
		return product_id < other.product_id;
	}
	bool operator == (const Review &other) const {
		return (product_id == other.product_id && user_id == other.user_id && text == other.text);
	}
};

bool cmp(Review x, Review y) {
	if(x.user_id == y.user_id) {
		return x.text < y.text;
	}
	return x.user_id < y.user_id;
}

bool cmp2(Review x, Review y) {
	return (x.user_id == y.user_id && x.text == y.text);
}

class Product { 
public:
	string product_id;
	int count;
	bool operator < (const Product &other) const { 
		return count > other.count;
	}
};

class VoteRatio {
public:
	int helpful;
	int all;
	VoteRatio() {
		helpful =0;
		all = 0;
	}
};

vector<Review> reviews, filter_reviews;
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
// Assuming 30 min is the longest video
vector<double> video_votes[60*10+10], helpful_video_votes[60*10+10];

// Seasonal variations, string to count of the word in that season
map<string, int> words_in_winter, words_in_summer;
// Count to the word so we can sort them
vector<pair<int, string> > words_in_winter_vec, words_in_summer_vec;
// Number of words in different seasons
int summer_count, winter_count;

// Langauge innovations
vector<pair<string, Review> > innovations;
set<string> dictionary;
int TimeDifferenceInMonth(MyTime first, MyTime second) {
	int first_month = first.year * 12 + first.month;
	int second_month = second.year * 12 + second.month;
	return abs(first_month - second_month);
}

string SimpleRemoveAnySymbol(string input) {
	string result = "";
	for (int i = 0; i <(int) input.size(); i++) {
		if(isalpha(input[i]) || input[i] =='\''){
			result += input[i];
		}
	}
	return result;
}

//returns -1 if denominator is equal to 0.
double SimpleStringFractionToDouble(string input) {
	stringstream ss(input);
	double numerator, denominator;
	char ch;
	ss >> numerator >> ch >> denominator;
	if (denominator == 0){
		return -1;
	}
	return numerator / denominator;
}

double SimpleStringToDouble(string input) {
	stringstream ss(input);
	double ret;
	ss >> ret;
	return ret;
}


bool EqDouble(double x, double y) {
	if(abs(x-y) < 1e-6) {
		return true;
	}
	return false;
}

string SimpleToLower(string s) {
	for (int i = 0; i < (int)s.size(); i++) {
		s[i] = tolower(s[i]);
	}
	return s;
}
string GetField(string raw_input) {
	int delimeter = raw_input.find(":");
	if (delimeter == std::string::npos) {
		return "THIS INPUT IS TRASH";
	}
	return raw_input.substr(delimeter+2);
}

string RemoveAllSymbols(string raw_input) {
	string ret = "";
	for (int i = 0; i < (int)raw_input.size(); i++) {
		if(isalpha(raw_input[i]) || raw_input[i]=='\'' || isspace(raw_input[i]) || isdigit(raw_input[i])) {
			ret += raw_input[i];
		}
		if(raw_input[i] == ',' || raw_input[i]=='.') {
			ret += ' ';
		}
	}
	return ret;
}

bool ReadOneReview() {
	string raw_input;
	Review review;
	if (getline(cin, raw_input)) {
		review.product_id = GetField(raw_input);
		getline(cin, raw_input);
		review.product_title = RemoveAllSymbols(SimpleToLower(GetField(raw_input)));
		getline(cin, raw_input);
		review.price = GetField(raw_input);
		getline(cin, raw_input);
		review.user_id = GetField(raw_input);
		getline(cin, raw_input);
		review.profile_name = GetField(raw_input);
		do {
			getline(cin, raw_input);
			review.helpfulness = GetField(raw_input);
		} while (review.helpfulness == "THIS INPUT IS TRASH");
		getline(cin, raw_input);
		review.score = GetField(raw_input);
		getline(cin, raw_input);
		int time_int = atoi((GetField(raw_input)).c_str());
		time_t review_time(time_int);
		review.time = MyTime(localtime(&review_time));
		getline(cin, raw_input);
		review.summary = RemoveAllSymbols(SimpleToLower(GetField(raw_input)));
		getline(cin, raw_input);

		review.text = RemoveAllSymbols(SimpleToLower(GetField(raw_input)));
		getline(cin, raw_input);
		reviews.push_back(review);
		return SUCCESS;
	}
	return FAIL;
}

void MyFilter(string key, string value) {
	filter_reviews.clear();
	for ( int i = 0; i < (int)reviews.size(); i++){
		if (key == "productId") {
			if (reviews[i].product_id == value) {
				filter_reviews.push_back(reviews[i]);
			}
		}
		if (key == "product_title") {
			if (reviews[i].product_title.find(value) != string::npos) {
				filter_reviews.push_back(reviews[i]);
			}
		}
	}
	reviews = filter_reviews;
}




//////////////////////////////// End of Utility functions

// Number of reviews written in different months.
// The number for a month is accumulated over all the years.
void CountMonthlyAccumulatedReviews() {
	for (int i = 0; i < (int)reviews.size(); i++) {
		overall_count_month[reviews[i].time.month]++;
	}
	ofstream overall_outputs_monthly_accumulated_out("../Output_All/overall_monthly_accumulated.txt");
	for (int i = 0; i < 12; i++) {
		overall_outputs_monthly_accumulated_out << month[i] << " " << overall_count_month[i] << endl;
	}
}

// Number of reviews written in different years.
void CountYearlyReviews() {
	for (int i = 0; i < (int)reviews.size(); i++) {
		overall_count_year[reviews[i].time.year]++;
	}
	ofstream overall_outputs_yearly_out("../Output_All/overall_yearly.txt");
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
			fout.open(("../Output_All/PerItem/" + current->first.first + "_monthly.txt").c_str(),std::ofstream::out);
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
	ofstream top_products_out("../Output_All/top_products.txt");
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

double FindAverage(vector<double> *data) {
	if (data->size() == 0) {
		return 0;
	}
	double average = 0;
	for (int i = 0; i < (int)data->size(); i++) {
		average += (*data)[i];
	}
	return average / data->size();
}

double FindError(vector<double> *data, double average) {
	if (data->size() == 0) {
		return 0;
	}
	double error = 0;
	for (int i = 0; i < (int)data->size(); i++) {
		error += ((*data)[i]-average) * ((*data)[i]-average);
	}

	error /= data->size();
	return sqrt(error);
}

// All the data that is analyzed have at least one vote.
void ReviewsWithVideo() {
	double video_ratio = 0, data_ratio = 0;
	double video_average, data_average;
	double video_error = 0, data_error = 0;
	int length_of_video, minute, second;
	char ch;
	const int time_bucket = 60;
	int number_of_videos = 0;

	for (int i = 0; i < (int)reviews.size(); i++) {
		double helpfulness = SimpleStringFractionToDouble(reviews[i].helpfulness);
		if (EqDouble(helpfulness,-1)){
			continue;
		}
		data_ratio += helpfulness;
		if (reviews[i].text.find("Length::") != string::npos) {
			video_ratio += (double)helpfulness;
			number_of_videos++;
			int position = reviews[i].text.find("Length::");
			string has_the_length = reviews[i].text.substr(position+8);
			stringstream get_length(has_the_length);
			get_length >> minute >> ch >> second;
			length_of_video = minute * 60 + second;
			video_votes[length_of_video / time_bucket].push_back(helpfulness);
		}
	}
	data_average = data_ratio / reviews.size();
	video_average = video_ratio / number_of_videos;
	map<string, vector<Review> > user_to_reviews;
	user_to_reviews.clear();
	for (int i = 0; i < (int)reviews.size(); i++) {
		double helpfulness = SimpleStringFractionToDouble(reviews[i].helpfulness);
		if (EqDouble(helpfulness,-1)){
			continue;
		}
		data_error += (helpfulness - data_average) * (helpfulness - data_average);
		if (reviews[i].text.find("Length::") != string::npos) {
			video_error += (helpfulness - video_average) * (helpfulness - video_average);
			if(user_to_reviews.find(reviews[i].user_id) == user_to_reviews.end()) {
				user_to_reviews[reviews[i].user_id] = vector<Review>();
			}
			vector<Review> current = user_to_reviews[reviews[i].user_id];
			current.push_back(reviews[i]);
			user_to_reviews[reviews[i].user_id] = current;
		}
	}

	data_error /= reviews.size();
	video_error /= number_of_videos;

	ofstream video_correlation_to_helpfulness("../Output_All/video_correlation_to_helpfulness.out");
	video_correlation_to_helpfulness << "Number of videos " << number_of_videos << endl;
	video_correlation_to_helpfulness << "Average of video and data: " << video_average << " " << data_average <<endl;
	video_correlation_to_helpfulness << "Error of video and data: " << video_error << " " << data_error <<endl;

	ofstream video_length_correlation_to_helpfulness(
			"../Output_All/video_correlation_to_helpfulness_has_error.txt");
	for (int i = 0; i < 600/time_bucket ; i++) {
		double average, error;
		average = FindAverage(&video_votes[i]);
		error = FindError(&video_votes[i], average);
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
	ofstream overall_outputs_monthly_accumulated_out_star_rating("../Output_All/overall_monthly_accumulated_star_rating_has_error.txt");
	for (int i = 0; i < 12; i++) {
		average = FindAverage(&star_rating[i]);
		error = FindError(&star_rating[i], average);
		overall_outputs_monthly_accumulated_out_star_rating << month[i] << " " << average << " " << error << endl;
	}
}

// Number of reviews written in different years.
void StarAveragePerYear() {
	vector<double> star_rating[30];
	double average;
	double error;
	for (int i = 0; i < (int)reviews.size(); i++) {
		stringstream ss(reviews[i].score);
		double score;
		ss >> score;
		star_rating[reviews[i].time.year-1998].push_back(score);
	}
	ofstream overall_outputs_yearly_out_star_rating("../Output_All/overall_yearly_star_rating_has_error.txt");
	for (int i = 1998; i < 2015; i++) {
		average = FindAverage(&star_rating[i - 1998]);
		error = FindError(&star_rating[i - 1998], average);
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
	ofstream overall_outputs_hourly_accumulated_out_star_rating("../Output_All/overall_hourly_accumulated_star_rating_has_error.txt");
	for (int i = 0; i < 24; i++) {
		average = FindAverage(&star_rating[i]);
		error = FindError(&star_rating[i], average);
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
	ofstream summer_top_words("../Output_All/summer_top_words.txt");
	ofstream winter_top_words("../Output_All/winter_top_words.txt");
	for (auto x : words_in_summer_vec ) {
		summer_top_words << x.first << " " << x.second <<endl;
	}

	for (auto x : words_in_winter_vec ) {
		winter_top_words << x.first << " " << x.second <<endl;
	}
}

//Learn Dictionary from the first len reviews
void LearnDictionary(int start, int end) {
	string word;
	for (int i = start; i < end; i++ ) {
		stringstream ss(reviews[i].text);
		while (ss.good()) {
			ss >> word;
			dictionary.insert(word);
		}
	}
}

bool PopWordAddToDictionary(string word, vector<Review> *review_history) {
	set<string> products_that_have_this_word;
	set<string> users_that_have_used_this_word;
	if (review_history->size() < 8) {
		return false;
	}
	for (int i = 0; i < (int)review_history->size(); i++) {
		products_that_have_this_word.insert((*review_history)[i].product_id);
		users_that_have_used_this_word.insert((*review_history)[i].user_id);
	}
	if (products_that_have_this_word.size() < 5) {
		return false;
	}
	if (users_that_have_used_this_word.size() < 10) {
		return false;
	}
	return true;
}

void FindInnovations(int start, vector<pair<string, Review> > *innovations) {
	map<string, vector<Review> > new_words;
	string word;
	vector<Review> temp;
	for (int i = start; i < (int)reviews.size(); i++) {
		stringstream ss(reviews[i].text);
		while (ss.good()) {
			ss >> word;
			if(dictionary.find(word) != dictionary.end()) {
				continue;
			}
			if (new_words.find(word) == new_words.end()) {
				temp.clear();
				temp.push_back(reviews[i]);
				new_words[word] = temp;
			} else {
				temp = new_words.find(word)->second;
				if (temp[temp.size() - 1] == reviews[i]) {
					continue;
				}
				if (TimeDifferenceInMonth(temp[0].time, reviews[i].time) > 6) {
					if (PopWordAddToDictionary(word, &temp) == true) {
						innovations->push_back(make_pair(word, temp[0]));
					}
					new_words.erase(word);
					dictionary.insert(word);
				} else {
					temp.push_back(reviews[i]);
					new_words[word] = temp;
				}
			}
		}
	}
	for (auto x : new_words) {
		word = x.first;
		temp = x.second;
		if (TimeDifferenceInMonth(temp[0].time, temp[temp.size()-1].time) > 6) {
			if (PopWordAddToDictionary(word, &temp) == true) {
				innovations->push_back(make_pair(word, temp[0]));
			}
		}
	}
}

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

void FindNumOfReviews(vector<pair<string, Review> > *innovations,
		set<Innovator> *innovators) {
	for (int i = 0; i < (int)reviews.size(); i++) {
		for (int j = 0; j < (int)innovations->size(); j++) {
			if(reviews[i].user_id == (*innovations)[j].second.user_id) {
				Innovator temp;
				temp.user_id = reviews[i].user_id;
				if(innovators->find(temp) != innovators->end()) {
					temp = *(innovators->find(temp));
					innovators->erase(temp);
				}
				temp.num_of_reviews++;
				if((*innovations)[j].second == reviews[i]) {
					temp.experience_level = temp.num_of_reviews;
				}
				temp.word = (*innovations)[j].first;
				innovators->insert(temp);
			}
		}
	}
}

void AnalyseInnovation(vector<pair<string, Review> > *innovations) {
	set<Innovator> innovators;
	FindNumOfReviews(innovations, & innovators);
	ofstream fout("../Output_All/innovators.txt");
	fout << "word\tuser_id\tnum_of_reviews\texperience_level" << endl;
	for ( Innovator innovator : innovators) {
		cout << innovator.word << "\t" << innovator.user_id << "\t" << innovator.num_of_reviews << "\t" << innovator.experience_level << endl;
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


bool cmp3 (const UserInfo &x,const UserInfo &y) {
	if(x.num_of_reviews == y.num_of_reviews) {
		return x.user_id < y.user_id;
	}
	return x.num_of_reviews < y.num_of_reviews;
}

void UserAngrinessBasedOnNumberOfReviews() {
	FindUserInfo();
	users_info_vec = vector<UserInfo>(users_info.begin(), users_info.end());
	sort(users_info_vec.begin(), users_info_vec.end(), cmp3);
	double current_sum = 0;
	int current_num = 0;
	ofstream fout("../Output_All/user_angriness_based_on_number_of_reviews.txt");
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

map<string, int> counter_for_reviewer;
map<int, int> distribution_for_count_of_reviewer;

int main() {
	// Read input.
	while (true) {
		if (!ReadOneReview()) {
			break;
		}
	}

	//	cerr << reviews.size() <<endl;
	//	MyFilter("product_title", "skirt");
	cerr << reviews.size() << endl;
	sort(reviews.begin(), reviews.end(), cmp);
	reviews.erase(unique( reviews.begin(), reviews.end(), cmp2 ), reviews.end());
	cerr << reviews.size() << endl;
	/*
	CountMonthlyAccumulatedReviews();
	CountYearlyReviews();

	PerItemPerMonth();
	PerItemPerYear();
	// Top products.
	int size_of_list = 40;
	TopProducts(size_of_list);
	// Video Average vs All average.
	ReviewsWithVideo();
	StarAveragePerMonth();
	StarAveragePerYear();
	// Time in Day is useless! The timestamp is on a daily basis
	StarAveragePerTimeInTheDay();
	 */

	/**/
	sort(reviews.begin(), reviews.end());
	LearnDictionary(1500, 1700);
	FindInnovations(2000, &innovations); // returns pair of word and review it was started.
	AnalyseInnovation(&innovations);
	/**/
//	UserAngrinessBasedOnNumberOfReviews();
	cout<<"______________________________"<<endl;
	counter_for_reviewer.clear();
	distribution_for_count_of_reviewer.clear();
	for (Review x : reviews) {
		counter_for_reviewer[x.user_id]++;
	}
	for (pair<string, int> y : counter_for_reviewer) {
		distribution_for_count_of_reviewer[y.second]++;
	}
	for (pair<int, int> y : distribution_for_count_of_reviewer ){
		cout << y.first << " " << y.second << endl;
	}

	return 0;
}
