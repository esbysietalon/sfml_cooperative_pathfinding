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

bool Brain::uniqueSight(Playable* sight) {
	for (int i = 0; i < _sensedEntities.size(); i++) {
		if (_sensedEntities.at(i) == sight)
			return false;
	}
	return true;
}

void Brain::see() {
	
	//implement cfov
	//reset sightmaps
	//fprintf(stderr, "brain's thought q size is %d\n", brain->_thoughtQueue.size());
	//fprintf(stderr, "%d can see\n", social);
	for (int i = 0; i < 4 * (BASE_SIGHT_RANGE+1) * (BASE_SIGHT_RANGE+1); i++) {
		sightMap[i] = 0;
	}
	//check in rings of sight around playable
	for (int i = 0; i < _host->fovRings.size(); i++) {
		//fprintf(stderr, "ring %d:\n", i);
		std::vector<intpair> currRing = _host->fovRings.at(i);
		for (int j = 0; j < _host->fovRings.at(i).size(); j++) {
			intpair currPair = currRing.at(j);
			int x = currPair.x;
			int y = currPair.y;
			//fprintf(stderr, "(%d,%d)\n", x, y);
			int cx = x + _host->getX() / TILE_SIZE;
			int cy = y + _host->getY() / TILE_SIZE;
			int smx = (x + BASE_SIGHT_RANGE);
			int smy = (y + BASE_SIGHT_RANGE);
			
			if (cx >= 0 && cx < MAP_WIDTH && cy >= 0 && cy < MAP_HEIGHT) {
				//if((*rmap)[cx + cy * MAP_WIDTH] != 0 && (*rmap)[cx + cy * MAP_WIDTH] != _host)
					
				sightMap[smx + smy * 2 * (BASE_SIGHT_RANGE+1)] = (*rmap)[cx + cy * MAP_WIDTH];
				
				if (sightMap[smx + smy * 2 * (BASE_SIGHT_RANGE + 1)] > (Playable*)RESERVED_RMAP_NUMBERS) {
					if (uniqueSight(sightMap[smx + smy * 2 * (BASE_SIGHT_RANGE + 1)]) && sightMap[smx + smy * 2 * (BASE_SIGHT_RANGE + 1)] != _host) {
						//fprintf(stderr, "and i see %d\n", (*rmap)[cx + cy * MAP_WIDTH]->getSocial());
						_sensedEntities.emplace(_sensedEntities.end(), sightMap[smx + smy * 2 * (BASE_SIGHT_RANGE + 1)]);
					}
				}
				else {
					//sightMap[smx + smy * 2 * (BASE_SIGHT_RANGE + 1)] = (Playable*) 2;
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
	//fprintf(stderr, "i saw %d things\n", _sensedEntities.size());
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
	social = distr(generator) % 100 + 1;
	originalSoc = social;
	randCoeff = distr(generator) / 1000000.0;
	_currPath = NULL;
	_thoughtQueue = new std::deque<order_t*>();
	
	memGraph = new Graph(&lemoryMap, LMAP_W, LMAP_H, _host);
	//pf = new PathFinder(memGraph);
	pfp = new PathFinderPlus(memGraph, INFLATION_FACTOR);
	pfp->setBrain(this);
	//printLemory();
	
}

void Brain::setSocial(int soc) {
	//fprintf(stderr, "\n%d-%d\n", social, soc);
	social = soc;
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
	//fprintf(stderr, "request next step\n");
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

std::deque<move_t>* Brain::pathLook(int x, int y) {
	x = (int)round((float)x / TILE_SIZE);
	y = (int)round((float)y / TILE_SIZE);
	intpair s = _host->moveGoals();
	//fprintf(stderr, "path look is from (%d,%d) to (%d,%d); request from %d to %d\n", s.x / TILE_SIZE, s.y / TILE_SIZE, x, y, this, pf);
	pfp->planMore();
	//fprintf(stderr, "planned more; now looking\n");
	return pfp->findPath(intpair(s.x / TILE_SIZE, s.y / TILE_SIZE), intpair(x, y));
}
std::deque<move_t>* Brain::pathFind(int x, int y) {
	x = (int)round((float)x / TILE_SIZE);
	y = (int)round((float)y / TILE_SIZE);
	intpair s = _host->moveGoals();
	//fprintf(stderr, "path find is from (%d,%d) to (%d,%d)\n", s.x / TILE_SIZE, s.y / TILE_SIZE, x, y);
	pfp->replanPath(intpair(s.x / TILE_SIZE, s.y / TILE_SIZE), intpair(x, y));
	return pfp->findPath(intpair(s.x / TILE_SIZE, s.y / TILE_SIZE), intpair(x, y));
	//return pf->findPath(intpair(sx, sy), intpair(x, y));
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
	//fprintf(stderr, "\nsay call\n");
	float distSq = (_host->getSprite()->getPosition().x - target->getSprite()->getPosition().x) * (_host->getSprite()->getPosition().x - target->getSprite()->getPosition().x) + (_host->getSprite()->getPosition().y - target->getSprite()->getPosition().y) * (_host->getSprite()->getPosition().y - target->getSprite()->getPosition().y);
	//if (distSq <= BASE_HEAR_DIST) {
		if (target->getSocial() < social && target->getSocial() != 0) {
			target->setSocial(0);
			struct order_t* thought = new struct order_t;
			thought->type = order_type_t::FOLLOW;
			thought->target = _host;
			//fprintf(stderr, "i am %d\n", _host);
			target->interruptThought(thought);
		}
	//}
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
			int unit = (int)lemoryMap[j + i * LMAP_W];
			if (unit > RESERVED_RMAP_NUMBERS && lemoryMap[j + i * LMAP_W] != _host) {
				fprintf(stderr, "%d ", unit % 10);
			}
			else if (lemoryMap[j + i * LMAP_W] == _host) {
				fprintf(stderr, "m ");
			}
			else if (lemoryMap[j + i * LMAP_W] == (Playable*)2) {
				fprintf(stderr, "* ");
			}
			else {
				fprintf(stderr, "- ");
			}
			
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n\n\n");
}
void Brain::think() {
	//fprintf(stderr, "default-behavior\n");
	//fprintf(stderr, "thought queue size is %d\n", _thoughtQueue->size());
	
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
		//fprintf(stderr, "non-NULL target; target is %d\n", nextThought->target);
		distSq = (nextThought->target->getX() - _host->getX()) * (nextThought->target->getX() - _host->getX()) + (nextThought->target->getY() - _host->getY()) * (nextThought->target->getY() - _host->getY());
	}
	else {
		//fprintf(stderr, "NULL target\n");
		//fprintf(stderr, "them: %d %d me: %d %d\n", nextThought->x, nextThought->y, _host->getX(), _host->getY());
		distSq = (nextThought->x - _host->getX()) * (nextThought->x - _host->getX()) + (nextThought->y - _host->getY()) * (nextThought->y - _host->getY());
	}
	//fprintf(stderr, "retrieved next thought\n");
	struct order_t* newSayThought = new struct order_t;
	Playable* newSayTarget = NULL;
	intpair maintain = _host->moveGoals();

	switch (nextThought->type) {
	case order_type_t::FOLLOW:
		//fprintf(stderr, "follow\n");
		social = 0;
		//if (nextThought->target->getSocial() == 0) {
			//social = originalSoc;
			//_thoughtQueue->clear();
			//break;
		//}
		if (_currPath != NULL)
			std::deque<move_t>().swap(*_currPath);
		
		_currPath = pathLook(maintain.x, maintain.y);
		if (distSq > BASE_FOLLOW_DIST * (1 + 100 * randCoeff)) {
			/*if (_currPath != NULL) {
				std::deque<move_t>().swap(*_currPath);
				//_currPath->clear();
				//_currPath->shrink_to_fit();
				//delete _currPath;
			}*/
			//fprintf(stderr, "pathfind\n");
			//fprintf(stderr, "%d is target, i am %d\n", nextThought->target, _host);
			//fprintf(stderr, "%d\n", nextThought->target->getSprite());
			
			if (_currPath == NULL || _currPath->empty()) {
				//fprintf(stderr, "find call\n");
				_currPath = pathFind(nextThought->target->getX(), nextThought->target->getY());
			}
			else {
				std::deque<move_t>().swap(*_currPath);
				_currPath = pathLook(nextThought->target->getX(), nextThought->target->getY());
			}
			//else {
				//fprintf(stderr, "look call\n");
				
			//}
			//fprintf(stderr, "end pathfind\n");
			//pathIndex = 0;
		}
		
		//fprintf(stderr, "end follow\n");
		break;
	case order_type_t::MOVE:
		//fprintf(stderr, "move\n");
		/*if (_currPath != NULL) {
			std::deque<move_t>().swap(*_currPath);
			//_currPath->clear();
			//_currPath->shrink_to_fit();
			//delete _currPath;
		}*/
		
		if (_currPath == NULL || _currPath->empty()) {
			//fprintf(stderr, "path empty - recalculate\n");
			_currPath = pathFind(nextThought->x, nextThought->y);
		}
		else {
			std::deque<move_t>().swap(*_currPath);
			_currPath = pathLook(nextThought->x, nextThought->y);
		}//else {
			//fprintf(stderr, "path look\n");
			
		//}
		//fprintf(stderr, "distSq %f\n", distSq);
		//fprintf(stderr, "_sensedEntities(%d)", _sensedEntities.size());
		if (_sensedEntities.size() > 0) {
			
			std::shuffle(_sensedEntities.begin(), _sensedEntities.end(), generator);

			for (int i = 0; i < _sensedEntities.size(); i++) {
				if (_sensedEntities.at(i)->getSocial() < social && _sensedEntities.at(i)->getSocial() != 0) {
					//fprintf(stderr, "%d: new say candidate\n", _sensedEntities.at(i)->getSocial());
					newSayTarget = _sensedEntities.at(i);
					break;
				}
			}
			newSayThought->target = newSayTarget;
			if (newSayTarget != NULL) {
				//fprintf(stderr, "\n%d found new say target: %d\n", social, newSayThought->target->getSocial());
				newSayThought->type = order_type_t::SAY;
				_thoughtQueue->pop_front();
				interruptThought(newSayThought);
				break;
			}
		}
		if (distSq <= BASE_NE_DIST || _currPath == NULL || _currPath->empty()) {
			
			//printLemory();
			if(_currPath != NULL)
				_currPath->clear();
			//fprintf(stderr, "move again\n");
			_thoughtQueue->pop_front();
			struct order_t* newThought = new struct order_t;

			
			newThought->target = NULL;
			newThought->type = order_type_t::MOVE;
			int goalX = 0;
			int goalY = 0;
			meander(&goalX, &goalY);
			newThought->x = goalX;
			newThought->y = goalY;
			
			queueThought(newThought);
			
			
			//fprintf(stderr, "end move\n");
		}
		//fprintf(stderr, "end move\n");
		break;
	case order_type_t::SAY:
		//fprintf(stderr, "say\n");
		
		
		
		
		if (_currPath == NULL || _currPath->empty()) {
			_currPath = pathFind(nextThought->target->getX(), nextThought->target->getY());
		}
		else {
			std::deque<move_t>().swap(*_currPath);
			_currPath = pathLook(nextThought->target->getX(), nextThought->target->getY());
		}		//else {
			
		//}
		
		if (distSq <= BASE_HEAR_DIST) {
			//fprintf(stderr, "do say\n");
			say(nextThought->target);
			//fprintf(stderr, "did say\n");
			_thoughtQueue->pop_front();
			
			struct order_t* newThought = new struct order_t;

			std::shuffle(_sensedEntities.begin(), _sensedEntities.end(), generator);
			Playable* target = NULL;
			for (int i = 0; i < _sensedEntities.size(); i++) {
				if (_sensedEntities.at(i)->getSocial() < social && _sensedEntities.at(i)->getSocial() != 0) {
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
