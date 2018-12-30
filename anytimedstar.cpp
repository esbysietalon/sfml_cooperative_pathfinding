#include "stdafx.h"
#include "anytimedstar.h"

void PathFinderPlus::insertQueue(intpair u, key k, std::set<vertEntry>* set)
{
	vertEntry entry = vertEntry(u, k);
	set->insert(entry);
}

void PathFinderPlus::removeQueue(intpair u, std::set<vertEntry>* set) {
	for (std::set<vertEntry>::iterator it = set->begin(); it != set->end(); it++) {
		if ((*it).vertex == u) {
			set->erase(*it);
			return;
		}
	}
}

bool PathFinderPlus::inQueue(intpair u, std::set<vertEntry>* set) {
	for (std::set<vertEntry>::iterator it = set->begin(); it != set->end(); it++) {
		if ((*it).vertex == u) {
			return true;
		}
	}
	return false;
}

void PathFinderPlus::insertQueue(vertEntry u, std::set<vertEntry>* set)
{
	set->insert(u);
}

void PathFinderPlus::removeQueue(vertEntry u, std::set<vertEntry>* set) {
	for (std::set<vertEntry>::iterator it = set->begin(); it != set->end(); it++) {
		if ((*it).vertex == u.vertex) {
			set->erase(*it);
			return;
		}
	}
}

bool PathFinderPlus::inQueue(vertEntry u, std::set<vertEntry>* set) {
	for (std::set<vertEntry>::iterator it = set->begin(); it != set->end(); it++) {
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
	lastMap = new Playable*[size];
}

void PathFinderPlus::replanPath(intpair start, intpair end)
{
	//fprintf(stderr, "replanPath call\n");
	end = getNearestFree(end, start);
	//fprintf(stderr, "post getnearest call\n");
	//fprintf(stderr, "made it here\n");
	reset(start, end);
	//fprintf(stderr, "also here\n");
	improvePath();
	//fprintf(stderr, "replanPath end\n");
}

intpair PathFinderPlus::getNearestFree(intpair tile, intpair origin) {
	if (graph->isFree(tile))
		return tile;
	move_t dir;
	intpair nearest = tile;
	int p = 1;
	while (!graph->isFree(nearest)) {
		//fprintf(stderr, "nearest candidate is (%d,%d) [origin is (%d,%d)] by %d\n", nearest.x, nearest.y, origin.x, origin.y, this);
		int normalX = abs(origin.x - nearest.x);
		int normalY = abs(origin.y - nearest.y);
		if (normalX == 0)
			normalX = 1;
		if (normalY == 0)
			normalY = 1;
		dir = (move_t)((origin.x - nearest.x) / normalX * 4 - (origin.y - nearest.y) / normalY);
		//fprintf(stderr, "dir is %d\n", dir);
		intpair lastnearest = intpair(nearest.x, nearest.y);
		switch (dir) {
		case move_t::NONE:
			for (int yp = -1 * p; yp <= p; yp++) {
				for (int xp = -1 * p; xp <= p; xp++) {
					nearest = intpair(lastnearest.x + xp, lastnearest.y + yp);
					if (graph->isFree(nearest)) {
						return nearest;
					}
				}
			}
			nearest = intpair(lastnearest.x, lastnearest.y);
			p++;
			break;
		case move_t::N:
			nearest = intpair(nearest.x, nearest.y - 1);
			break;
		case move_t::E:
			nearest = intpair(nearest.x + 1, nearest.y);
			break;
		case move_t::W:
			nearest = intpair(nearest.x - 1, nearest.y);
			break;
		case move_t::S:
			nearest = intpair(nearest.x, nearest.y + 1);
			break;
		case move_t::NE:
			nearest = intpair(nearest.x + 1, nearest.y - 1);
			break;
		case move_t::NW:
			nearest = intpair(nearest.x - 1, nearest.y - 1);
			break;
		case move_t::SE:
			nearest = intpair(nearest.x + 1, nearest.y + 1);
			break;
		case move_t::SW:
			nearest = intpair(nearest.x - 1, nearest.y + 1);
			break;
		}
		//fprintf(stderr, "nearest candidate is (%d,%d)\n", tile.x, tile.y, nearest.x, nearest.y);
	}
	//if (tile != nearest)
		
	return nearest;
	//dir is what direction to face standing from origin looking at tile
}

std::deque<move_t>* PathFinderPlus::findPath(intpair start, intpair end) {
	end = getNearestFree(end, start);
	//start = getNearestFree(start, start);
	fprintf(stderr, "finding path from (%d,%d) to (%d,%d)\n", start.x, start.y, end.x, end.y);
	s_start = start;
	s_goal = end;
	std::deque<move_t>* path = new std::deque<move_t>();
	intpair currTile = s_start;
	
	int stickCounter = 0;
	while (currTile != s_goal) {
		if (stickCounter > FUBAR) {
			//fprintf(stderr, "FUBARED in findPath; checking if possible\n");
			if (g(s_start) == INFINITY) {
				//fprintf(stderr, "impossible path; aborting\n");
				resetCount = 0;
				return nullptr;
			}
			//fprintf(stderr, "possible path claimed; resetting (this is %d btw)\n", this);
			resetCount++;
			replanPath(start, end);
			return findPath(start, end);
		}
		stickCounter++;

		float minCost = INFINITY;
		intpair nextTile = intpair(-1, -1);
		int len = 0;
		intpair* succ = new intpair[8];
		graph->succ(currTile, &succ, &len);

	
		for (int i = 0; i < len; i++) {
			float currCost = c(currTile, succ[i]) + g(succ[i]);
			if (currCost < minCost) {
				minCost = currCost;
				nextTile = succ[i];
			}
		}
		if (nextTile == intpair(-1, -1) && resetCount > HARD_FUBAR) {
			fprintf(stderr, "path is hard fubar; ignoring instruction\n", this);
			resetCount = 0;
			return nullptr;
		}
		fprintf(stderr, "nextTile is (%d,%d) by %d\n", nextTile.x, nextTile.y, this);
		move_t nextMove = (move_t)(-1 * (currTile.x - nextTile.x) * 4 + currTile.y - nextTile.y);

		path->push_back(nextMove);
		currTile = nextTile;
	}
	resetCount = 0;
	//fprintf(stderr, "returning path by %d\n", this);
	return path;
}

key PathFinderPlus::topKey(std::set<vertEntry>* set) {
	//fprintf(stderr, "topKey call on a set of size %d\n",set->size());
	if (set->empty()) {
		return key(INFINITY, INFINITY);
	}
	return (*set->begin()).key;
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

void PathFinderPlus::planMore() {
	//fprintf(stderr, "planMore call\n");
	int replan = observeChanges();
	if (replan > 0) {
		if(replan == 2){
			return replanPath(s_start, s_goal);
		}
		else {
			epsilon += EPSILON_STEP + 1;
		}
		//fprintf(stderr, "latestChanges are %d\n", latestChanges.size());
		for (int i = 0; i < latestChanges.size(); i++) {
			intpair u = latestChanges.at(i);
			updateState(u);
		}
	}
	if (epsilon > 1) {
		epsilon--;
		//fprintf(stderr, "in plan more loop\n");
		if (open->size() > 0) {
			for (std::set<vertEntry>::iterator it = open->begin(); it != open->end(); it++) {
				insertQueue(*it, incons);
			}
			open->clear();
		}
		if (incons->size() > 0) {
			for (std::set<vertEntry>::iterator it = incons->begin(); it != incons->end(); it++) {
				insertQueue((*it).vertex, getKey((*it).vertex), open);
			}
			incons->clear();
		}
		
		closed->clear();
		//fprintf(stderr, "end transfer\n");
		improvePath();
		//fprintf(stderr, "end improve path\n");
	}
	//fprintf(stderr, "planMore end\n");
}

bool PathFinderPlus::observeChanges() {
	latestChanges.clear();
	int size = graph->height() * graph->width();
	Playable** currMap = new Playable*[size];
	graph->getMapCopy(&currMap, s_start);
	int changes = 0;
	for (int j = s_start.y - LOOK_AHEAD_DIST; j <= s_start.y + LOOK_AHEAD_DIST; j++) {
		for (int i = s_start.x - LOOK_AHEAD_DIST; i <= s_start.x + LOOK_AHEAD_DIST; i++) {
			int currIndex = i + j * graph->width();
			if (currIndex < 0 || currIndex >= size)
				continue;
			if (currMap[currIndex] != lastMap[currIndex]) {
				latestChanges.push_back(intpair(i, j));
				changes++;
			}
		}
	}
	delete[] currMap;
	graph->getMapCopy(&lastMap, s_start);
	//fprintf(stderr, "change percent: %f\n", changes / (float)size);
	if (changes / (float)size > MAP_CHANGE_TOLERANCE_LARGE) {
		return 2;
	}
	if (changes / (float)size > MAP_CHANGE_TOLERANCE_SMALL) {
		return 1;
	}
	return 0;
}

void PathFinderPlus::improvePath() {
	//fprintf(stderr, "improvePath call\n");
	int stickCount = 0;
	while (topKey(open) < getKey(s_start) || rhs(s_start) != g(s_start)) {
		//fprintf(stderr, "in while loop\n");
		if (stickCount > FUBAR) {
			fprintf(stderr, "FUBARED in improvePath; checking if possible\n");
			if (g(s_start) == INFINITY) {
				fprintf(stderr, "impossible path from (%d,%d) to (%d,%d); aborting\n", s_start.x, s_start.y, s_goal.x, s_goal.y);
				resetCount = HARD_FUBAR + 1;
				return;
			}
			fprintf(stderr, "possible path claimed; resetting\n");
			//s_start = getNearestFree(s_start, s_start);
			//s_goal = getNearestFree(s_goal, s_goal);
			return replanPath(s_start, s_goal);
		}
		stickCount++;
		vertEntry s = *open->begin();
		open->erase(s);
		
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

void PathFinderPlus::reset(intpair start, intpair end)
{
	s_start = start;
	s_goal = end;
	epsilon = epsilon_i;
	int size = graph->width() * graph->height();
	for (int i = 0; i < size; i++) {
		gScores[i] = INFINITY;
		rhsScores[i] = INFINITY;
		lastMap[i] = 0;
	}
	
	open = new std::set<vertEntry>();
	closed = new std::set<vertEntry>();
	incons = new std::set<vertEntry>();
	setRHS(s_goal, 0);
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
