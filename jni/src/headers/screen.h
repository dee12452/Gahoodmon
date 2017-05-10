#ifndef GAHOOD_SCREEN_H
#define GAHOOD_SCREEN_H

struct SDL_Renderer;
enum GameState;

void gahood_screenInit();
void gahood_screenDraw(struct SDL_Renderer *);
void gahood_screenUpdate(enum GameState);
void gahood_screenClose();

#endif