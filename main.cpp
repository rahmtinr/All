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

#include "Aggregation.h"
#include "AmazonUtility.h"
#include "Innovations.h"
#include "MyUtility.h"
#include "ProductRelated.h"
#include "ReviewReader.h"
#include "Reviews.h"
#include "Seasonal.h"
#include "UserRelated.h"
#include "Videos.h"
using namespace std;


vector<Review> reviews;

// Langauge innovations
vector<pair<string, vector<Review> > >  innovations[100];




int main(int argc, char *argv[]) {
	string MODE = "";
	//	MODE = "_Top3TaxRemoved";
	Global::NAMEOFDATASET = (string(argv[1])).substr(string(argv[1]).find("/") + 1 ,
			string(argv[1]).find(".") - string(argv[1]).find("/") - 1) + MODE;
	cerr << Global::NAMEOFDATASET <<endl;
	Innovations::numbers_for_appearances = {4 , 8, 10};
	Innovations::numbers_for_products = {3 , 6, 10};
	Innovations::numbers_for_authors = {3, 5, 7};
	ifstream fin(argv[1]);
	Amazon::Global::min_year = 2015;
	Amazon::Global::max_year = 1995;
	// Read input.
	while (true) {
		if (!ReadOneReview(fin, &reviews)) {
			break;
		}
		Amazon::Global::min_year = min(Amazon::Global::min_year, reviews[reviews.size() - 1].time.year);
		Amazon::Global::max_year = max(Amazon::Global::max_year, reviews[reviews.size() - 1].time.year);
	}
	//	cerr << reviews.size() <<endl;
	//	MyFilter("text", "turbotax");
	//	MyFilter("text", "hr block", &reviews);
	//	MyFilter("text", "taxact", &reviews);
	cerr << reviews.size() << endl;
	sort(reviews.begin(), reviews.end(), CompareReviewOnUserId);
	reviews.erase(unique( reviews.begin(), reviews.end(), ReviewEquality), reviews.end());
	cerr << reviews.size() << endl;
	/**/
	// CountMonthlyAccumulatedReviews(&reviews);
	// CountYearlyReviews(&reviews);
	// PerItemPerMonth(&reviews);
	// PerItemPerYear(&reviews);
	// Top products.
	// int size_of_list = 40;
	// TopProducts(size_of_list, &reviews);
	// Video Average vs All average.
	// ReviewsWithVideo(&reviews);
	// StarAveragePerYear(&reviews);
	// Time in Day is useless! The timestamp is on a daily basis
	// StarAveragePerTimeInTheDay(&reviews);
	//
	// sort(reviews.begin(), reviews.end());
	Innovations::LearnDictionary(0, reviews.size() / 2, &reviews);
	Innovations::FindInnovations(reviews.size() / 2, &reviews, innovations); // returns pair of word and review it was started.
	Innovations::AnalyseInnovation(innovations, &reviews);
	//	UserDistributionBasedOnNumberOfReviews(&reviews);
	/**/
	//	UserAngrinessBasedOnNumberOfReviews(&reviews);
	//	StarAveragePerMonth(&reviews);
	//	StarAveragePerMonthAccumulatedOverYears(&reviews);
	return 0;
}
