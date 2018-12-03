#include "stdafx.h"
#include "playable.h"
#include "globals.h"

Playable::Playable() {

}

Playable::Playable(sf::Texture* texture, int w, int h, int x, int y) {
	currSprite = new AnimSprite(texture, w, h, 0, 0, 0);
	currSprite->getSprite()->setPosition(x, y);
	_texture = texture;
	_width = w;
	_height = h;
	_x = x;
	_y = y;
	moveGoalX = x;
	moveGoalY = y;
	setFlagState({ animation_t::IDLE });
	for (int i = 0; i < MAX_ANIMS; i++) {
		animations[i] = NULL;
	}
	animations[getFlagState({ animation_t::IDLE })] = currSprite;
	brain = new Brain(this);
	isPlayer = false;
	delta_orientation = 2 * asin((float)TILE_SIZE / (2 * BASE_SIGHT_RANGE));
	currStep = move_t::NONE;
	//fprintf(stderr, "%f\n", delta_orientation);
}

Playable::~Playable()
{
}

bool Playable::isControlled()
{
	return isPlayer;
}

void Playable::setControl(bool control)
{
	isPlayer = control;
}

int intpow(int base, int exponent) {
	int out = 1;
	while (exponent > 0) {
		exponent--;
		out *= base;
	}
	return out;
}

void Playable::see() {
	float currLineAngle;
	float startX = _x + SPRITE_SIZE / 2;
	float startY = _y + SPRITE_SIZE / 2;
	for (currLineAngle = orientation - BASE_LOS_CONE; currLineAngle <= orientation + BASE_LOS_CONE; currLineAngle += delta_orientation) {
		float endX = _x + SPRITE_SIZE / 2 + BASE_SIGHT_RANGE * cos(currLineAngle);
		float endY = _y + SPRITE_SIZE / 2 + BASE_SIGHT_RANGE * sin(currLineAngle);
		// bresenhamCollision(_x + SPRITE_SIZE / 2, _y + SPRITE_SIZE / 2, endX, endY);
		int roundX = -1;
		int roundY = -1;
		int lastX = -1;
		int lastY = -1;
		for (int i = 0; i < BASE_SIGHT_RANGE; i++) {
			float currX = startX + i * (endX - startX) / BASE_SIGHT_RANGE;
			float currY = startY + i * (endY - startY) / BASE_SIGHT_RANGE;
			roundX = (int)(currX) / TILE_SIZE * TILE_SIZE;
			roundY = (int)(currY) / TILE_SIZE * TILE_SIZE;
			
			if (roundX != lastX && roundY != lastY) {

			}
		}
	}
}

void Playable::addAnimation(int sx, int sy, int len, animation_t flagSet[]) {
	AnimSprite* sprite = new AnimSprite(_texture, _width, _height, sx, sy, len);
	animations[getFlagState(flagSet)] = sprite;
}
void Playable::addAnimation(int sx, int sy, int len, std::initializer_list<animation_t> flagSet) {
	AnimSprite* sprite = new AnimSprite(_texture, _width, _height, sx, sy, len);
	animations[getFlagState(flagSet)] = sprite;
}

int Playable::getFlagState(animation_t flagSet[])
{
	int out = 0;
	for (int i = 0; i < animation_t::NUM_FLAGS; i++) {
		if (flagSet[i] > -1) {
			out += intpow(2, flagSet[i]);
		}
	}
	return out;
}
int Playable::getFlagState(std::initializer_list<animation_t> list) {
	int out = 0;
	for (int i = 0; i < list.size(); i++) {
		if (list.begin()[i] > animation_t::INVALID) {
			out += intpow(2, list.begin()[i]);
		}
	}
	//fprintf(stderr, "%d - list\n", out);
	return out;
}

void Playable::setFlagState(std::initializer_list<animation_t> list) {
	for (int i = 0; i < animation_t::NUM_FLAGS; i++) {
		_flagSet[i] = animation_t::INVALID;
	}
	for (int i = 0; i < list.size(); i++) {
		if (list.begin()[i] > animation_t::INVALID) {
			_flagSet[list.begin()[i]] = list.begin()[i];
		}
	}
}
bool Playable::areFlagsSet(std::initializer_list<animation_t> list)
{
	for(int i = 0; i < list.size(); i++){
		if(list.begin()[i] > animation_t::INVALID){
			if(_flagSet[list.begin()[i]] <= animation_t::INVALID){
				return false;
			}
		}
	}
	return true;
}
void Playable::addFlags(std::initializer_list<animation_t> list) {
	for (int i = 0; i < list.size(); i++) {
if (list.begin()[i] > animation_t::INVALID) {
	_flagSet[list.begin()[i]] = list.begin()[i];
}
	}
}
void Playable::removeFlags(std::initializer_list<animation_t> list) {
	for (int i = 0; i < list.size(); i++) {
		if (list.begin()[i] > animation_t::INVALID) {
			_flagSet[list.begin()[i]] = animation_t::INVALID;
		}
	}
}
void Playable::manageAnimations() {
	int flagState = getFlagState(_flagSet);
	//fprintf(stderr, "%d\n", flagState);
	if (flagState > 0) {
		if (animations[flagState] != NULL) {
			if (prevFlagState != flagState) {
				currSprite->resetFrame();
				currSprite = animations[getFlagState(_flagSet)];
				prevFlagState = flagState;
			}
			//fprintf(stderr, " non-null animation access \n");
		}
		else {
			currSprite = animations[getFlagState({ animation_t::IDLE })];
		}
	}
	else {
		currSprite = animations[getFlagState({ animation_t::IDLE })];
	}

}
sf::Sprite* Playable::getSprite()
{
	return currSprite->getSprite();
}

void Playable::update() {
	//CONTROLLING (MOVING) AND ANIMATING PLAYER SPRITE
	//fprintf(stderr, "player_update\n");
	if (currStep != move_t::NONE) {
		distSq = (moveGoalX - _x) * (moveGoalX - _x) + (moveGoalY - _y) * (moveGoalY - _y);
		if (distSq < 3) {
			currStep = move_t::NONE;
			//fprintf(stderr, "%f, %f\n", _x, _y);
			_x = round(_x / TILE_SIZE) * TILE_SIZE;
			_y = round(_y / TILE_SIZE) * TILE_SIZE;
			//fprintf(stderr, "%f, %f\n", _x, _y);
		}
		if ((int)_x == (int)moveGoalX && (int)_y == (int)moveGoalY) {
			currStep = move_t::NONE;
		}
	}
	if (currStep == move_t::NONE || bufferFrames > 0) {
		_x = round(_x / TILE_SIZE) * TILE_SIZE;
		_y = round(_y / TILE_SIZE) * TILE_SIZE;
		if (bufferFrames > 0) {
			bufferFrames--;
		}
		if (currStep == move_t::NONE) {
			bufferFrames = 1;
		}
		int ver = 0, hor = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			ver++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			hor--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			ver--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			hor++;
		}
		currStep = (move_t)(ver + hor * 4);
		
		moveGoalX = _x + hor * TILE_SIZE;
		moveGoalY = _y - ver * TILE_SIZE;
		//fprintf(stderr, "%d %d\n", hor, ver);
		//fprintf(stderr, "%d %f %f\n", currStep, moveGoalX, moveGoalY);
	}
	
	if (areFlagsSet({ animation_t::MOVE_RIGHT })) {
		addFlags({ animation_t::FACE_RIGHT });
	}
	if (areFlagsSet({ animation_t::MOVE_LEFT })) {
		addFlags({ animation_t::FACE_LEFT });
	}
	removeFlags({ animation_t::MOVE_LEFT, animation_t::MOVE_RIGHT });
	if (currStep == move_t::N) {
		_y-=SPEED;
		if (areFlagsSet({ animation_t::MOVE_RIGHT })) {
			addFlags({ animation_t::MOVE_RIGHT });
			removeFlags({ animation_t::MOVE_LEFT });
		}
		else if (areFlagsSet({ animation_t::MOVE_LEFT })) {
			addFlags({ animation_t::MOVE_LEFT });
			removeFlags({ animation_t::MOVE_RIGHT });
		}
		else {
			addFlags({ animation_t::MOVE_RIGHT });
		}
		removeFlags({ animation_t::IDLE, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });

	}
	if (currStep == move_t::W) {
		_x-= SPEED;
		addFlags({ animation_t::MOVE_LEFT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_RIGHT, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });
	}
	if (currStep == move_t::NW) {
		_y-= 0.707106781 * SPEED;
		_x-= 0.707106781 * SPEED;
		addFlags({ animation_t::MOVE_LEFT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_RIGHT, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });
	}
	if (currStep == move_t::SW) {
		_y+= 0.707106781 * SPEED;
		_x-= 0.707106781 * SPEED;
		addFlags({ animation_t::MOVE_LEFT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_RIGHT, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });
	}
	if (currStep == move_t::S) {
		_y+= SPEED;
		if (areFlagsSet({ animation_t::MOVE_RIGHT })) {
			addFlags({ animation_t::MOVE_RIGHT });
			removeFlags({ animation_t::MOVE_LEFT });
		}
		else if (areFlagsSet({ animation_t::MOVE_LEFT })) {
			addFlags({ animation_t::MOVE_LEFT });
			removeFlags({ animation_t::MOVE_RIGHT });
		}
		else {
			addFlags({ animation_t::MOVE_LEFT });
		}
		removeFlags({ animation_t::IDLE, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });
	}
	if (currStep == move_t::E) {
		_x+= SPEED;
		addFlags({ animation_t::MOVE_RIGHT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_LEFT, animation_t::FACE_RIGHT,animation_t::FACE_LEFT });
	}
	if (currStep == move_t::NE) {
		_y-= 0.707106781 * SPEED;
		_x+= 0.707106781 * SPEED;
		addFlags({ animation_t::MOVE_RIGHT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_LEFT, animation_t::FACE_RIGHT,animation_t::FACE_LEFT });
	}
	if (currStep == move_t::SE) {
		_y+= 0.707106781 * SPEED;
		_x+= 0.707106781 * SPEED;
		addFlags({ animation_t::MOVE_RIGHT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_LEFT, animation_t::FACE_RIGHT,animation_t::FACE_LEFT });
	}
	manageAnimations();
	currSprite->getSprite()->setPosition(_x, _y);
	currSprite->update();
}

int Playable::getSocial() {
	return brain->getSocial();
}

void Playable::queueThought(struct order_t* thought) {
	brain->queueThought(thought);
}

void Playable::interruptThought(struct order_t* thought) {
	brain->interruptThought(thought);
}

void Playable::registerEntity(Playable* playable) {
	brain->registerEntity(playable);
}

void Playable::emptyRegistry() {
	brain->emptyRegistry();
}

void Playable::clipToGrid() {
	_x = round(_x / TILE_SIZE) * TILE_SIZE;
	_y = round(_y / TILE_SIZE) * TILE_SIZE;
}
int Playable::getX() {
	return (int)round(_x / TILE_SIZE) * TILE_SIZE;
}
int Playable::getY() {
	return (int)round(_y / TILE_SIZE) * TILE_SIZE;
}
void Playable::update_AI()
{
	
	//THINKING - BRAIN ACTIVITY
	/*int randInt = rand();
	if (actionTimer.getElapsedTime().asSeconds() * 50 > randInt) {
		brain->think();
		currMove = brain->nextMove();
		goalAngle = atan2(getSprite()->getPosition().y - currMove->y, getSprite()->getPosition().x - currMove->x);
		xspeed = -cos(goalAngle);
		yspeed = -sin(goalAngle);
		actionTimer.restart();
	}*/
	//fprintf(stderr, "pre-think\n");
	brain->think();
	move_t nextStep = brain->nextStep();
	if (currStep == move_t::NONE) {
		currStep = nextStep;
		int hori = round((float)currStep / 4);
		int vert = currStep - hori * 4;
		moveGoalX = getX() + hori * TILE_SIZE;
		moveGoalY = getY() - vert * TILE_SIZE;
		//fprintf(stderr, "end move\n");
		//fprintf(stderr, "currX: %d, currY: %d, new moveGoalX: %d, new moveGoalY: %d, hori: %d, vert: %d\n", getX(), getY(), moveGoalX, moveGoalY, hori, vert);
	}
	if (currStep != move_t::NONE) {
		distSq = (moveGoalX - _x) * (moveGoalX - _x) + (moveGoalY - _y) * (moveGoalY - _y);
		//fprintf(stderr, "%d\n", distSq);
		if (distSq < 4) {
			currStep = move_t::NONE;
			clipToGrid();
		}
	}
	
	
	//fprintf(stderr, "%f %f %d %d %d\n", _x, _y, moveGoalX, moveGoalY, currStep);
	if (areFlagsSet({ animation_t::MOVE_RIGHT })) {
		addFlags({ animation_t::FACE_RIGHT });
	}
	if (areFlagsSet({ animation_t::MOVE_LEFT })) {
		addFlags({ animation_t::FACE_LEFT });
	}
	removeFlags({ animation_t::MOVE_LEFT, animation_t::MOVE_RIGHT });
	if (currStep == move_t::N) {
		_y -= SPEED;
		if (areFlagsSet({ animation_t::MOVE_RIGHT })) {
			addFlags({ animation_t::MOVE_RIGHT });
			removeFlags({ animation_t::MOVE_LEFT });
		}
		else if (areFlagsSet({ animation_t::MOVE_LEFT })) {
			addFlags({ animation_t::MOVE_LEFT });
			removeFlags({ animation_t::MOVE_RIGHT });
		}
		else {
			addFlags({ animation_t::MOVE_RIGHT });
		}
		removeFlags({ animation_t::IDLE, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });

	}
	if (currStep == move_t::W) {
		_x -= SPEED;
		addFlags({ animation_t::MOVE_LEFT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_RIGHT, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });
	}
	if (currStep == move_t::NW) {
		_y -= 0.707106781 * SPEED;
		_x -= 0.707106781 * SPEED;
		addFlags({ animation_t::MOVE_LEFT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_RIGHT, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });
	}
	if (currStep == move_t::SW) {
		_y += 0.707106781 * SPEED;
		_x -= 0.707106781 * SPEED;
		addFlags({ animation_t::MOVE_LEFT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_RIGHT, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });
	}
	if (currStep == move_t::S) {
		_y += SPEED;
		if (areFlagsSet({ animation_t::MOVE_RIGHT })) {
			addFlags({ animation_t::MOVE_RIGHT });
			removeFlags({ animation_t::MOVE_LEFT });
		}
		else if (areFlagsSet({ animation_t::MOVE_LEFT })) {
			addFlags({ animation_t::MOVE_LEFT });
			removeFlags({ animation_t::MOVE_RIGHT });
		}
		else {
			addFlags({ animation_t::MOVE_LEFT });
		}
		removeFlags({ animation_t::IDLE, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });
	}
	if (currStep == move_t::E) {
		_x += SPEED;
		addFlags({ animation_t::MOVE_RIGHT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_LEFT, animation_t::FACE_RIGHT,animation_t::FACE_LEFT });
	}
	if (currStep == move_t::NE) {
		_y -= 0.707106781 * SPEED;
		_x += 0.707106781 * SPEED;
		addFlags({ animation_t::MOVE_RIGHT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_LEFT, animation_t::FACE_RIGHT,animation_t::FACE_LEFT });
	}
	if (currStep == move_t::SE) {
		_y += 0.707106781 * SPEED;
		_x += 0.707106781 * SPEED;
		addFlags({ animation_t::MOVE_RIGHT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_LEFT, animation_t::FACE_RIGHT,animation_t::FACE_LEFT });
	}
	manageAnimations();
	currSprite->getSprite()->setPosition(_x, _y);
	currSprite->update();
	//fprintf(stderr, "post-think\n");
	//if (currMove == NULL) {
	/*currMove = brain->nextMove();
	//fprintf(stderr, "%d\n", currMove->type);
	if (currMove != NULL) {
		//fprintf(stderr, "x: %f, y: %f, goalX: %d, goalY: %d\n", getSprite()->getPosition().x, getSprite()->getPosition().y, currMove->x, currMove->y);
		goalAngle = atan2(getSprite()->getPosition().y - currMove->y, getSprite()->getPosition().x - currMove->x);
		xspeed = -cos(goalAngle);
		yspeed = -sin(goalAngle);
		float distSq = (currMove->x - getSprite()->getPosition().x) * (currMove->x - getSprite()->getPosition().x) + (currMove->y - getSprite()->getPosition().y) * (currMove->y - getSprite()->getPosition().y);
		if (distSq <= BASE_NE_DIST) {
			xspeed = 0;
			yspeed = 0;
		}
	}
	//}
	/*if (currMove != NULL) {
		float distSq = (currMove->x - getSprite()->getPosition().x) * (currMove->x - getSprite()->getPosition().x) + (currMove->y - getSprite()->getPosition().y) * (currMove->y - getSprite()->getPosition().y);
		if (distSq <= BASE_NE_DIST) {
			currMove = brain->nextMove();
			goalAngle = atan2(getSprite()->getPosition().y - currMove->y, getSprite()->getPosition().x - currMove->x);
			xspeed = -cos(goalAngle);
			yspeed = -sin(goalAngle);
		}
	}*/
	/*
	
	if (currMove != NULL) {
		float distSq = (currMove->x - getSprite()->getPosition().x) * (currMove->x - getSprite()->getPosition().x) + (currMove->y - getSprite()->getPosition().y) * (currMove->y - getSprite()->getPosition().y);
		if (distSq <= BASE_NE_DIST){
			xspeed = 0;
			yspeed = 0;
		}
	}
	*/
	//MOVING AND ANIMATING THE SPRITE
	//removeFlags({ animation_t::MOVE_LEFT, animation_t::MOVE_RIGHT });
	/*
	if(xspeed * xspeed > 0 || yspeed * yspeed > 0){
		removeFlags({ animation_t::IDLE, animation_t::FACE_LEFT, animation_t::FACE_RIGHT });
		if (yspeed != 0) {
			if(areFlagsSet({animation_t::MOVE_LEFT})){
				addFlags({ animation_t::MOVE_LEFT });
				removeFlags({ animation_t::MOVE_RIGHT });
			}else if(areFlagsSet({ animation_t::MOVE_RIGHT })){
				addFlags({ animation_t::MOVE_RIGHT });
				removeFlags({ animation_t::MOVE_LEFT });
			}else{
				addFlags({ animation_t::MOVE_RIGHT });
			}
		}
		_x += xspeed;
		if (xspeed > 0) {
			addFlags({ animation_t::MOVE_RIGHT});
			removeFlags({ animation_t::MOVE_LEFT, animation_t::IDLE });
		}else if(xspeed < 0){
			addFlags({ animation_t::MOVE_LEFT });
			removeFlags({ animation_t::MOVE_RIGHT, animation_t::IDLE });
		}
		_y += yspeed;
		
	}else{
		if(areFlagsSet({animation_t::MOVE_LEFT})){
			removeFlags({ animation_t::MOVE_LEFT, animation_t::MOVE_RIGHT, animation_t::FACE_RIGHT });
			addFlags({ animation_t::FACE_LEFT });
		} else if (areFlagsSet({ animation_t::MOVE_RIGHT })) {
			removeFlags({ animation_t::MOVE_RIGHT, animation_t::MOVE_LEFT, animation_t::FACE_LEFT });
			addFlags({ animation_t::FACE_RIGHT });
		}
	}
	manageAnimations();
	currSprite->getSprite()->setPosition(_x, _y);
	currSprite->update();*/
}
