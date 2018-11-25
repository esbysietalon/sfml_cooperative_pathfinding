#include "stdafx.h"
#include "brain.h"

std::random_device                  rand_dev;
std::mt19937                        generator(rand_dev());
std::uniform_int_distribution<int>  distr(0, 1000000);

Brain::Brain() {

}
Brain::~Brain() {

}
Brain::Brain(Playable* target) {
	//srand(time(0));
	_host = target;
	pathIndex = 0;
	for (int i = 0; i < LMAP_H * LMAP_W; i++) {
		localMap[i] = 0;
	}
	social = distr(generator) % 100;
	randCoeff = distr(generator) / 1000000.0;
	fprintf(stderr, "%d\n", social);
}
void Brain::registerEntity(Playable* entity) {
	_sensedEntities.emplace(_sensedEntities.end(), entity);
}
order_t* Brain::nextMove() {
	if (pathIndex >= _currPath->size()) {
		return NULL;
	}
	struct order_t* move = (*_currPath)[pathIndex];
	//pathIndex++;
	return move;
}
std::vector<order_t*>* Brain::pathFind(int x, int y) {
	std::vector<order_t*>* path = new std::vector<order_t*>;

	int lx = _host->getSprite()->getPosition().x + 0;
	int ly = _host->getSprite()->getPosition().y + 0;
	float lAngle = atan2(y - ly, x - lx);
	float xs = cos(lAngle);
	float ys = sin(lAngle);
	float ix = lx;
	float iy = ly;
	float distSq = (ix - x) * (ix - x) + (iy - y) * (iy - y);
	while (distSq > 2) {
		ix += xs;
		iy += ys;
		distSq = (ix - x) * (ix - x) + (iy - y) * (iy - y);
		if (localMap[((int)(ix) / 16) + (int)(iy)* LMAP_W / 16] != 0) {
			fprintf(stderr, "obstacle in way\n");
		}
	}
	struct order_t* move = new struct order_t;
	move->type = order_type_t::MOVE;
	move->x = x;
	move->y = y;
	move->target = NULL;
	path->emplace(path->end(), move);
	return path;
}

void Brain::queueThought(struct order_t* thought) {
	_thoughtQueue.push_back(thought);
}
void Brain::interruptThought(struct order_t* thought) {
	_thoughtQueue.push_front(thought);
}

void Brain::emptyRegistry() {
	_sensedEntities.clear();
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
}

void Brain::think() {
	//fprintf(stderr, "default-behavior\n");
	if (_thoughtQueue.size() == 0) {
		//fprintf(stderr, "default behavior\n");
		struct order_t* defaultBehaviour = new struct order_t;
		
		std::shuffle(_sensedEntities.begin(), _sensedEntities.end(), generator);
		Playable* defaultTarget = NULL;
		for (int i = 0; i < _sensedEntities.size(); i++) {
			if (_sensedEntities.at(i)->getSocial() < social) {
				defaultTarget = _sensedEntities.at(i);
				break;
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
		queueThought(defaultBehaviour);
	}

	//fprintf(stderr, "i am %d next thought\n", _host);
	struct order_t* nextThought = _thoughtQueue.front();
	float distSq = 0;
	if (nextThought->target != NULL) {
		//fprintf(stderr, "non-NULL target\n");
		distSq = (nextThought->target->getSprite()->getPosition().x - _host->getSprite()->getPosition().x) * (nextThought->target->getSprite()->getPosition().x - _host->getSprite()->getPosition().x) + (nextThought->target->getSprite()->getPosition().y - _host->getSprite()->getPosition().y) * (nextThought->target->getSprite()->getPosition().y - _host->getSprite()->getPosition().y);
	}
	else {
		//fprintf(stderr, "NULL target\n");
		distSq = (nextThought->x - _host->getSprite()->getPosition().x) * (nextThought->x - _host->getSprite()->getPosition().x) + (nextThought->y - _host->getSprite()->getPosition().y) * (nextThought->y - _host->getSprite()->getPosition().y);
	}
	//fprintf(stderr, "retrieved next thought\n");
	switch (nextThought->type) {
	case order_type_t::FOLLOW:
		//fprintf(stderr, "follow\n");
		social = nextThought->target->getSocial();
		if (distSq > BASE_FOLLOW_DIST * (1 + 100 * randCoeff)) {
			if (_currPath != NULL) {
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
		//fprintf(stderr, "move\n");
		if (_currPath != NULL) {
			//delete _currPath;
		}
		_currPath = pathFind(nextThought->x, nextThought->y);
		if (distSq < BASE_NE_DIST) {
			_thoughtQueue.pop_front();
			struct order_t* newThought = new order_t;
			newThought->type = order_type_t::MOVE;
			int goalX = 0;
			int goalY = 0;
			meander(&goalX, &goalY);
			newThought->target = NULL;
			newThought->x = goalX;
			newThought->y = goalY;
			queueThought(newThought);
		}
		//fprintf(stderr, "end move\n");
		break;
	case order_type_t::SAY:
		//fprintf(stderr, "say\n");
		if (_currPath != NULL) {
			//delete _currPath;
		}
		//fprintf(stderr, "path find\n");
		_currPath = pathFind(nextThought->target->getSprite()->getPosition().x, nextThought->target->getSprite()->getPosition().y);
		if (distSq < BASE_HEAR_DIST) {
			//fprintf(stderr, "do say\n");
			say(nextThought->target);
			//fprintf(stderr, "did say\n");
			_thoughtQueue.pop_front();
			
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