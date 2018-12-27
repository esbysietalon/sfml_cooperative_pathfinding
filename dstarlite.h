#pragma once
#include "playable.h"
#include <deque>
#include <set>
#include <algorithm>
#include "vertEntry.h"
#include "graph.h"

enum move_t;

class Graph;

class PathFinder {
public:
	PathFinder(Graph* g);
	PathFinder();
	~PathFinder();

	std::deque<move_t>* findPath(intpair start, intpair end);
	void reset();
	std::deque<move_t>* updatePath(intpair start, intpair end);
private:
	intpair getNearestFree(intpair tile, intpair origin);

	void updateVertex(vertEntry u);
	void updateVertex(intpair s);
	void computeShortestPath();
	key calculateKey(intpair s);

	void setRHS(intpair s, float score);
	void setG(intpair s, float score);

	float c(intpair s, intpair s_);
	float g(intpair s);
	float rhs(intpair s);
	float h(intpair s, intpair s_);

	float km;

	std::set<vertEntry> bigU;
	key topKey();
	void insertBigU(intpair s, key k);
	void updateBigU(intpair s, key k);
	void updateBigU(vertEntry u, key k);
	vertEntry popBigU();
	bool removeBigU(intpair s);
	bool removeBigU(vertEntry u);
	bool inBigU(intpair s);
	bool inBigU(vertEntry u);

	Graph* graph;
	float* gScores;
	float* rhsScores;
	intpair s_start = intpair(), s_goal = intpair();
};