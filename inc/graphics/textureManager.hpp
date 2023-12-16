#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <map>
#include "type.hpp"

typedef enum
{
    IMG_FMT_PNG,
    IMG_FMT_JPG
} imageTypes;

// Keeps pointers to textures to free them on exit. Uses strings to "name" them to prevent duplicates
class textureManager
{
    public:
        textureManager() = default;
        ~textureManager();

        // Creates and pushes new texture using SDL flags in sdlTextureFlags
        SDL_Texture *textureCreate(std::string textureName, SDL_Renderer *renderer, int width, int height, int textureAccess);
        // Creates texture from surface
        SDL_Texture *textureCreateFromSurface(std::string textureName, SDL_Renderer *renderer, SDL_Surface *surface);
        // Loads path
        SDL_Texture *textureLoadFromFile(std::string textureName, SDL_Renderer *renderer, const char *path);
        // Loads image from data. Only really used for icons
        SDL_Texture *textureLoadFromMem(std::string textureName, SDL_Renderer *renderer, imageTypes imgType, const byte *data, size_t dataSize);

    private:
        bool textureIsLoaded(std::string textureName);
        std::map<std::string, SDL_Texture *> textureMap;
};