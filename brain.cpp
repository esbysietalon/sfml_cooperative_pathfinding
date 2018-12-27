#include "stdafx.h"
#include "brain.h"

std::random_device                  rand_dev;
std::mt19937                        generator(rand_dev());
std::uniform_int_distribution<int>  distr(0, 1000000);

Brain::Brain() {

}
Brain::~Brain() {

}

void Brain::setRMap(Playable*** rMap) {
	rmap = rMap;
}

void Brain::see() {
	
	//implement cfov
	//reset sightmaps
	//fprintf(stderr, "brain's thought q size is %d\n", brain->_thoughtQueue.size());
	//fprintf(stderr, "i can see\n");
	for (int i = 0; i < 4 * (BASE_SIGHT_RANGE+1) * (BASE_SIGHT_RANGE+1); i++) {
		sightMap[i] = 0;
	}
	//check in rings of sight around playable
	for (int i = 0; i < _host->fovRings.size(); i++) {
		std::vector<intpair> currRing = _host->fovRings.at(i);
		for (int j = 0; j < _host->fovRings.at(i).size(); j++) {
			intpair currPair = currRing.at(j);
			int x = currPair.x;
			int y = currPair.y;
			//fprintf(stderr, "%d  and  %d\n", x, y);
			int cx = x + _host->getX() / TILE_SIZE;
			int cy = y + _host->getY() / TILE_SIZE;
			int smx = (x + BASE_SIGHT_RANGE);
			int smy = (y + BASE_SIGHT_RANGE);
			if (cx >= 0 && cx < MAP_WIDTH && cy >= 0 && cy < MAP_HEIGHT) {
				sightMap[smx + smy * 2 * (BASE_SIGHT_RANGE+1)] = (*rmap)[cx + cy * MAP_WIDTH];
				if (sightMap[smx + smy * 2 * (BASE_SIGHT_RANGE + 1)] > (Playable*)1) {
					//_sensedEntities.emplace(_sensedEntities.end(), sightMap[smx + smy * 2 * (BASE_SIGHT_RANGE + 1)]);
				}
				//fprintf(stderr, "x: %d y: %d xlim: %d ylim: %d\n", (x + BASE_SIGHT_RANGE), (y + BASE_SIGHT_RANGE), 2 * (BASE_SIGHT_RANGE + 1), 2 * (BASE_SIGHT_RANGE + 1));
			}
			else {
				sightMap[smx + smy * 2 * (BASE_SIGHT_RANGE + 1)] = (Playable*)1;
			}
			lemX = _host->getX() / TILE_SIZE;
			lemY = _host->getY() / TILE_SIZE;
			int lX = lemX + x;
			int lY = lemY + y;
			if (lX >= 0 && lX < LMAP_W) {
				if (lY >= 0 && lY < LMAP_H) {
					lemoryMap[lX + lY * LMAP_W] = sightMap[smx + smy * 2 * (BASE_SIGHT_RANGE + 1)];
				}
			}
		}
	}
	//fprintf(stderr, "brain's thought q size is %d\n", brain->_thoughtQueue.size());
}
Brain::Brain(Playable* target) {
	//srand(time(0));

	lemoryMap = new Playable*[LMAP_W * LMAP_H];
	sightMap = new Playable*[4 * (BASE_SIGHT_RANGE + 1) * (BASE_SIGHT_RANGE + 1)];

	_host = target;
	pathIndex = 0;
	for (int i = 0; i < LMAP_H * LMAP_W; i++) {
		//sightMap[i] = 0;
		lemoryMap[i] = 0;
		memoryMap[i] = 0;
	}
	social = distr(generator) % 100;
	randCoeff = distr(generator) / 1000000.0;
	_currPath = NULL;
	_thoughtQueue = new std::deque<order_t*>();
	
	memGraph = new Graph(&lemoryMap, LMAP_W, LMAP_H, _host);
	pf = new PathFinder(memGraph);
	//printLemory();
	
}
void Brain::registerEntity(Playable* entity) {
	_sensedEntities.emplace(_sensedEntities.end(), entity);
}
/*
order_t* Brain::nextMove() {
	if (pathIndex >= _currPath->size()) {
		return NULL;
	}
	struct order_t* move = (*_currPath)[pathIndex];
	//pathIndex++;
	return move;
}
*/
move_t Brain::nextStep() {
	fprintf(stderr, "request next step\n");
	move_t step;
	if (_currPath != NULL && !_currPath->empty()) {
		step = _currPath->front();
		_currPath->pop_front();
		return step;
	}
	else {
		return (move_t) 0;
	}
	//return (move_t) 0;
}

std::deque<move_t>* Brain::pathFind(int x, int y) {
	x = (int)round((float)x / TILE_SIZE);
	y = (int)round((float)y / TILE_SIZE);
	int sx = _host->getX() / TILE_SIZE;
	int sy = _host->getY() / TILE_SIZE;
	
	return pf->findPath(intpair(sx, sy), intpair(x, y));
	/*std::deque<move_t>* path = new std::deque<move_t>;


	int sx = _host->getX();
	int sy = _host->getY();
	//float angle = atan2(y - sy, x - sx);
	
	//float xs = cos(angle);
	//float ys = sin(angle);
	
	int dx = x - sx;
	int dy = y - sy;
	if (dx == 0 && dy == 0) {
		return path;
	}
	
	float error = 0.0;
	move_t move = (move_t)0;

	//bresenham's line algorithm

	if (abs(dx) > abs(dy)) {
		float de = abs(dy / dx);
		int ady = 0;
		if (dy != 0) {
			ady = -1 * dy / abs(dy);
		}
		int inc = (x - sx) / abs(x - sx);
		for (int i = sx; i != x; i += inc) {
			error += de;
			if (error < 0.5) {
				move = (move_t)(inc * 4);
			}
			else {
				move = (move_t)(inc * 4 + ady);
				error -= 1.0;
			}
			path->push_back(move);
		}
	}
	else {
		float de = abs(dx / dy);
		int adx = 0;
		if (dx != 0) {
			adx = dx / abs(dx);
		}
		int inc = (y - sy) / abs(y - sy);
		for (int i = sy; i != y; i += inc) {
			error += de;
			if (error < 0.5) {
				move = (move_t)(-1 * inc);
			}
			else {
				move = (move_t)(-1 * inc + 4 * adx);
				error -= 1.0;
			}
			path->push_back(move);
		}
	}
	
	
	
	
	/*int pathLen = (int)(sqrt((y - sy) * (y - sy) + (x - sx) * (x - sx)));
	float xs = (x - sx) / pathLen;
	float ys = (y - sy) / pathLen;

	for (int i = 0; i < pathLen; i++) {
		cx += xs;
		cy += ys;
		if ((int) cx != lx || (int) cy != ly) {
			int hor = (int) cx - lx;
			int ver = -((int) cy - ly);
			lx = (int)cx;
			ly = (int)cy;
			move = (move_t)(ver + 4 * hor);
			path->emplace(path->end(), move);
		}
		
	}*/

	
}

void Brain::queueThought(struct order_t* thought) {
	//fprintf(stderr, "queueing thought\n");
	_thoughtQueue->push_back(thought);
}
void Brain::interruptThought(struct order_t* thought) {
	//fprintf(stderr, "interrupting thought\n");
	_thoughtQueue->push_front(thought);
}

void Brain::emptyRegistry() {
	//fprintf(stderr, "thought queue size is %d\n", _thoughtQueue.size());
	std::vector<Playable*>().swap(_sensedEntities);
	//fprintf(stderr, "thought queue size is %d\n", _thoughtQueue.size());
}
int Brain::getSocial() {
	return social;
}
void Brain::say(Playable* target) {
	float distSq = (_host->getSprite()->getPosition().x - target->getSprite()->getPosition().x) * (_host->getSprite()->getPosition().x - target->getSprite()->getPosition().x) + (_host->getSprite()->getPosition().y - target->getSprite()->getPosition().y) * (_host->getSprite()->getPosition().y - target->getSprite()->getPosition().y);
	if (distSq <= BASE_HEAR_DIST) {
		if (target->getSocial() < social) {
			struct order_t* thought = new struct order_t;
			thought->type = order_type_t::FOLLOW;
			thought->target = _host;
			//fprintf(stderr, "i am %d\n", _host);
			target->interruptThought(thought);
		}
	}
}

void Brain::meander(int* goalX, int* goalY) {
	float goalAngle = (float)(distr(generator)) / 1000000.0 * 2 * 3.14159;
	int dist = distr(generator) % 250 + 50;

	if ((float)(distr(generator)) / 1000000.0 > 0.5) {
		dist *= -1;
	}
	*goalX = _host->getSprite()->getPosition().x + dist * cos(goalAngle);
	dist = distr(generator) % 250 + 50;
	if ((float)(distr(generator)) / 1000000.0 > 0.5) {
		dist *= -1;
	}
	*goalY = _host->getSprite()->getPosition().y + dist * sin(goalAngle);
	if (*goalX < 1)
		*goalX = 1;
	if (*goalY < 1)
		*goalY = 1;
	if (*goalX >= WINDOW_WIDTH)
		*goalX = WINDOW_WIDTH - 1;
	if (*goalY >= WINDOW_HEIGHT)
		*goalY = WINDOW_HEIGHT - 1;
	*goalX = round(*goalX / TILE_SIZE) * TILE_SIZE;
	*goalY = round(*goalY / TILE_SIZE) * TILE_SIZE;
}

void Brain::printLemory() {
	for (int i = 0; i < LMAP_H; i++) {
		for (int j = 0; j < LMAP_W; j++) {
			fprintf(stderr, "%d ", lemoryMap[j + i * LMAP_W]);
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n\n\n");
}
void Brain::think() {
	//fprintf(stderr, "default-behavior\n");
	//fprintf(stderr, "thought queue size is %d\n", _thoughtQueue.size());
	if(_currPath != NULL)
		fprintf(stderr, "path size is currently %d\n", _currPath->size());
	if (_thoughtQueue->size() == 0) {
		//fprintf(stderr, "default behavior\n");
		struct order_t* defaultBehaviour = new struct order_t;
		Playable* defaultTarget = NULL;
		if (_sensedEntities.size() > 0) {
			std::shuffle(_sensedEntities.begin(), _sensedEntities.end(), generator);

			
			for (int i = 0; i < _sensedEntities.size(); i++) {
				if (_sensedEntities.at(i)->getSocial() < social) {
					defaultTarget = _sensedEntities.at(i);
					break;
				}
			}
		}
		defaultBehaviour->target = defaultTarget;
		if (defaultTarget != NULL) {
			defaultBehaviour->type = order_type_t::SAY;
		}
		else {
			defaultBehaviour->type = order_type_t::MOVE;
			int goalX = 0;
			int goalY = 0;
			meander(&goalX, &goalY);
			defaultBehaviour->x = goalX;
			defaultBehaviour->y = goalY;
		}
		//fprintf(stderr, "%d\n", defaultBehaviour->type);
		queueThought(defaultBehaviour);
	}

	//fprintf(stderr, "i am %d next thought\n", _host);
	//fprintf(stderr, "thought queue size is %d\n", _thoughtQueue.size());
	struct order_t* nextThought = _thoughtQueue->front();
	//fprintf(stderr, "thought queue size is %d\n", _thoughtQueue.size());
	float distSq = 0;
	if (nextThought->target != NULL) {
		//fprintf(stderr, "non-NULL target\n");
		distSq = (nextThought->target->getX() - _host->getX()) * (nextThought->target->getX() - _host->getX()) + (nextThought->target->getY() - _host->getY()) * (nextThought->target->getY() - _host->getY());
	}
	else {
		//fprintf(stderr, "NULL target\n");
		//fprintf(stderr, "them: %d %d me: %d %d\n", nextThought->x, nextThought->y, _host->getX(), _host->getY());
		distSq = (nextThought->x - _host->getX()) * (nextThought->x - _host->getX()) + (nextThought->y - _host->getY()) * (nextThought->y - _host->getY());
	}
	//fprintf(stderr, "retrieved next thought\n");
	switch (nextThought->type) {
	case order_type_t::FOLLOW:
		fprintf(stderr, "follow\n");
		social = nextThought->target->getSocial();
		if (distSq > BASE_FOLLOW_DIST * (1 + 100 * randCoeff)) {
			if (_currPath != NULL) {
				std::deque<move_t>().swap(*_currPath);
				//_currPath->clear();
				//_currPath->shrink_to_fit();
				//delete _currPath;
			}
			//fprintf(stderr, "pathfind\n");
			//fprintf(stderr, "%d is target, i am %d\n", nextThought->target, _host);
			//fprintf(stderr, "%d\n", nextThought->target->getSprite());
			_currPath = pathFind(nextThought->target->getSprite()->getPosition().x, nextThought->target->getSprite()->getPosition().y);
		
			//fprintf(stderr, "end pathfind\n");
			pathIndex = 0;
		}
		
		//fprintf(stderr, "end follow\n");
		break;
	case order_type_t::MOVE:
		fprintf(stderr, "move\n");
		/*if (_currPath != NULL) {
			std::deque<move_t>().swap(*_currPath);
			//_currPath->clear();
			//_currPath->shrink_to_fit();
			//delete _currPath;
		}*/
		if (_currPath == NULL || _currPath->empty()) {
			fprintf(stderr, "path empty - recalculate\n");
			_currPath = pathFind(nextThought->x, nextThought->y);
		}
		//fprintf(stderr, "distSq %f\n", distSq);
		if (distSq <= BASE_NE_DIST) {
			
			//printLemory();
			if(_currPath != NULL)
				_currPath->clear();
			fprintf(stderr, "move again\n");
			_thoughtQueue->pop_front();
			struct order_t* newThought = new order_t;
			newThought->type = order_type_t::MOVE;
			//_host->clipToGrid();
			int goalX = 0;
			int goalY = 0;
			meander(&goalX, &goalY);
			newThought->target = NULL;
			newThought->x = goalX;
			newThought->y = goalY;
			queueThought(newThought);
			//fprintf(stderr, "end move\n");
		}
		//fprintf(stderr, "end move\n");
		break;
	case order_type_t::SAY:
		fprintf(stderr, "say\n");
		if (_currPath != NULL) {
			std::deque<move_t>().swap(*_currPath);
			//_currPath->clear();
			//_currPath->shrink_to_fit();
			//delete _currPath;
		}
		//fprintf(stderr, "path find\n");
		_currPath = pathFind(nextThought->target->getX(), nextThought->target->getY());
		if (distSq < BASE_HEAR_DIST) {
			//fprintf(stderr, "do say\n");
			say(nextThought->target);
			//fprintf(stderr, "did say\n");
			_thoughtQueue->pop_front();
			
			struct order_t* newThought = new struct order_t;

			std::shuffle(_sensedEntities.begin(), _sensedEntities.end(), generator);
			Playable* target = NULL;
			for (int i = 0; i < _sensedEntities.size(); i++) {
				if (_sensedEntities.at(i)->getSocial() < social) {
					target = _sensedEntities.at(i);
					break;
				}
			}
			newThought->target = target;
			if (target != NULL) {
				newThought->type = order_type_t::SAY;
			}
			else {
				newThought->type = order_type_t::MOVE;
				int goalX = 0;
				int goalY = 0;
				meander(&goalX, &goalY);
				newThought->x = goalX;
				newThought->y = goalY;
			}
			queueThought(newThought);
		}
		//fprintf(stderr, "end say\n");
		break;
	}
	/*float goalAngle = (float)(distr(generator)) / 1000000.0 * 2 * 3.14159;
	int dist = distr(generator) % 250 + 50;
	
	if ((float)(distr(generator)) / 1000000.0 > 0.5) {
		dist *= -1;
	}
	int goalX = _host->getSprite()->getPosition().x + dist * cos(goalAngle);
	dist = distr(generator) % 250 + 50;
	if ((float)(distr(generator)) / 1000000.0 > 0.5) {
		dist *= -1;
	}
	int goalY = _host->getSprite()->getPosition().y + dist * sin(goalAngle);
	if (goalX < 1)
		goalX = 1;
	if (goalY < 1)
		goalY = 1;
	if (goalX >= WINDOW_WIDTH)
		goalX = WINDOW_WIDTH - 1;
	if (goalY >= WINDOW_HEIGHT)
		goalY = WINDOW_HEIGHT - 1;
	_currPath = pathFind(goalX, goalY, NULL);
	pathIndex = 0;*/
	//_thoughtQueue.pop_front();
}