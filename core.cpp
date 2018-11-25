#include "stdafx.h"
#include "core.h"
#include "graphics.h"
#include "globals.h"
#include "terrain.h"

void Core::update() {
	for (size_t i = 0; i < characters.size(); i++) {
		characters.at(i)->emptyRegistry();
		for (size_t j = 0; j < characters.size(); j++) {
			//registering entities
			//fprintf(stderr, "%d %d\n", i, j);
			if (i != j) {	
				//int distSq = rand() % 1000000;
				//if (distSq < 10000) {
					characters.at(i)->registerEntity(characters.at(j));
				//}
			}
			//fprintf(stderr, "%d %d\n", i, j);
		}
		if (characters.at(i)->isControlled())
			characters.at(i)->update();
		else
			characters.at(i)->update_AI();
	}
}

void Core::generateNPCs(int num) {
	for (int i = 0; i < num; i++) {
		sf::Texture* _npcTexture = graphics->loadImage("resources/sprites/squid_npc.png");
		int randX = rand() % WINDOW_WIDTH;
		int randY = rand() % WINDOW_HEIGHT;
		Playable* _npc = new Playable(_npcTexture, 32, 32, randX, randY);
		
		_npc->addAnimation(0, 0, 0, { animation_t::IDLE });
		_npc->addAnimation(0, 1, 6, { animation_t::MOVE_RIGHT });
		_npc->addAnimation(0, 0, 6, { animation_t::MOVE_LEFT });
		_npc->addAnimation(0, 0, 0, { animation_t::FACE_LEFT });
		_npc->addAnimation(0, 1, 0, { animation_t::FACE_RIGHT });
		characters.emplace(characters.end(), _npc);
	}
}
void Core::load() {
	//load floor
	
	for (int i = (int)tile_t::FLOOR_1; i < (int)tile_t::NUM_TILES; i++) {
		std::string filePath = "resources/tiles/floor_"+ std::to_string(i+1) +".png";
		sf::Texture* _tileTexture = graphics->loadImage(filePath);
		terrain->registerTile((tile_t)i, _tileTexture);
	}
	map = (sf::Sprite**)malloc(sizeof(sf::Sprite*) * MAP_HEIGHT * MAP_WIDTH);
	for (int j = 0; j < MAP_HEIGHT; j++) {
		for (int i = 0; i < MAP_WIDTH; i++) {
			map[i + j * MAP_WIDTH] = new sf::Sprite;
			map[i + j * MAP_WIDTH]->setTexture(*(terrain->getTileTAt(i, j)));
			map[i + j * MAP_WIDTH]->setPosition(i * 16, j * 16);
		}
	}
	//load player sprite
	sf::Texture* _playerTexture = graphics->loadImage("resources/sprites/squid_player.png");
	Playable* _player = new Playable(_playerTexture, 32, 32, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	
	//load animations
	_player->addAnimation(0, 1, 6, { animation_t::MOVE_RIGHT });
	_player->addAnimation(0, 0, 6, { animation_t::MOVE_LEFT });
	_player->addAnimation(0, 0, 0, { animation_t::FACE_LEFT });
	_player->addAnimation(0, 1, 0, { animation_t::FACE_RIGHT });
	
	//place player into update cycle

	//_player->setControl(true);
	characters.emplace(characters.end(), _player);
	
	//load NPCs
	generateNPCs(1);
}

void Core::draw(sf::RenderWindow* window) {
	(*window).clear(sf::Color::Black);
	for (int j = 0; j < MAP_HEIGHT; j++) {
		for (int i = 0; i < MAP_WIDTH; i++) {
			window->draw(*map[i + j * MAP_WIDTH]);
		}
	}
	for (size_t i = 0; i < characters.size(); i++) {
		(*window).draw(*characters.at(i)->getSprite());
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