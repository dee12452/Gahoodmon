#include "World.hpp"

#include <algorithm>
#include <SDL2/SDL_render.h>
#include "../game/Game.hpp"
#include "../map/MapLoader.hpp"
#include "../sprite/Sprites.hpp"
#include "../util/Utils.hpp"
#include "WorldCharacter.hpp"
#include "WorldTextBox.hpp"

/**
* Move listener for the player
*/
class PlayerMoveListener : public WorldCharacterMoveListener {
private:
	World *world;
public:
	PlayerMoveListener(World *w) { world = w; }
	~PlayerMoveListener() override { world = 0; }

	void onMoveStart(FacingDirection direction, int tileX, int tileY) override;
	void onMove(FacingDirection direction, float percentToNextTile, int positionX, int positionY) override;
	void onMoveEnd(FacingDirection direction, int tileX, int tileY) override;
};

World::World() : map(NULL), mapTexture(NULL), player(NULL), routeTextBox(NULL) {}

World::~World() { 
	if(player != NULL) {
        delete player;
        player = NULL;
    }
	if (mapTexture != NULL) {
		SDL_DestroyTexture(mapTexture);
		mapTexture = NULL;
	}
	if (routeTextBox != NULL) {
		delete routeTextBox;
		routeTextBox = NULL;
	}
	map = NULL;
}

void World::start(Game *game) {
	changeMap(Constants::MAP_ROUTE_1);

	routeTextBox = new WorldTextBox(this, game->getSpriteSheet("choice 1.png"), game->getFont(Constants::FONT_JOYSTIX), false);
	player = new WorldCharacter(this, game->getSpriteSheet("NPC 01.png"), Constants::CHARACTER_WALK_TIMER, Constants::CHARACTER_WALK_SPEED);
	static_cast<WorldCharacter *>(player)->setOnMoveListener(new PlayerMoveListener(this));
	player->setTileX(9); player->setTileY(32);
	game->schedule(player);
	game->schedule(routeTextBox);
}

void World::stop(Game *game) {
	game->unschedule(player);
	game->unschedule(routeTextBox);
}

void World::render(Window *win) {
    drawMap(win);
}

/**
 * Draw the current map to the window
*/
void World::drawMap(Window *win) {

	int drawWidth = Constants::WORLD_DRAW_WIDTH * map->getTileWidth();
	int drawHeight = Constants::WORLD_DRAW_HEIGHT * map->getTileHeight();

	if(mapTexture == NULL) mapTexture = win->createTexture(drawWidth * 2, drawHeight * 2);

	win->setRenderTarget(mapTexture);
    win->clearRenderTarget();

	SDL_Rect mapSrc = Util::createRect(player->getPositionX(), player->getPositionY(),  drawWidth, drawHeight);
	SDL_Rect mapDst = Util::createRect(drawWidth / 2, drawHeight / 2, drawWidth, drawHeight);

	//Deal with x < 0
	int targetX = mapSrc.x - drawWidth / 2 + map->getTileWidth() / 2;
	if (targetX < 0) {
		mapSrc.x = 0;
		mapDst.x -= player->getPositionX() - drawWidth / 2 + map->getTileWidth() / 2;
		drawBorderingMap(win, MapDirection::MAP_WEST, mapSrc, mapDst);
	}

	//Deal with x > map width
	else if (targetX > (map->getWidth() * map->getTileWidth()) - drawWidth) {
		mapSrc.x = map->getWidth() * map->getTileWidth() - drawWidth;
		mapDst.x += (map->getWidth() * map->getTileWidth())
			- player->getPositionX() - drawWidth / 2 - map->getTileWidth() / 2;
		drawBorderingMap(win, MapDirection::MAP_EAST, mapSrc, mapDst);
	}

	//No issues on x axis
	else {
		mapSrc.x = targetX;
	}

	//Deal with y < 0
	int targetY = mapSrc.y - drawHeight / 2 + map->getTileHeight() / 2;
	if (targetY < 0) {
		mapSrc.y = 0;
		mapDst.y -= player->getPositionY() - drawHeight / 2 + map->getTileHeight() / 2;
		drawBorderingMap(win, MapDirection::MAP_NORTH, mapSrc, mapDst);
	}

	//Deal with y > map height
	else if (targetY > (map->getHeight() * map->getTileHeight()) - drawHeight) {
		mapSrc.y = (map->getHeight() * map->getTileHeight()) - drawHeight;
		mapDst.y += (map->getHeight() * map->getTileHeight()) 
			- player->getPositionY() - drawHeight / 2 - map->getTileHeight() / 2;
		drawBorderingMap(win, MapDirection::MAP_SOUTH, mapSrc, mapDst);
	}

	//No issues on y axis
	else {
		mapSrc.y = targetY;
	}

    //Draw the map
	for (unsigned int i = 0; i < map->getNumberOfLayers(); i++) {
		win->drawTexture(map->getLayer(i), &mapSrc, &mapDst);
		if (i == static_cast<unsigned int> (player->getLayer() + 1)) {
			player->setRawX(drawWidth - player->getWidth() / 2);
			player->setRawY(drawHeight - player->getHeight() / 2 + Constants::CHARACTER_TILE_OFFSET_Y);
			player->draw(win);
		}
	}

	mapSrc = Util::createRect(drawWidth / 2, drawHeight / 2, drawWidth, drawHeight);

	win->resetRenderTarget();
	win->drawTexture(mapTexture, &mapSrc, NULL);

	if (routeTextBox != NULL) routeTextBox->draw(win);
}

/**
 * Draw any bordering map
 */
void World::drawBorderingMap(Window *win, MapDirection direction, SDL_Rect, SDL_Rect mapDst) {
	Map *borderMap = map->getBorderingMap(direction);
	if (borderMap == NULL) return;
	int drawWidth = Constants::WORLD_DRAW_WIDTH * borderMap->getTileWidth();
	int drawHeight = Constants::WORLD_DRAW_HEIGHT * borderMap->getTileHeight();
	SDL_Rect src = Util::createRect(player->getPositionX(), player->getPositionY(), drawWidth, drawHeight);
	src.x = src.x - drawWidth / 2 + borderMap->getTileWidth() / 2;
	SDL_Rect dst = Util::createRect(drawWidth / 2, drawHeight / 2, drawWidth, drawHeight);
	switch (direction) {
	case MapDirection::MAP_NORTH: {
		int h = mapDst.y - dst.y;
		src.y = borderMap->getHeight() * borderMap->getTileHeight() - h;
		src.h = h;
		dst.h = h;
		break;
	}
	case MapDirection::MAP_EAST: {
		break;
	}
	case MapDirection::MAP_SOUTH: {
		src.y = 0;
		src.h = dst.y - mapDst.y;
		dst.h = dst.y - mapDst.y;
		dst.y = mapDst.y + mapDst.h;
		break;
	}
	case MapDirection::MAP_WEST: {
		break;
	}
	default:
		return;
	}

	//Draw the map
	for (unsigned int i = 0; i < borderMap->getNumberOfLayers(); i++) {
		win->drawTexture(borderMap->getLayer(i), &src, &dst);
	}
}

/**
 *Change the current map
 */
void World::changeMap(const char * const mapFile) {
	changeMap(MapLoader::getInstance()->getMap(mapFile));
}

void World::changeMap(Map *newMap) {
	map = newMap;
	if (mapTexture != NULL) {
		SDL_DestroyTexture(mapTexture);
		mapTexture = NULL;
	}
	if (routeTextBox != NULL) {
		WorldTextBox *txtBox = static_cast<WorldTextBox *> (routeTextBox);
		txtBox->dismiss();
		txtBox->setText(map->getMapName());
		txtBox->show();
		txtBox->dismissAfter(5000);
	}
}

/**
 * Getters and setters
 */
Map * World::getMap() const { return map; }
BaseWorldObject * World::getPlayer() const { return player; }

/**
* Move listener for the player
*/
void PlayerMoveListener::onMove(FacingDirection, float, int, int) {}
void PlayerMoveListener::onMoveEnd(FacingDirection direction, int tileX, int tileY) {
	if (direction == FacingDirection::DOWN && tileY >= world->getMap()->getHeight()) {
		world->changeMap(world->getMap()->getBorderingMap(MapDirection::MAP_SOUTH));
		world->getPlayer()->setTileY(0);
	}
	else if (direction == FacingDirection::UP && tileY < 0) {
		world->changeMap(world->getMap()->getBorderingMap(MapDirection::MAP_NORTH));
		world->getPlayer()->setTileY(world->getMap()->getHeight() - 1);
	}
	else if (direction == FacingDirection::RIGHT && tileX >= world->getMap()->getWidth()) {
		world->changeMap(world->getMap()->getBorderingMap(MapDirection::MAP_EAST));
		world->getPlayer()->setTileX(0);
	}
	else if (direction == FacingDirection::LEFT && tileX < 0) {
		world->changeMap(world->getMap()->getBorderingMap(MapDirection::MAP_WEST));
		world->getPlayer()->setTileX(world->getMap()->getWidth() - 1);
	}
}
void PlayerMoveListener::onMoveStart(FacingDirection, int, int) {}
