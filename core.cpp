#include "stdafx.h"
#include "core.h"
#include "graphics.h"
#include "globals.h"
#include "terrain.h"
#include <stdio.h>
#include "dstarlite.h"

void Core::update() {
	for (int j = 0; j < MAP_HEIGHT; j++) {
		for (int i = 0; i < MAP_WIDTH; i++) {
			rMap[i + j * MAP_WIDTH] = (Playable*)((terrain->getTileAt(i, j) > 3) ? rmap::EMPTY : rmap::EMPTY);
		}
	}
	for (size_t i = 0; i < characters.size(); i++) {
		rMap[characters.at(i)->getX() / TILE_SIZE + characters.at(i)->getY() / TILE_SIZE * MAP_WIDTH] = characters.at(i);
		//fprintf(stderr, "%d - i\n", i);
		characters.at(i)->emptyRegistry();
		characters.at(i)->see();
		
		if (characters.at(i)->isControlled())
			characters.at(i)->update();
		else
			characters.at(i)->update_AI();
		
		//fprintf(stderr, "%d - i\n", i);
	}
}

void Core::calculateCFov() {
	std::vector<struct intpair> listed;
	//FILE* fov_file = fopen("cfov.txt", "w");
	//fclose(fov_file);
//fov_file = fopen("cfov.txt", "a+");
for (int rad = 0; rad <= BASE_SIGHT_RANGE; rad++) {
	//fprintf(fov_file, "\nstd::vector<struct intpair>* newRing%d = new std::vector<struct intpair>();\n", rad);
	int ringLength = 0;
	int skipDist = (int)((rad / sqrt(2)));
	//fprintf(stderr, "%d, %d\n", rad, skipDist);
	std::vector<struct intpair> newRing;
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
			struct intpair point(x, y);
			bool notListed = false;
			for (int i = 0; i < listed.size(); i++) {
				if (listed.at(i).x == x && listed.at(i).y == y) {
					notListed = true;
					break;
				}
			}
			if (!notListed) {
				listed.push_back(point);
				newRing.emplace_back(point);
				ringLength++;
				//fprintf(fov_file, "newRing%d->emplace(newRing%d.end(), intpair(%d, %d));\n", rad, rad, point.x, point.y);
			}
			//}
		}
	}
	//fprintf(stderr, "%d, %d, %d\n", rad, loops, ringLength);
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
	for (int i = (int)tile_t::FLOOR_1; i < (int)tile_t::NUM_TILES; i++) {
		std::string filePath = "resources/tiles/floor_" + std::to_string(i + 1) + ".png";
		sf::Texture* _tileTexture = graphics->loadImage(filePath);
		terrain->registerTile((tile_t)i, _tileTexture);
	}
	map = (sf::Sprite**)malloc(sizeof(sf::Sprite*) * MAP_HEIGHT * MAP_WIDTH);
	for (int j = 0; j < MAP_HEIGHT; j++) {
		for (int i = 0; i < MAP_WIDTH; i++) {
			map[i + j * MAP_WIDTH] = new sf::Sprite;
			map[i + j * MAP_WIDTH]->setTexture(*(terrain->getTileTAt(i, j)));
			map[i + j * MAP_WIDTH]->setPosition(i * 16, j * 16);
			rMap[i + j * MAP_WIDTH] = (Playable*)((terrain->getTileAt(i, j) > 3) ? rmap::EMPTY : rmap::EMPTY);
		}
	}

	/*fprintf(stderr, "loading pathfinder\n");
	Graph* g = new Graph(&rMap, MAP_WIDTH, MAP_HEIGHT);
	PathFinder* pf = new PathFinder(g);
	fprintf(stderr, "checking U\n");
	int wayX = (rand() % WINDOW_WIDTH) / TILE_SIZE;
	int wayY = (rand() % WINDOW_HEIGHT) / TILE_SIZE;
	std::deque<move_t>* path = pf->findPath(intpair(0, 0), intpair(wayX, wayY));
	fprintf(stderr, "generated path from (0,0) to (%d,%d) is: \n", wayX, wayY);
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
	_player->setControl(true);
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