#include "stdafx.h"
#include "terrain.h"

void Terrain::generateMap() {
	_map = (tile_t *) malloc(MAP_WIDTH * MAP_HEIGHT * sizeof(tile_t));
	for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {
		_map[i] = (tile_t) (rand() % tile_t::NUM_TILES);
	}
}
void Terrain::registerTile(tile_t tile, sf::Texture* texture) {
	tileSet[tile] = texture;
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
