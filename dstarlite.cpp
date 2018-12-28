#include "stdafx.h"
#include "dstarlite.h"

PathFinder::PathFinder(Graph* g)
{
	graph = g;
	int size = g->width() * g->height();
	gScores = (float*)malloc(sizeof(float) * size);
	rhsScores = (float*)malloc(sizeof(float) * size);
	for(int i = 0; i < size; i++){
		gScores[i] = INFINITY;
		rhsScores[i] = INFINITY;
	}
	km = 0;
	bigU.clear();
	
}

void PathFinder::reset() {
	int size = graph->width() * graph->height();
	for (int i = 0; i < size; i++) {
		gScores[i] = INFINITY;
		rhsScores[i] = INFINITY;
	}
	km = 0;
	bigU.clear();
}

intpair PathFinder::getNearestFree(intpair tile, intpair origin) {
	if (graph->isFree(tile))
		return tile;
	move_t dir;
	intpair nearest = tile;
	while (!graph->isFree(nearest)) {
		int normalX = abs(origin.x - nearest.x);
		int normalY = abs(origin.y - nearest.y);
		if (normalX == 0)
			normalX = 1;
		if (normalY == 0)
			normalY = 1;
		dir = (move_t)((origin.x - nearest.x) / normalX * -4 + (origin.y - nearest.y) / normalY);

		switch (dir) {
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
	}
	return nearest;
	//dir is what direction to face standing from origin looking at tile
}

std::deque<move_t>* PathFinder::findPath(intpair start, intpair end)
{
	
	
	end = getNearestFree(end, start);
	//fprintf(stderr, "finding path from (%d,%d) to (%d,%d)\n", start.x, start.y, end.x, end.y);

	if (start == end)
		return nullptr;
	

	//fprintf(stderr, "we make it here\n");
	reset();

	s_start = start;
	s_goal = end;
	//fprintf(stderr, "and here\n");
	std::deque<move_t>* path = new std::deque<move_t>();
	//fprintf(stderr, "we make it there\n");
	setRHS(s_goal, 0);
	insertBigU(s_goal, key(h(s_start, s_goal), 0));
	//fprintf(stderr, "and there\n");
	computeShortestPath();
	//fprintf(stderr, "after compute call\n");
	/*insertBigU(intpair(0, 0), key(3.0, 2.0));
	insertBigU(intpair(0, 1), key(3.0, 1.0));
	insertBigU(intpair(0, 2), key(2.0, 3.0));
	insertBigU(intpair(0, 3), key(0.1, 0.1));
	updateBigU(intpair(0, 3), key(10.0, 10.0));
	int usize = bigU.size();
	fprintf(stderr, "U's size is: %d\n", usize);
	if (inBigU(intpair(0, 2)))
		fprintf(stderr, "(0, 2) is in U\n");
	else
		fprintf(stderr, "(0, 2) is not in U\n");
	usize = bigU.size();
	fprintf(stderr, "U's size is: %d\n", usize);
	
	for (int i = 0; i < usize; i++) {
		vertEntry currEntry = popBigU();
		fprintf(stderr, "(%d,%d),[%f,%f]\n", currEntry.vertex.x, currEntry.vertex.y, currEntry.key.k1, currEntry.key.k2);
	}
	usize = bigU.size();
	fprintf(stderr, "U's size is: %d\n", usize);
	insertBigU(intpair(0, 0), key(3.0, 2.0));
	insertBigU(intpair(0, 1), key(3.0, 1.0));
	insertBigU(intpair(0, 2), key(3.0, 3.0));
	removeBigU(intpair(0, 0));
	usize = bigU.size();
	fprintf(stderr, "U's size is: %d\n", usize);
	for (int i = 0; i < usize; i++) {
		vertEntry currEntry = popBigU();
		fprintf(stderr, "(%d,%d),[%f,%f]\n", currEntry.vertex.x, currEntry.vertex.y, currEntry.key.k1, currEntry.key.k2);
	}*/
	
	intpair currTile = s_start;
	//fprintf(stderr, "scanning through graph for shortest path\n");
	//fprintf(stderr, "returning path:\n");
	while (currTile != s_goal) {
		
		float minCost = FLT_MAX;
		intpair nextTile = intpair(-1, -1);
		int len = 0;
		intpair* succ = new intpair[8];
		graph->succ(currTile, &succ, &len);
		
		//fprintf(stderr, "currTile is (%d,%d)\n", currTile.x, currTile.y);
		
		for (int i = 0; i < len; i++) {
			//fprintf(stderr, "in for loop, %d to %d\n", i, len);
			//fprintf(stderr, "current successor is (%d,%d)\n", succ[i].x, succ[i].y);
			float currCost = c(currTile, succ[i]) + g(succ[i]);
			//fprintf(stderr, "c is %f while g is %f\n", c(currTile, succ[i]), g(succ[i]));
			if (currCost < minCost) {
				minCost = currCost;
				nextTile = succ[i];
				//fprintf(stderr, "nextTile candidate is (%d,%d)\n", nextTile.x, nextTile.y);
			}
		}
		//fprintf(stderr, "nextTile is (%d,%d)\n", nextTile.x, nextTile.y);
		//fprintf(stderr, "minCost is %f\n", minCost);
		move_t nextMove = (move_t)(-1 * (currTile.x - nextTile.x) * 4 + currTile.y - nextTile.y);
		//fprintf(stderr, "%d ", nextMove);
		path->push_back(nextMove);
		currTile = nextTile;
	}
	
	//fprintf(stderr, "\n");
	return path;
}

void PathFinder::updateVertex(vertEntry u)
{
	if (u.vertex != s_goal) {
		//fprintf(stderr, "if #1\n");
		intpair* succ = new intpair[8];

		float minCost = INFINITY;

		int len = 0;
		graph->succ(u.vertex, &succ, &len);
		//fprintf(stderr, "len of succ is %d\n", len);
		for (int i = 0; i < len; i++) {
			float currCost = c(u.vertex, succ[i]) + g(succ[i]);
			//fprintf(stderr, "given (%d,%d) and (%d,%d), currCost is %f, comprised of c(u,s') (%f) + g(s') (%f)\n", u.vertex.x, u.vertex.y, succ[i].x, succ[i].y, currCost, c(u.vertex, succ[i]), g(succ[i]));
			if (currCost < minCost) {
				minCost = currCost;
			}
		}
		setRHS(u.vertex, minCost);
	}
	if (inBigU(u)) {
		//fprintf(stderr, "if #2\n");
		removeBigU(u.vertex);
	}
	if (g(u.vertex) != rhs(u.vertex)) {
		//fprintf(stderr, "if #3\n");
		insertBigU(u.vertex, calculateKey(u.vertex));
	}
}
void PathFinder::updateVertex(intpair s)
{
	//fprintf(stderr, "updateVertex intpair call\n");
	if (s != s_goal) {
		//fprintf(stderr, "if #1\n");
		intpair* succ = new intpair[8];
		
		float minCost = INFINITY;
		
		int len = 0;
		graph->succ(s, &succ, &len);
		//fprintf(stderr, "len of succ is %d\n", len);
		for (int i = 0; i < len; i++) {
			float currCost = c(s, succ[i]) + g(succ[i]);
			//fprintf(stderr, "given (%d,%d) and (%d,%d), currCost is %f, comprised of c(u,s') (%f) + g(s') (%f)\n", s.x, s.y, succ[i].x, succ[i].y, currCost, c(s, succ[i]), g(succ[i]));
			if (currCost < minCost) {
				minCost = currCost;
			}
		}
		setRHS(s, minCost);
	}
	if (inBigU(s)) {
		//fprintf(stderr, "if #2\n");
		removeBigU(s);
	}
	if (g(s) != rhs(s)) {
		//fprintf(stderr, "if #3\n");
		insertBigU(s, calculateKey(s));
	}
	/*if (g(s) != rhs(s) && inBigU(s)) {
		fprintf(stderr, "if #1\n");
		updateBigU(s, calculateKey(s));
	}
	else if (g(s) != rhs(s) && !inBigU(s)) {
		fprintf(stderr, "if #2\n");
		insertBigU(s, calculateKey(s));
	}
	else if (g(s) == rhs(s) && inBigU(s)) {
		fprintf(stderr, "if #3\n");
		removeBigU(s);
	}*/
	//fprintf(stderr, "end updateVertex call\n");
}

void PathFinder::computeShortestPath() {
	//fprintf(stderr, "we make it here as well\n");
	while (topKey() < calculateKey(s_start) || rhs(s_start) != g(s_start)) {
		//fprintf(stderr, "in the while loop\n");
		vertEntry u = *bigU.begin();
		key k_old = topKey();
		key k_new = calculateKey(u.vertex);
		//fprintf(stderr, "still in the loop\n");
		//fprintf(stderr, "k_old is [%f,%f] k_new is [%f,%f]\n", k_old.k1, k_old.k2, k_new.k1, k_new.k2);
		//fprintf(stderr, "g(u) is %f rhs(u) is %f\n", g(u.vertex), rhs(u.vertex));
		
		if (k_old < k_new) {
			fprintf(stderr, "if #1 start\n");
			updateBigU(u, k_new);
			////fprintf(stderr, "if #1 end\n");
		}
		else if (g(u.vertex) > rhs(u.vertex)) {
			//fprintf(stderr, "if #2 start\n");
			setG(u.vertex, rhs(u.vertex));
			removeBigU(u);
			int len = 0;
			//fprintf(stderr, "before pred\n");
			intpair* pred = new intpair[8];
			graph->pred(u.vertex, &pred, &len);
			for (int i = 0; i < len; i++) {
				//fprintf(stderr, "pred[%d] = (%d,%d)\n", i, pred[i].x, pred[i].y);
			}
			//fprintf(stderr, "after pred\n");
			for (int i = 0; i < len; i++) {
				//fprintf(stderr, "in pred loop at %d to %d\n", i, len);
				//fprintf(stderr, "RHS is set\n");
				//fprintf(stderr, "updating (%d,%d)\n", pred[i].x, pred[i].y);
				updateVertex((pred[i]));
			}
			//fprintf(stderr, "if #2 end\n");
		}
		else {
			//fprintf(stderr, "if #3 start\n");
			float g_old = g(u.vertex);
			setG(u.vertex, INFINITY);

			if (rhs(u.vertex) == c(u.vertex, u.vertex) + g_old){
				if (u.vertex != s_goal) {
					float minCost = FLT_MAX;
					int slen = 0;
					intpair* succ = new intpair[8];
					graph->succ(u.vertex, &succ, &slen);
					for (int j = 0; j < slen; j++) {
						if (c(u.vertex, succ[j]) + g(succ[j]) < minCost)
							minCost = c(u.vertex, succ[j]) + g(succ[j]);
					}
					setRHS(u.vertex, minCost);
				}
			}
			updateVertex(u.vertex);

			int len = 0;
			intpair* pred = new intpair[8];
			graph->pred(u.vertex, &pred, &len);
			
			for (int i = 0; i < len; i++) {
				if(rhs(pred[i]) == c(pred[i], u.vertex) + g_old)
					if (pred[i] != s_goal) {
						float minCost = FLT_MAX;
						int slen = 0;
						intpair* succ = new intpair[8];
						graph->succ(pred[i], &succ, &slen);
						for (int j = 0; j < slen; j++) {
							if (c(pred[i], succ[j]) + g(succ[j]) < minCost)
								minCost = c(pred[i], succ[j]) + g(succ[j]);
						}
						setRHS(pred[i], minCost);
					}
				updateVertex(pred[i]);
			}

		}
		/*if (!(topKey() < calculateKey(s_start) || rhs(s_start) > g(s_start))) {
			//fprintf(stderr, "topKey is [%f,%f], calculateKey(s_start) is [%f,%f]\n", topKey().k1, topKey().k2, calculateKey(s_start).k1, calculateKey(s_start).k2);
			//fprintf(stderr, "rhs(s_start) is %f, g(s_start) is %f\n", rhs(s_start), g(s_start));
		}*/
	}
}

void PathFinder::setRHS(intpair s, float score){
	rhsScores[s.x + s.y * graph->width()] = score;
}
void PathFinder::setG(intpair s, float score) {
	//fprintf(stderr, "score for (%d,%d) is now %f\n", s.x, s.y, score);
	gScores[s.x + s.y * graph->width()] = score;
}

key PathFinder::calculateKey(intpair s) {
	return key(std::min(g(s), rhs(s)) + h(s_start, s) + km, std::min(g(s), rhs(s)));
}
key PathFinder::topKey(){
	if(bigU.empty()){
		return key(INFINITY, INFINITY);
	}
	return (*bigU.begin()).key;
}

bool PathFinder::inBigU(intpair s) {
	//fprintf(stderr, "intpair inBigU call\n");
	std::set<vertEntry> copyU = bigU;
	if (removeBigU(s)) {
		bigU = copyU;
		return true;
	}
	return false;
}
bool PathFinder::inBigU(vertEntry u) {
	return bigU.count(u) > 0;
}

void PathFinder::insertBigU(intpair s, key k) {
	vertEntry entry = vertEntry(s, k);
	bigU.insert(entry);
}
void PathFinder::updateBigU(vertEntry u, key k) {
	vertEntry newEntry = vertEntry(u.vertex, k);
	bigU.erase(u);
	bigU.insert(newEntry);
}
void PathFinder::updateBigU(intpair s, key k) {
	removeBigU(s);
	vertEntry newEntry = vertEntry(s, k);
	bigU.insert(newEntry);
}
vertEntry PathFinder::popBigU() {
	vertEntry out = *bigU.begin();
	bigU.erase(*bigU.begin());
	return out;
}
bool PathFinder::removeBigU(intpair s) {
	//fprintf(stderr, "intpair removeBigU call\n");
	if (bigU.empty())
		return false;
	vertEntry desired_entry = vertEntry(s, key());
	vertEntry curr_entry = *bigU.begin();
	for (std::set<vertEntry>::iterator it = bigU.begin(); it != bigU.end(); it++) {
		curr_entry = *it;
		//fprintf(stderr, "curr_entry is (%d,%d),[%f,%f] and desired_entry is (%d,%d),[%f,%f]\n", curr_entry.vertex.x, curr_entry.vertex.y, curr_entry.key.k1, curr_entry.key.k2, desired_entry.vertex.x, desired_entry.vertex.y, desired_entry.key.k1, desired_entry.key.k2);
		if (curr_entry == desired_entry) {
			bigU.erase(curr_entry);
			return true;
		}
	}
	return false;
}
bool PathFinder::removeBigU(vertEntry u) {
	bool wasIn = inBigU(u);
	bigU.erase(u);
	return wasIn;
}

float PathFinder::g(intpair s){
	return gScores[s.x + s.y * graph->width()];
}
float PathFinder::c(intpair s, intpair s_){
	return graph->costAdj(s, s_);
}
float PathFinder::h(intpair s, intpair s_){
	int distX = s.x - s_.x;
	int distY = s.y - s_.y;
	return sqrt(distX * distX + distY * distY);
}
float PathFinder::rhs(intpair s)
{
	return rhsScores[s.x + s.y * graph->width()];
}
