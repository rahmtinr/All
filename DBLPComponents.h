/*
 * DBLPComponents.cpp
 *
 *  Created on: Dec 23, 2014
 *      Author: rahmtin
 */



#ifndef DBLPCOMPONENTS_H_
#define DBLPCOMPONENTS_H_

#include<ctime>
#include<cstdlib>
#include<fstream>
#include<map>
#include<string>

#include "AmazonUtility.h"
#include "MyUtility.h"
#include "ReviewReader.h"

using namespace std;

vector<int> a[2*1000*1000];
int mark[2*1000*1000];
int num_of_nodes = 0;
int author_component[2 * 1000 * 1000];
int current_comp;
set<int> fake_nodes;

void dfs(int x) {
	author_component[x]  = current_comp;
	num_of_nodes ++;
	mark[x] = 1;
	for(int i = 0; i < (int) a[x].size(); i++) {
		if(mark[a[x][i]] == 0) {
			dfs(a[x][i]);
		}
	}
}

void BuildGraph(set<pair<int, int> > *edges) {
	if(a[edges->begin()->first].size() > 0) {
		return;
	}
	for (pair<int, int> edge : *edges) {
		a[edge.first].push_back(edge.second);
		a[edge.second].push_back(edge.first);
	}
}
void RemoveFakeNodes(set<pair<int,int>> *edges, int counter){
	BuildGraph(edges);
	set<int> neighbors;
	double coeff;
	for(int i = 1; i < counter; i++) {
		neighbors.clear();
		for(int j = 0; j < (int)a[i].size(); j++) {
			neighbors.insert(a[i][j]);
		}
		int local_edge = 0;
		for(int neighbor : neighbors) {
			for(int j = 0; j < (int)a[neighbor].size(); j++) {
				if(neighbors.find(a[neighbor][j]) != neighbors.end()) {
					local_edge++;
				}
			}
		}
		if(neighbors.size() < 50) {
			coeff = 1;
		} else {
			coeff = (double)local_edge / (double)(neighbors.size() * (neighbors.size() - 1));
		}
		if(i == 108504) { // Jon.
			cerr << "Jon: " << coeff << endl;
		}
		if(coeff < 1.2 * (1e-2)) {
			fake_nodes.insert(i);
			mark[i] = 1;
		}
	}
	cerr << "Fake nodes size: " <<  fake_nodes.size() << endl;
}

void Components(set<pair<int, int> > *edges, int counter){
	int max_component = 0;
	int min_component = 2 * 1000 * 1000;
	int comp = 0;
	BuildGraph(edges);
	for( int i = 1; i < counter; i++) {
		if(mark[i] == 0) {
			current_comp = comp;
			dfs(i);
			comp++;
			max_component = max(max_component, num_of_nodes);
			min_component = min(min_component, num_of_nodes);
			num_of_nodes = 0;
		}
	}
	cerr << "Number of components: " << comp << endl;
	cerr << "Max component size: " << max_component << endl;
	cerr << "Min component size: " << min_component << endl;
}

void DfsOverValid(string word, set<int> *valid_nodes, double &best_ratio, double &worst_ratio, string &best_word, string &worst_word) {
	memset(mark,-1,sizeof mark);
	for(int x : *valid_nodes) {
		mark[x] = 0;
	}
	int innovators_comp = 0;
	int biggest_comp = 0;
	int second_comp = 0;
	for(int x : *valid_nodes) {
		if(mark[x] == 0) {
			num_of_nodes = 0;
			dfs(x);
			if(num_of_nodes >= biggest_comp) {
				second_comp = biggest_comp;
				biggest_comp = num_of_nodes;
			} else if(num_of_nodes > second_comp) {
				second_comp = num_of_nodes;
			}
			innovators_comp++;
		}
	}
	double ratio = biggest_comp / (double) valid_nodes->size();
	if(ratio < worst_ratio) {
		worst_ratio = ratio;
		worst_word = word;
	}
	if(ratio > best_ratio) {
		best_ratio = ratio;
		best_word = word;
	}
//	cerr << "Ratio of biggest components: " << biggest_comp / (double)valid_nodes->size() << " " << second_comp / (double)valid_nodes->size() << endl;
//	cerr << "The number of components on the induced graph of innovators is: " << innovators_comp << endl;
}
#endif /* DBLPCOMPONENTS_H_ */

