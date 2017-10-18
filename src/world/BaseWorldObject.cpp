#include "BaseWorldObject.hpp"
#include "../game/Sprites.hpp"

BaseWorldObject::BaseWorldObject(SpriteSheet *spriteSheet) : 
    posX(0), 
    posY(0), 
    layer(0), 
    objectSprite(spriteSheet->createSprite()) {
}

BaseWorldObject::~BaseWorldObject() {
    if(objectSprite != NULL) {
        delete objectSprite;
        objectSprite = NULL;
    }
}

void BaseWorldObject::setPositionX(int x) { posX = x; }
void BaseWorldObject::setPositionY(int y) { posY = y; }
void BaseWorldObject::setLayer(int newLayer) { layer = newLayer; }

int BaseWorldObject::getPositionX() const { return posX; }
int BaseWorldObject::getPositionY() const { return posY; }
int BaseWorldObject::getLayer() const { return layer; }
Sprite * BaseWorldObject::getSprite() const { return objectSprite; }