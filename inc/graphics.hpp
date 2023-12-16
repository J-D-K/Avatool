#pragma once
#include <SDL2/SDL.h>
#include "graphics/colors.hpp"
#include "graphics/font.hpp"
#include "graphics/textureManager.hpp"

class graphics
{
    public:
        graphics();
        ~graphics();

        SDL_Renderer *getRenderer() { return this->renderer; };

        void beginFrame(uint32_t clearColor);
        void endFrame();

        // Font is private
        void renderTextf(SDL_Texture *target, int fontSize, uint32_t color, int x, int y, const char *format, ...);
        void renderTextfWrap(SDL_Texture *target, int fontSize, uint32_t color, int x, int y, int maxWidth, const char *format, ...);
        int getTextWidth(std::string text, int fontSize) { return this->sharedFont->getTextWidth(this->renderer, text, fontSize); }

        // I dont' want to make the manager public
        SDL_Texture *textureCreate(std::string textureName, int width, int height, uint32_t sdlTextureFlags) { return this->texManager->textureCreate(textureName, this->renderer, width, height, sdlTextureFlags); }
        SDL_Texture *textureCreateFromSurface(std::string textureName, SDL_Surface *surface) { return this->texManager->textureCreateFromSurface(textureName, this->renderer, surface); }
        SDL_Texture *textureLoadFromFile(std::string textureName, const char *path) { return this->texManager->textureLoadFromFile(textureName, this->renderer, path); }
        SDL_Texture *textureLoadFromMem(std::string textureName, imageTypes imgType, const byte *data, size_t dataSize) { return this->texManager->textureLoadFromMem(textureName, this->renderer, imgType, data, dataSize); }

        // Texture drawing/rendering functions
        void clearTexture(SDL_Texture *texture, uint32_t color);
        void renderTexture(SDL_Texture *texture, SDL_Texture *target, int x, int y);
        void renderTexturePart(SDL_Texture *texture, SDL_Texture *target, int x, int y, int sourceX, int sourceY, int sourceWidth, int sourceHeight);
        void renderTextureStretched(SDL_Texture *texture, SDL_Texture *target, int x, int y, int width, int height);

        // Primitives/misc
        void renderLine(SDL_Texture *target, uint32_t color, int x1, int y1, int x2, int y2);
        void renderRectFill(SDL_Texture *target, uint32_t color, int x, int w, int width, int height);

    private:
        SDL_Renderer *renderer;
        SDL_Window *window;
        systemFont *sharedFont;
        textureManager *texManager = NULL;
};