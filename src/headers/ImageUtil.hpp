#ifndef IMAGE_UTIL_HPP
#define IMAGE_UTIL_HPP

#include <string>
#include <map>
#include <vector>

struct SDL_Texture;
struct SDL_Renderer;

class ImageUtil {
public:
    static ImageUtil * getInstance();
    static void deleteInstance();

    void setImageFolder(const char *);
    void loadNextImage(SDL_Renderer *);
    void loadImage(SDL_Renderer *, const std::string &);
    bool hasLoadedAllImages() const;
    SDL_Texture * getImage(const std::string &) const;

private:
    unsigned int currentImage;
    std::vector<std::string> imageIds;
    std::map<std::string, SDL_Texture *> images;

    static ImageUtil *instance;
    ImageUtil();
    ~ImageUtil();
};

#endif
