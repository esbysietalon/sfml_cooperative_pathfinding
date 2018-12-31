#pragma once
#include "globals.h"
#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <map>

enum tile_t {FLOOR_1, FLOOR_2, FLOOR_3, FLOOR_4, FLOOR_5, FLOOR_6, FLOOR_7, FLOOR_8, GAP, NUM_TILES};
class Terrain {
public:
	Terrain();
	~Terrain();
	tile_t getTileAt(int x, int y);
	sf::Texture* getTileTAt(int x, int y);
	void registerTile(tile_t tile, sf::Texture* texture);
private:
	void generateMap();
	void generateGaps();
	tile_t* _map;
	std::map<tile_t, sf::Texture*> tileSet;
};