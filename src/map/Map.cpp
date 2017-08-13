#include "../headers/Map.hpp"

#include <SDL2/SDL.h>
#include "../headers/Constants.hpp"
#include "../headers/Tileset.hpp"
#include "../headers/Tile.hpp"
#include "../headers/Window.hpp"
#include "../headers/Util.hpp"
#include "../headers/ImageLoader.hpp"
#include "../headers/PlayerCharacter.hpp"

Map::Map() {
	this->width = 0;
	this->height = 0;
	this->tileset = NULL;
	this->playerCharacter = NULL;
}

Map::Map(int w, int h, std::vector<int **>tileCoords, Tileset *tileset) {
	this->width = w;
	this->height = h;
	this->tileset = tileset;
	this->mapTiles = tileCoords;
	this->playerCharacter = NULL;
}

Map::~Map() {
	//Clear the map tiles
	for (unsigned int j = 0; j < mapTiles.size(); j++) {
		for (int i = 0; i < width / Constants::SPRITE_TILE_WIDTH; i++) {
			delete[] mapTiles[j][i];
			mapTiles[j][i] = NULL;
		}
		delete[] mapTiles[j];
		mapTiles[j] = NULL;
	}
	mapLayers.clear();

	//Clear the map layers (textures)
	for (unsigned int j = 0; j < mapLayers.size(); j++) {
		SDL_DestroyTexture(mapLayers[j]);
		mapLayers[j] = NULL;
	}
	mapLayers.clear();

	//Delete the overworld character
	if (playerCharacter != NULL) {
		delete playerCharacter;
		playerCharacter = NULL;
	}
	
	//Map loader will handle deletion of tilesets
	tileset = NULL;
}

void Map::draw(Window *win) {
	//The map has not been generated yet
	if (mapLayers.size() < 1) {
		generate(win);
	}

	//draw the map in respect to the player
	else if(playerCharacter != NULL) {
		SDL_Rect mapSrc;
		mapSrc.x = (playerCharacter->getMapX()) - ((Constants::MAP_NUM_TILES_WIDTH / 2) * Constants::SPRITE_TILE_WIDTH);
		mapSrc.y = (playerCharacter->getMapY()) - ((Constants::MAP_NUM_TILES_HEIGHT / 2) * Constants::SPRITE_TILE_WIDTH);
		mapSrc.w = Constants::SPRITE_TILE_WIDTH * Constants::MAP_NUM_TILES_WIDTH;
		mapSrc.h = Constants::SPRITE_TILE_WIDTH * Constants::MAP_NUM_TILES_HEIGHT;
		for (unsigned int i = 0; i < mapLayers.size(); i++) {
			win->drawTexture(mapLayers[i], &mapSrc, NULL);
			if (i == playerCharacter->getCurrentMapLayer()) {
				playerCharacter->draw(win);
			}
		}
	}

	//since there is no player,
	//draw the map in its entirety
	else {
		for (unsigned int i = 0; i < mapLayers.size(); i++) {
			win->drawTexture(mapLayers[i], NULL, NULL);
		}
	}
}

void Map::update() {
	int i = 0;
	Tile *tile = tileset->getTile(i);
	while (tile != NULL) {
		tile->update();
		i++;
		tile = tileset->getTile(i);
	}
	if (playerCharacter != NULL) {
		playerCharacter->update();
	}
}

void Map::generate(Window *win) {
	//If there is already a map or the tileset image hasn't loaded, don't generate the map
	if (mapLayers.size() > 0 || ImageLoader::getInstance()->getImage(tileset->getImage()) == NULL) return;
	
	for (unsigned int layer = 0; layer < mapTiles.size(); layer++) {
		SDL_Texture *layerTexture = win->createTransparentTexture(
			Constants::SPRITE_TILE_WIDTH * width,
			Constants::SPRITE_TILE_HEIGHT * height
		);
		win->setRenderTarget(layerTexture);
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (mapTiles[layer][j][i] - 1 < 0) continue;
				Tile *currTile = tileset->getTile(mapTiles[layer][j][i] - 1);
				SDL_Rect dst;
				dst.x = j * Constants::SPRITE_TILE_WIDTH;
				dst.y = i * Constants::SPRITE_TILE_HEIGHT;
				dst.w = Constants::SPRITE_TILE_WIDTH;
				dst.h = Constants::SPRITE_TILE_HEIGHT;
				currTile->setDestinationRect(dst);
				currTile->draw(win);
			}
		}
		mapLayers.push_back(layerTexture);
	}
	win->resetRenderTarget();
}

void Map::placePlayer(int x, int y) {
	if (playerCharacter == NULL) {
		playerCharacter = new PlayerCharacter(Constants::IMAGE_CHARACTER_1,
			Constants::SPRITE_CHARACTER_X,
			Constants::SPRITE_CHARACTER_Y,
			Constants::SPRITE_CHARACTER_WIDTH,
			Constants::SPRITE_CHARACTER_HEIGHT);
	}
	playerCharacter->setMapX(x);
	playerCharacter->setMapY(y);
}

void Map::removePlayer() {
	if (playerCharacter != NULL) {
		delete playerCharacter;
		playerCharacter = NULL;
	}
}

Tileset * Map::getTileset() const { return tileset; }
int Map::getWidth() const { return width; }
int Map::getHeight() const { return height; }
std::vector<int **> Map::getLayers() const { return mapTiles; }
BaseCharacter * Map::getPlayer() const { return playerCharacter; }

void Map::setTileset(Tileset *ts) { this->tileset = ts; }
void Map::setWidth(int w) { this->width = w; }
void Map::setHeight(int h) { this->height = h; }
void Map::addLayer(int **tiles) { this->mapTiles.push_back(tiles); }
