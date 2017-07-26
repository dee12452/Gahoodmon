#include "../headers/Map.hpp"

#include <SDL2/SDL_rect.h>
#include "../headers/Constants.hpp"
#include "../headers/Tileset.hpp"
#include "../headers/Tile.hpp"

Map::Map() {
	this->width = 0;
	this->height = 0;
	this->tileset = NULL;
}

Map::Map(int w, int h, std::vector<int **>tileCoords, Tileset *tileset) {
	this->width = w;
	this->height = h;
	this->tileset = tileset;
	this->mapLayers = tileCoords;
}

Map::~Map() {
	for (unsigned int j = 0; j < mapLayers.size(); j++) {
		for (int i = 0; i < width / Constants::TILE_WIDTH; i++) {
			delete[] mapLayers[j][i];
			mapLayers[j][i] = NULL;
		}
		delete[] mapLayers[j];
		mapLayers[j] = NULL;
	}
	mapLayers.clear();
	
	//Map loader will handle deletion of tilesets
	tileset = NULL;
}

void Map::draw(SDL_Renderer *r) {
}

void Map::update() {
	int i = 0;
	Tile *tile = tileset->getTile(i);
	while (tile != NULL) {
		tile->update();
		i++;
		tile = tileset->getTile(i);
	}
}

Tileset * Map::getTileset() const { return tileset; }
int Map::getWidth() const { return width; }
int Map::getHeight() const { return height; }
std::vector<int **> Map::getLayers() const { return mapLayers; }

void Map::setTileset(Tileset *ts) { this->tileset = ts; }
void Map::setWidth(int w) { this->width = w; }
void Map::setHeight(int h) { this->height = h; }
void Map::addLayer(int **tiles) { this->mapLayers.push_back(tiles); }

