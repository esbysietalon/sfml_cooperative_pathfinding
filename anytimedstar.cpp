#include "stdafx.h"
#include "anytimedstar.h"

void PathFinderPlus::insertQueue(intpair u, key k, std::set<vertEntry> set)
{
	vertEntry entry = vertEntry(u, k);
	set.insert(entry);
}

void PathFinderPlus::removeQueue(intpair u, std::set<vertEntry> set) {
	for (std::set<vertEntry>::iterator it = set.begin(); it != set.end(); it++) {
		if ((*it).vertex == u) {
			set.erase(*it);
			return;
		}
	}
}

bool PathFinderPlus::inQueue(intpair u, std::set<vertEntry> set) {
	for (std::set<vertEntry>::iterator it = set.begin(); it != set.end(); it++) {
		if ((*it).vertex == u) {
			return true;
		}
	}
	return false;
}

void PathFinderPlus::insertQueue(vertEntry u, std::set<vertEntry> set)
{
	set.insert(u);
}

void PathFinderPlus::removeQueue(vertEntry u, std::set<vertEntry> set) {
	for (std::set<vertEntry>::iterator it = set.begin(); it != set.end(); it++) {
		if ((*it).vertex == u.vertex) {
			set.erase(*it);
			return;
		}
	}
}

bool PathFinderPlus::inQueue(vertEntry u, std::set<vertEntry> set) {
	for (std::set<vertEntry>::iterator it = set.begin(); it != set.end(); it++) {
		if ((*it).vertex == u.vertex) {
			return true;
		}
	}
	return false;
}

PathFinderPlus::PathFinderPlus(Graph * g, int inflation_factor)
{
	graph = g;
	epsilon_i = inflation_factor;
	int size = graph->width() * graph->height();
	gScores = new float[size];
	rhsScores = new float[size];
	reset();
}

std::deque<move_t>* PathFinderPlus::planPath(intpair start, intpair end)
{
	s_start = start;
	s_goal = end;

	return nullptr;
}

key PathFinderPlus::topKey(std::set<vertEntry> set) {
	return (*set.begin()).key;
}

float PathFinderPlus::c(intpair s, intpair s_) {
	return graph->costAdj(s, s_);
}
float PathFinderPlus::c(vertEntry u, vertEntry u_) {
	return graph->costAdj(u.vertex, u_.vertex);
}

void PathFinderPlus::updateState(intpair s)
{
	if (s != s_goal) {
		int len;
		intpair* succ = new intpair[8];
		graph->succ(s, &succ, &len);
		
		float minScore = INFINITY;
		for (int i = 0; i < len; i++) {
			float currScore = c(s, succ[i]) + g(succ[i]);
			if (currScore < minScore) {
				minScore = currScore;
			}
		}
		setRHS(s, minScore);
	}
	if (inQueue(s, open)) {
		removeQueue(s, open);
	}
	if (g(s) != rhs(s)) {
		if (!inQueue(s, closed)) {
			insertQueue(s, getKey(s), open);
		}
		else {
			insertQueue(s, getKey(s), incons);
		}
	}
}

void PathFinderPlus::improvePath() {
	while (topKey(open) < getKey(s_start) || rhs(s_start) != g(s_start)) {
		vertEntry s = *open.begin();
		open.erase(s);
		
		if (g(s) > rhs(s)) {
			setG(s, rhs(s));
			insertQueue(s, closed);

			int len;
			intpair* pred = new intpair[8];
			graph->pred(s.vertex, &pred, &len);
			for (int i = 0; i < len; i++) {
				updateState(pred[i]);
			}
		}
		else {
			setG(s, INFINITY);
			updateState(s.vertex);

			int len;
			intpair* pred = new intpair[8];
			graph->pred(s.vertex, &pred, &len);
			for (int i = 0; i < len; i++) {
				updateState(pred[i]);
			}
		}
		
	}
}

void PathFinderPlus::reset()
{
	epsilon = epsilon_i;
	int size = graph->width() * graph->height();
	for (int i = 0; i < size; i++) {
		gScores[i] = INFINITY;
		rhsScores[i] = INFINITY;
	}
	setRHS(s_goal, 0);
	open.clear();
	closed.clear();
	incons.clear();
	insertQueue(s_goal, getKey(s_goal), open);
}

float PathFinderPlus::h(intpair s, intpair s_) {
	if (s == s_)
		return 0;
	int distX = s.x - s_.x;
	int distY = s.y - s_.y;
	return sqrt(distX * distX + distY * distY);
}
float PathFinderPlus::h(vertEntry u, vertEntry u_) {
	return h(u.vertex, u_.vertex);
}

key PathFinderPlus::getKey(intpair s)
{
	if (g(s) > rhs(s)) {
		return key(rhs(s) + epsilon * h(s_start, s), rhs(s));
	}
	else {
		return key(g(s) + h(s_start, s), g(s));
	}
}

void PathFinderPlus::setG(intpair u, float score) {
	gScores[u.x + u.y * graph->width()] = score;
}
void PathFinderPlus::setRHS(intpair u, float score) {
	rhsScores[u.x + u.y * graph->width()] = score;
}
void PathFinderPlus::setG(vertEntry u, float score) {
	setG(u.vertex, score);
}
void PathFinderPlus::setRHS(vertEntry u, float score) {
	setRHS(u.vertex, score);
}

float PathFinderPlus::g(intpair u) {
	return gScores[u.x + u.y * graph->width()];
}
float PathFinderPlus::rhs(intpair u) {
	return rhsScores[u.x + u.y * graph->width()];
}
float PathFinderPlus::g(vertEntry u) {
	return g(u.vertex);
}
float PathFinderPlus::rhs(vertEntry u) {
	return rhs(u.vertex);
}
