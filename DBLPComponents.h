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
#include<string>

#include "AmazonUtility.h"
#include "MyUtility.h"
#include "ReviewReader.h"

using namespace std;

vector<int> a[2*1000*1000];
int mark[2*1000*1000];
int num_of_nodes = 0;

void dfs(int x) {
	num_of_nodes ++;
	mark[x] = 1;
	for(int i = 0; i < (int) a[x].size(); i++) {
		if(mark[a[x][i]] == 0) {
			dfs(a[x][i]);
		}
	}
}

void Components(set<pair<int, int> > *edges, int counter){
	int max_component = 0;
	int min_component = 2 * 1000 * 1000;
	for (pair<int, int> edge : *edges ) {
		a[edge.first].push_back(edge.second);
		a[edge.second].push_back(edge.first);
	}
	int comp = 0;
	for( int i = 1; i < counter; i++) {
		if(mark[i] == 0) {
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

#endif /* DBLPCOMPONENTS_H_ */

