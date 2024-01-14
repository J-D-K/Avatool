#pragma once
#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <memory>
#include <string>
#include <cstdio>
#include <map>
#include <utility>
#include "graphics/textureManager.hpp"

typedef struct
{
    uint16_t width, height;
    int advanceX, top, left;
    SDL_Texture *tex;
} glyph;

class systemFont
{
    public:
        systemFont();
        ~systemFont();

        void renderText(SDL_Renderer *renderer, SDL_Texture *target, int fontSize, uint32_t color, int x, int y, const char *str);
        void renderTextWrap(SDL_Renderer *render, SDL_Texture *target, int fontSize, int x, int y, int maxWidth, uint32_t color, const char *str);
        int getTextWidth(SDL_Renderer *renderer, std::string text, int fontSize);

    private:
        //Freetype
        FT_Library m_Lib;
        FT_Face m_Faces[6];
        //How many fonts the system actually loaded
        int m_TotalFonts = 0;
        //Fonts get their own texture manager
        std::unique_ptr<textureManager> m_GlyphTextureManager;
        //Map keeps pointer to texture and coordinates for glyphs already loaded and converted
        std::map<std::pair<uint32_t, int>, glyph> m_GlyphMap;

        // No need for access outside of here
        void resizeFont(int fontSize);
        FT_GlyphSlot loadGlyph(uint32_t c, FT_Int32 flags);
        glyph *getGlyph(SDL_Renderer *renderer, uint32_t c, int size);
};