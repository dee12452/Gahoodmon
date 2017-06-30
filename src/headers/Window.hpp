#ifndef WINDOW_HPP
#define WINDOW_HPP

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class BaseScreen;

class Window {
public:
    Window();
    ~Window();

    void render(BaseScreen *);
private:
    bool loadImages;
    SDL_Window *win;
    SDL_Renderer *winRenderer;
    SDL_Texture *winTexture;
};

#endif
