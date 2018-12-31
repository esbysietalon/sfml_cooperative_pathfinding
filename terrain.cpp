#include "stdafx.h"
#include "terrain.h"

void Terrain::generateMap() {
	_map = new tile_t[MAP_WIDTH * MAP_HEIGHT];
	for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {
		_map[i] = (tile_t) (rand() % (tile_t::NUM_TILES ));
	}
	generateGaps();
}
void Terrain::registerTile(tile_t tile, sf::Texture* texture) {
	tileSet[tile] = texture;
}

void Terrain::generateGaps() {
	//fprintf(stderr, "LIMIT: %d\n", MAP_WIDTH * MAP_HEIGHT);
	for (int i = 0; i < GAP_BLOCK_NUM; i++) {
		int sx = rand() % MAP_WIDTH;
		int sy = rand() % MAP_HEIGHT;
		//fprintf(stderr, "(sx,sy):(%d,%d)\n", sx, sy);
		for (int y = -1 * GAP_BLOCK_HEIGHT / 2; y <= GAP_BLOCK_HEIGHT / 2; y++) {
			for (int x = -1 * GAP_BLOCK_WIDTH / 2; x <= GAP_BLOCK_WIDTH / 2; x++) {
				int index = (sx+x) + (sy+y) * MAP_WIDTH;
				//fprintf(stderr, "index: %d\n", index);
				if (index >= 0 && index < MAP_WIDTH * MAP_HEIGHT) {
					
					_map[index] = tile_t::GAP;
				}
			}
		}
	}
}
tile_t Terrain::getTileAt(int x, int y) {
	return _map[x + y * MAP_WIDTH];
}
sf::Texture* Terrain::getTileTAt(int x, int y) {
	return tileSet[_map[x + y * MAP_WIDTH]];
}
Terrain::Terrain() {
	generateMap();
}
Terrain::~Terrain() {
	free(_map);
}
