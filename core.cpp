#include "stdafx.h"
#include "core.h"
#include "graphics.h"
#include "globals.h"
#include "terrain.h"
#include <stdio.h>
#include "dstarlite.h"
#include "anytimedstar.h"

void Core::update() {
	for (int j = 0; j < MAP_HEIGHT; j++) {
		for (int i = 0; i < MAP_WIDTH; i++) {
			rMap[i + j * MAP_WIDTH] = (Playable*)((terrain->getTileAt(i, j) != tile_t::GAP) ? rmap::EMPTY : rmap::TERRAIN);
		}
	}
	for (size_t i = 0; i < characters.size(); i++) {
		//fprintf(stderr, "%d:", characters.at(i)->getSocial());
		
		int x = characters.at(i)->moveGoals().x / TILE_SIZE;
		int y = characters.at(i)->moveGoals().y / TILE_SIZE;
		//fprintf(stderr, "(%d,%d)", x, y);
		for (int iy = -1 * PERSONAL_SPACE; iy <= PERSONAL_SPACE; iy++) {
			for (int ix = -1 * PERSONAL_SPACE; ix <= PERSONAL_SPACE; ix++) {
				int index = x + ix + (y + iy) * MAP_WIDTH;
				if (index >= 0 && index < MAP_HEIGHT * MAP_WIDTH && rMap[index] == 0)
					rMap[index] = characters.at(i);
			}
		}

		x = characters.at(i)->getX() / TILE_SIZE;
		y = characters.at(i)->getY() / TILE_SIZE;
		
		for (int iy = -1 * PERSONAL_SPACE; iy <= PERSONAL_SPACE; iy++) {
			for (int ix = -1 * PERSONAL_SPACE; ix <= PERSONAL_SPACE; ix++) {
				int index = x + ix + (y + iy) * MAP_WIDTH;
				if (index >= 0 && index < MAP_HEIGHT * MAP_WIDTH && ((rMap[index] == 0) || (ix == 0 && iy == 0)))
					rMap[index] = characters.at(i);
			}
		}
		//fprintf(stderr, "%d - i\n", i);
		
		//fprintf(stderr, "||%d", i);
		//fprintf(stderr, "%d - i\n", i);
	}

	/*for (int j = 0; j < MAP_HEIGHT; j++) {
		for (int i = 0; i < MAP_WIDTH; i++) {
			fprintf(stderr, "%d ", std::min(1,(int)(rMap[i + j * MAP_WIDTH]) % 10));
		}
		fprintf(stderr, "\n");
	}*/
	//fprintf(stderr, "\n\n");
	for (int i = 0; i < characters.size(); i++) {
		//fprintf(stderr, "%d: \n\n", i);
		characters.at(i)->emptyRegistry();
		characters.at(i)->see();
		
		characters.at(i)->printLemory();
		if (characters.at(i)->isControlled()) {
			//fprintf(stderr, "-pl");
			characters.at(i)->update();
		}
		else {
			//fprintf(stderr, "-ai");
			characters.at(i)->update_AI();
		}
		for (int j = 0; j < characters.size(); j++) {
			if (i == j)
				continue;
			if (characters.at(i)->moveGoals() == characters.at(j)->moveGoals()) {
				fprintf(stderr, "%d and %d collision at (%d,%d)\n", i, j, characters.at(i)->moveGoals().x / TILE_SIZE, characters.at(i)->moveGoals().y / TILE_SIZE);
				fprintf(stderr, "\n\n");
			}
			if (characters.at(i)->moveGoals() == intpair(characters.at(j)->getX(), characters.at(j)->getY())) {
				fprintf(stderr, "%d and %d collision at (%d,%d)-\n", i, j, characters.at(i)->moveGoals().x / TILE_SIZE, characters.at(i)->moveGoals().y / TILE_SIZE);
				fprintf(stderr, "\n\n");
			}
			if (characters.at(i)->getX() == characters.at(j)->getX() && characters.at(i)->getY() == characters.at(j)->getY()) {
				fprintf(stderr, "%d and %d collision at (%d,%d)\n", i, j, characters.at(i)->getX(), characters.at(i)->getY());
				fprintf(stderr, "\n\n");
			}
		}
	}
	
	//fprintf(stderr, "\nfollowers:%d\n", followers);
	/*if (followers == NPC_NUM) {
		fprintf(stderr, "WE DID IT\n");
	}*/
}

void Core::calculateCFov() {
	std::vector<intpair> listed;
	//FILE* fov_file = fopen("cfov.txt", "w");
	//fclose(fov_file);
//fov_file = fopen("cfov.txt", "a+");
	/*int* ringList = new int[BASE_SIGHT_RANGE * 2 + 2];
	for (int i = 0; i < BASE_SIGHT_RANGE * 2 + 2; i++) {
		ringList[i] = 0;
	}*/
	for (int rad = 0; rad <= BASE_SIGHT_RANGE; rad++) {
	//fprintf(fov_file, "\nstd::vector<struct intpair>* newRing%d = new std::vector<struct intpair>();\n", rad);
	int ringLength = 0;
	int skipDist = (int)((rad / sqrt(2)));
	//fprintf(stderr, "%d, %d\n", rad, skipDist);
	std::vector<fov_data> newRing;
	int loops = 0;
	for (int y = -rad; y <= rad; y++) {
		for (int x = -rad; x <= rad; x++) {
			loops++;
			if (rad > skipDist && y >= -skipDist && y <= skipDist) {
				if (x >= -skipDist && x <= skipDist) {
					x = skipDist;
					continue;
				}
			}
			float dist = sqrt(x * x + y * y);
			if (floor(dist) > rad) {
				if (x < 0)
					continue;
				//x += floor(sqrt((dist - rad) * (dist - rad) - y * y * (dist - rad) / (float)dist));
				else
					x = rad;
				continue;
			}
			//if (floor(dist) <= rad) {
			intpair pt(x, y);
			fov_data point = fov_data(pt);
			
			bool notListed = false;
			for (int i = 0; i < listed.size(); i++) {
				if (listed.at(i).x == x && listed.at(i).y == y) {
					notListed = true;
					break;
				}
			}
			//if(ringList[(x+BASE_SIGHT_RANGE+1) + (y+BASE_SIGHT_RANGE+1) * (BASE_SIGHT_RANGE+1)] == 0){
			if(!notListed){
				//ringList[(x + BASE_SIGHT_RANGE+1) + (y + BASE_SIGHT_RANGE+1) * (BASE_SIGHT_RANGE + 1)] = 1;
				listed.push_back(point.point);
				if (point.point.x * point.point.y > 0) {
					if (point.point.x > 0) {
						point.eAng = atan2(point.point.y + 1, point.point.x);
						point.oAng = atan2(point.point.y, point.point.x + 1);
						//fprintf(stderr, "1\n");
					}else {
						//fprintf(stderr, "2\n");
						point.eAng = atan2(point.point.y, point.point.x + 1);
						point.oAng = atan2(point.point.y + 1, point.point.x);
					}
				}
				else if (point.point.x * point.point.y < 0) {
					if(point.point.x > 0){
						//fprintf(stderr, "3\n");
						point.eAng = atan2(point.point.y + 1, point.point.x + 1);
						point.oAng = atan2(point.point.y, point.point.x);
					}else {
						//fprintf(stderr, "4\n");
						point.eAng = atan2(point.point.y, point.point.x);
						point.oAng = atan2(point.point.y + 1, point.point.x + 1);	
					}
				}else {
					if (point.point.x == 0 && point.point.y == 0) {
						point.oAng = 0;
						point.eAng = 2 * PI;
						//fprintf(stderr, "5\n");
					}else if (point.point.x == 0) {
						if (point.point.y > 0) {
							//fprintf(stderr, "6\n");
							point.eAng = atan2(point.point.y, point.point.x);
							point.oAng = atan2(point.point.y, point.point.x + 1);
						}
						else {
							//fprintf(stderr, "7\n");
							point.eAng = atan2(point.point.y + 1, point.point.x + 1);
							point.oAng = atan2(point.point.y + 1, point.point.x);
						}
					}else if (point.point.y == 0) {
						if (point.point.x > 0) {
							//fprintf(stderr, "8\n");
							point.eAng = atan2(point.point.y + 1, point.point.x);
							point.oAng = atan2(point.point.y, point.point.x);
						}
						else {
							//fprintf(stderr, "9\n");
							point.eAng = atan2(point.point.y, point.point.x + 1);
							point.oAng = atan2(point.point.y + 1, point.point.x + 1);
						}
					}
				}
				
				/*if (point.oAng > point.eAng && !(point.oAng < 0 && point.eAng < 0)) {
					fprintf(stderr, "(%d,%d) - [%f,%f]\n", point.point.x, point.point.y, point.oAng, point.eAng);
				}*/
				newRing.emplace_back(point);
				ringLength++;
				
				//fprintf(fov_file, "newRing%d->emplace(newRing%d.end(), intpair(%d, %d));\n", rad, rad, point.x, point.y);
			}
			//}
		}
		
	}
	cFovRings.emplace_back(newRing);
	//fprintf(fov_file, "cFovRings.emplace(cFovRings.end(), newRing%d);\n", rad);
	//fprintf(stderr, "%d\n", rad);
}
//fprintf(stderr, "%d", skippedLoops);
//fclose(fov_file);
}

void Core::generateNPCs(int num) {
	for (int i = 0; i < num; i++) {
		sf::Texture* _npcTexture = graphics->loadImage("resources/sprites/squid_npc.png");
		int randX = (rand() % WINDOW_WIDTH) / TILE_SIZE * TILE_SIZE;
		int randY = (rand() % WINDOW_HEIGHT) / TILE_SIZE * TILE_SIZE;
		//randX = 0;
		//randY = 0;
		Playable* _npc = new Playable(_npcTexture, SPRITE_SIZE, SPRITE_SIZE, randX, randY);

		_npc->setFov(cFovRings);
		_npc->setRMap(&rMap);
		_npc->addAnimation(0, 0, 0, { animation_t::IDLE });
		_npc->addAnimation(0, 1, 6, { animation_t::MOVE_RIGHT });
		_npc->addAnimation(0, 0, 6, { animation_t::MOVE_LEFT });
		_npc->addAnimation(0, 0, 0, { animation_t::FACE_LEFT });
		_npc->addAnimation(0, 1, 0, { animation_t::FACE_RIGHT });
		characters.emplace(characters.end(), _npc);
	}
}
void Core::load() {
	//calculate fov
	calculateCFov();

	rMap = new Playable*[MAP_WIDTH*MAP_HEIGHT];



	//load floor
	for (int i = (int)tile_t::FLOOR_1; i < (int)tile_t::NUM_TILES - 1; i++) {
		std::string filePath = "resources/tiles/floor_" + std::to_string(i + 1) + ".png";
		sf::Texture* _tileTexture = graphics->loadImage(filePath);
		terrain->registerTile((tile_t)i, _tileTexture);
	}

	std::string filePath = "resources/tiles/gaptile.png";
	sf::Texture* _tileTexture = graphics->loadImage(filePath);
	terrain->registerTile(tile_t::GAP, _tileTexture);
	
	map = (sf::Sprite**)malloc(sizeof(sf::Sprite*) * MAP_HEIGHT * MAP_WIDTH);
	for (int j = 0; j < MAP_HEIGHT; j++) {
		for (int i = 0; i < MAP_WIDTH; i++) {
			map[i + j * MAP_WIDTH] = new sf::Sprite;
			map[i + j * MAP_WIDTH]->setTexture(*(terrain->getTileTAt(i, j)));
			map[i + j * MAP_WIDTH]->setPosition(i * 16, j * 16);
			rMap[i + j * MAP_WIDTH] = (Playable*)((terrain->getTileAt(i, j) != tile_t::GAP) ? rmap::EMPTY : rmap::TERRAIN);
		}
	}

	/*fprintf(stderr, "loading pathfinder\n");
	Graph* g = new Graph(&rMap, MAP_WIDTH, MAP_HEIGHT, NULL);
	PathFinderPlus* pf = new PathFinderPlus(g, 4);
	fprintf(stderr, "checking U\n");
	int wayX = (rand() % WINDOW_WIDTH) / TILE_SIZE;
	int wayY = (rand() % WINDOW_HEIGHT) / TILE_SIZE;
	pf->replanPath(intpair(0, 0), intpair(wayX, wayY));
	std::deque<move_t>* path = pf->findPath(intpair(0, 0), intpair(wayX, wayY));
	fprintf(stderr, "generated path (of size %d) from (0,0) to (%d,%d) is: \n", path->size(), wayX, wayY);
	for (int i = 0; i < path->size(); i++){
		fprintf(stderr, "%d\n", path->at(i));
	}*/
	
	//load player sprite
	sf::Texture* _playerTexture = graphics->loadImage("resources/sprites/squid_player.png");
	Playable* _player = new Playable(_playerTexture, SPRITE_SIZE, SPRITE_SIZE, WINDOW_WIDTH / 2 / TILE_SIZE * TILE_SIZE, WINDOW_HEIGHT / 2 / TILE_SIZE * TILE_SIZE);
	
	_player->setFov(cFovRings);

	//load animations
	_player->addAnimation(0, 1, 6, { animation_t::MOVE_RIGHT });
	_player->addAnimation(0, 0, 6, { animation_t::MOVE_LEFT });
	_player->addAnimation(0, 0, 0, { animation_t::FACE_LEFT });
	_player->addAnimation(0, 1, 0, { animation_t::FACE_RIGHT });
	
	//place player into update cycle
	_player->setRMap(&rMap);
	//_player->setControl(true);
	//_player->setSocial(999);
	characters.emplace(characters.end(), _player);
	
	
	//fov check
	/*sf::Texture* _fovTexture = graphics->loadImage("resources/tiles/fovtile.png");
	for (int i = 0; i < cFovRings.size(); i++) {
		for (int j = 0; j < cFovRings.at(i)->size(); j++) {
			sf::Sprite* fovsprite = new sf::Sprite;
			fovsprite->setTexture(*_fovTexture);
			fovsprite->setPosition(_player->getX() + cFovRings.at(i)->at(j).x * TILE_SIZE, _player->getY() + cFovRings.at(i)->at(j).y * TILE_SIZE);
			//fprintf(stderr, "x: %d, y: %d\n", _player->getX() + cFovRings.at(i)->at(j).x * TILE_SIZE, _player->getY() + cFovRings.at(i)->at(j).y * TILE_SIZE);
			tokens.emplace_back(fovsprite);
		}
	}*/
	//fprintf(stderr, "%d\n", tokens.size());
	//load NPCs
	generateNPCs(NPC_NUM);
}

void Core::draw(sf::RenderWindow* window) {
	(*window).clear(sf::Color::Black);
	for (int j = 0; j < MAP_HEIGHT; j++) {
		for (int i = 0; i < MAP_WIDTH; i++) {

			window->draw(*(map[i + j * MAP_WIDTH]));
		}
	}
	for (size_t i = 0; i < characters.size(); i++) {
		(*window).draw(*characters.at(i)->getSprite());
	}
	for (size_t i = 0; i < tokens.size(); i++) {
		(*window).draw(*tokens.at(i));
	}
	(*window).display();
}

void Core::mainLoop() {
	sf::RenderWindow _window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "v0000");
	sf::Event event;
	sf::Clock clock;
	int FPS;

	srand(time(0));
	load();
	
	bool oneFrameDelay = true;
	
	while (_window.isOpen())
	{
		while (_window.pollEvent(event))
		{
			if (event.type == sf::Event::EventType::Closed) {
				_window.close();
			}

		}
		
		FPS = (int)(1000000.0 / clock.getElapsedTime().asMicroseconds());
		
		
		if(oneFrameDelay){
			oneFrameDelay = false;
		} else {
			update();
		}
		while (clock.getElapsedTime().asMicroseconds() < 1000000.0 / (FRAMERATE)) {

		}

		FPS = (int)(1000000.0 / clock.getElapsedTime().asMicroseconds());
		
		draw(&_window);

		std::string title = "Demo -" + std::to_string(FPS) + " FPS";
		_window.setTitle(title);
		clock.restart();
		
	}
}

Core::Core() {
	graphics = new Graphics();
	terrain = new Terrain();
	mainLoop();
}
Core::~Core() {

}