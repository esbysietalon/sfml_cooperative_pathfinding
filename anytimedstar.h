#pragma once
#include "intpair.h"
#include "graph.h"
#include "vertEntry.h"
#include <deque>
#include <set>

enum move_t;

class Graph;

class PathFinderPlus {
public:
	PathFinderPlus(Graph* g, int inflation_factor);
	~PathFinderPlus();
	PathFinderPlus();

	std::deque<move_t>* planPath(intpair start, intpair end);
private:
	void reset();

	key getKey(intpair s);
	key topKey(std::set<vertEntry> set);

	void updateState(intpair s);
	void improvePath();

	void setG(intpair s, float score);
	void setRHS(intpair s, float score);
	void setG(vertEntry u, float score);
	void setRHS(vertEntry u, float score);

	float c(intpair s, intpair s_);
	float c(vertEntry u, vertEntry u_);

	float g(intpair s);
	float rhs(intpair s);
	float g(vertEntry u);
	float rhs(vertEntry u);

	float h(intpair s, intpair s_);
	float h(vertEntry u, vertEntry u_);

	Graph* graph;

	std::set<vertEntry> open;
	std::set<vertEntry> closed;
	std::set<vertEntry> incons;
	
	void removeQueue(intpair u, std::set<vertEntry> set);
	void insertQueue(intpair u, key k, std::set<vertEntry> set);
	bool inQueue(intpair u, std::set<vertEntry> set);

	void removeQueue(vertEntry u, std::set<vertEntry> set);
	void insertQueue(vertEntry u, std::set<vertEntry> set);
	bool inQueue(vertEntry u, std::set<vertEntry> set);

	int epsilon;
	int epsilon_i;
	float* gScores;
	float* rhsScores;

	intpair s_start, s_goal;
};