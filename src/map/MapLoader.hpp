#ifndef MAP_LOADER_HPP
#define MAP_LOADER_HPP

#include <map>
#include <vector>
#include <string>

class Tileset;
class Map;
class Game;
struct Tag;

class MapLoader {
public:
	static MapLoader * getInstance();
	static void deleteInstance();

    void loadAll(Game *game, const char *pathToRes);
    Map * getMap(const std::string &mapId) const;

private:
	MapLoader();
	~MapLoader();
	static MapLoader *instance;

	void loadTileset(const char *pathToTileset);
	void loadMap(Game *game, const char *pathToMap);
    void populateMapInfo(Tag *tag, Map *map);
	std::vector<Tileset *> tilesets;
    std::map<std::string, Map *> maps;
};

#endif
