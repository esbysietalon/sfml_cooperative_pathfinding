#pragma once
#include "playable.h"
#include <deque>
#include <set>
#include "vertEntry.h"

enum move_t;

class PathFinder {
public:
	
	std::deque<move_t>* findPath(intpair start, intpair end);
private:
	void init();
	void updateVertex(vertEntry u);
	void computeShortestPath();
	void calculateKey(intpair s);

	float c(intpair s, intpair s_);
	float g(intpair s_);
	float rhs(intpair s, intpair s_);

	std::set<vertEntry, entry_compare> pq;
	intpair s_start, s_goal;
};