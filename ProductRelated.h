/*
 * ProductRelated.h
 *
 *  Created on: Oct 3, 2014
 *      Author: rahmtin
 */

#ifndef PRODUCTRELATED_H_
#define PRODUCTRELATED_H_

#include<algorithm>
#include<iostream>
#include<fstream>
#include<string>
#include<iterator>
#include<map>

#include "AmazonUtility.h"
#include "Reviews.h"

using namespace std;
typedef map<pair<string, int>, int> ProductTimeCount;



// Number of reviews in different months for a particular item.
// Separate years are not accumulated.
void PerItemPerMonth(vector<Review> *reviews) {
	// item_count_per_month has year*100+month as the time window.
	// map from (product_id, time range) -> count
	map<pair<string, int>, int> item_count_per_month;

	// Map from user_id to the number of reviews they have
	for (int i = 0; i < (int)reviews->size(); i++) {
		pair<string, int> product_id_time_range;
		product_id_time_range = make_pair((*reviews)[i].product_id,
						(*reviews)[i].time.year*100 + (*reviews)[i].time.month);
		item_count_per_month[product_id_time_range]++;
	}
	ProductTimeCount::iterator before;
	ofstream fout;
	for ( ProductTimeCount :: iterator current = item_count_per_month.begin();
			current != item_count_per_month.end(); current++){
		if (current == item_count_per_month.begin() ||
				before->first.first != current->first.first) {
			fout.close();
			fout.open(("../Output_All/PerItem/" + current->first.first + "_monthly_" + Global::NAMEOFDATASET +".txt").c_str(),std::ofstream::out);
			//	fout << current ->first.first << endl;
		}
		fout << month[current->first.second%100] << "/" <<
				current->first.second/100 << " " << current->second <<endl;
		before = current;
	}
}

// Number of reviews in different years for a particular item.
void PerItemPerYear(vector<Review> *reviews) {
	map<pair<string, int>, int> item_count_per_year;
	for (int i = 0; i < (int)reviews->size(); i++) {
		pair<string, int> product_id_time_range;
		product_id_time_range = make_pair((*reviews)[i].product_id, (*reviews)[i].time.year);
		item_count_per_year[product_id_time_range]++;
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
void TopProducts(int size_of_list, vector<Review> *reviews) {
	map<string, int> product_count;
	vector<Product> products;
	for (int i = 0; i <(int) reviews->size(); i++) {
		product_count[(*reviews)[i].product_id]++;
	}
	Product product;
	ofstream top_products_out("../Output_All/top_products_" + Global::NAMEOFDATASET +".txt");
	for (map<string, int>::iterator it = product_count.begin(); it!=product_count.end(); it++) {
		product.product_id = it->first;
		product.count = it->second;
		products.push_back(product);
	}
	sort (products.begin(), products.end());
	for (int i = 0 ; i < size_of_list; i++) {
		top_products_out << products[i].product_id << " " << products[i].count << endl;
	}
}




#endif /* PRODUCTRELATED_H_ */
