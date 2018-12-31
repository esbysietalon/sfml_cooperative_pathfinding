#include "stdafx.h"
#include "terrain.h"

void Terrain::generateMap() {
	_map = new tile_t[MAP_WIDTH * MAP_HEIGHT];
	for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {
		_map[i] = (tile_t) (rand() % (tile_t::NUM_TILES ));
	}
	//generateGaps();
}
void Terrain::registerTile(tile_t tile, sf::Texture* texture) {
	tileSet[tile] = texture;
}

void Terrain::generateGaps() {
	for (int i = 0; i < GAP_BLOCK_NUM; i++) {
		int sx = rand() % MAP_WIDTH;
		int sy = rand() % MAP_HEIGHT;
		for (int y = sy; y < sy + GAP_BLOCK_SIZE; y++) {
			for (int x = sx; x < sx + GAP_BLOCK_SIZE; x++) {
				int index = x + y * MAP_WIDTH;
				if(index >= 0 && index < MAP_WIDTH * MAP_HEIGHT)
					_map[index] = tile_t::GAP;
			}
		}
	}
}
tile_t Terrain::getTileAt(int x, int y) {
	return _map[x + y * MAP_HEIGHT];
}
sf::Texture* Terrain::getTileTAt(int x, int y) {
	return tileSet[_map[x + y * MAP_HEIGHT]];
}
Terrain::Terrain() {
	generateMap();
}
Terrain::~Terrain() {
	free(_map);
}
