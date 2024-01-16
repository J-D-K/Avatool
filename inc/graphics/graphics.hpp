#pragma once
#include <memory>
#include <SDL2/SDL.h>
#include "graphics/colors.hpp"
#include "graphics/font.hpp"
#include "graphics/textureManager.hpp"

class graphics
{
    public:
        graphics();
        ~graphics();

        void beginFrame(uint32_t clearColor);
        void endFrame();

        // Font is private
        void renderTextf(SDL_Texture *target, int fontSize, uint32_t color, int x, int y, const char *format, ...);
        void renderTextfWrap(SDL_Texture *target, int fontSize, uint32_t color, int x, int y, int maxWidth, const char *format, ...);
        int getTextWidth(const std::string &text, int fontSize) { return m_SystemFont->getTextWidth(m_Renderer, text, fontSize); }

        // I dont' want to make the manager public
        SDL_Texture *textureCreate(std::string textureName, int width, int height, uint32_t sdlTextureFlags) { return m_TextureManager->textureCreate(textureName, m_Renderer, width, height, sdlTextureFlags); }
        SDL_Texture *textureCreateFromSurface(std::string textureName, SDL_Surface *surface) { return m_TextureManager->textureCreateFromSurface(textureName, m_Renderer, surface); }
        SDL_Texture *textureLoadFromFile(std::string textureName, const char *path) { return m_TextureManager->textureLoadFromFile(textureName, m_Renderer, path); }
        SDL_Texture *textureLoadFromMem(std::string textureName, imageTypes imgType, const std::byte *data, size_t dataSize) { return m_TextureManager->textureLoadFromMem(textureName, m_Renderer, imgType, data, dataSize); }

        // Texture drawing/rendering functions
        void clearTexture(SDL_Texture *texture, uint32_t color);
        void renderTexture(SDL_Texture *texture, SDL_Texture *target, int x, int y);
        void renderTexturePart(SDL_Texture *texture, SDL_Texture *target, int x, int y, int sourceX, int sourceY, int sourceWidth, int sourceHeight);
        void renderTextureStretched(SDL_Texture *texture, SDL_Texture *target, int x, int y, int width, int height);

        // Primitives/misc
        void renderLine(SDL_Texture *target, uint32_t color, int x1, int y1, int x2, int y2);
        void renderRectFill(SDL_Texture *target, uint32_t color, int x, int w, int width, int height);

    private:
        SDL_Renderer *m_Renderer;
        SDL_Window *m_Window;
        std::unique_ptr<systemFont> m_SystemFont;
        std::unique_ptr<textureManager> m_TextureManager;
};