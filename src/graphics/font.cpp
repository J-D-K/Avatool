#include <switch.h>
#include <cstring>
#include "graphics/colors.hpp"
#include "graphics/font.hpp"

static const uint32_t redMask   = 0xFF000000;
static const uint32_t greenMask = 0x00FF0000;
static const uint32_t blueMask  = 0x0000FF00;
static const uint32_t alphaMask = 0x000000FF;
static const uint32_t breakPoints[7] = {' ', L'　', '/', '_', '-', L'。', L'、'};

static bool isBreakCharacter(uint32_t codepoint)
{
    for(int i = 0; i < 7; i++)
    {
        if(codepoint == breakPoints[i])
            return true;
    }
    return false;
}

static size_t findNextBreakpoint(const char *str)
{
    uint32_t codepoint = 0;
    unsigned int stringLength = strlen(str);
    for(unsigned int i = 0; i < stringLength; )
    {
        ssize_t unitCount = decode_utf8(&codepoint, reinterpret_cast<const uint8_t *>(&str[i]));
        i += unitCount;
        if(unitCount <= 0)
            return stringLength;
        else if(isBreakCharacter(codepoint))
            return i;
    }
    return stringLength;
}

static bool processSpecialCharacters(uint32_t codepoint, uint32_t originalColor, uint32_t *color)
{
    bool isSpecialChar = true;
    switch(codepoint)
    {
        case L'#':
            *color = *color == COLOR_BLUE ? originalColor : COLOR_BLUE;
            break;

        case L'*':
            *color = *color == COLOR_RED ? originalColor : COLOR_RED;
            break;

        case L'<':
            *color = *color == COLOR_YELLOW ? originalColor : COLOR_YELLOW;
            break;

        case L'>':
            *color = *color == COLOR_GREEN ? originalColor : COLOR_GREEN;
            break;

        default:
            isSpecialChar = false;
            break;
    }
    return isSpecialChar;
}

systemFont::systemFont()
{
    uint64_t languageCode = 0;
    PlFontData sharedFont[6];

    //Get language code
    setGetLanguageCode(&languageCode);

    //Init freetype library
    FT_Init_FreeType(&m_Lib);

    //Get all shared fonts
    plGetSharedFont(languageCode, sharedFont, 6, &m_TotalFonts);

    //Loop and create new freetype faces for all of them
    for(int i = 0; i < m_TotalFonts; i++)
    {
        FT_New_Memory_Face(m_Lib, reinterpret_cast<FT_Byte *>(sharedFont[i].address), sharedFont[i].size, 0, &m_Faces[i]);
    }

    //Init texture cache for glyphs
    m_GlyphTextureManager = std::make_unique<textureManager>();
}

systemFont::~systemFont()
{
    //Free faces
    for(int i = 0; i < m_TotalFonts; i++)
    {
        FT_Done_Face(m_Faces[i]);
    }

    //Free lib
    FT_Done_FreeType(m_Lib);
}

void systemFont::renderText(SDL_Renderer *renderer, SDL_Texture *target, int fontSize, uint32_t color, int x, int y, const char *str)
{
    //Set render target
    SDL_SetRenderTarget(renderer, target);

    //Save orignal X position and color
    int workingX = x;
    uint32_t workingColor = color;
    uint32_t codepoint = 0;
    ssize_t unitCount = 0;
    size_t textLength = strlen(str);

    //Resize the font before processing
    resizeFont(fontSize);

    for(unsigned int i = 0; i < textLength; )
    {
        unitCount = decode_utf8(&codepoint, reinterpret_cast<const uint8_t *>(&str[i]));
        if(unitCount <= 0)
            break;
            
        //Add to i just in case char is special
        i += unitCount;
        if(processSpecialCharacters(codepoint, color, &workingColor))
            continue;

        else if(codepoint == '\n')
        {
            workingX = x;
            y += fontSize + 8;
            continue;
        }

        glyph *g = getGlyph(renderer, codepoint, fontSize);
        if(g != NULL)
        {
            SDL_Rect sourceRect = { 0, 0, g->width, g->height };
            SDL_Rect destinationRect = { workingX + g->left, y + (fontSize - g->top), g->width, g->height };
            SDL_SetTextureColorMod(g->tex, getRed(workingColor), getGreen(workingColor), getBlue(workingColor));
            SDL_RenderCopy(renderer, g->tex, &sourceRect, &destinationRect);
            workingX += g->advanceX;
        }
    }
}

void systemFont::renderTextWrap(SDL_Renderer *renderer, SDL_Texture *target, int fontSize, int x, int y, int maxWidth, uint32_t color, const char *str)
{
    SDL_SetRenderTarget(renderer, target);

    //Don't repeat code blah blah
    int workingX = x;
    char wordbuffer[128];
    size_t textlength = strlen(str), wordLength = 0;
    size_t nextBreakPoint = 0;
    ssize_t unitCount = 0;
    uint32_t codepoint = 0, workingColor = color;

    resizeFont(fontSize);

    for(unsigned int i = 0; i < textlength; )
    {
        nextBreakPoint = findNextBreakpoint(&str[i]);
        std::memset(wordbuffer, 0x00, 128);
        std::memcpy(wordbuffer, &str[i], nextBreakPoint);
        //Break the line if width is exceeded
        unsigned int wordWidth = getTextWidth(renderer, wordbuffer, fontSize);
        if((int)(workingX + wordWidth) >= (int)(x + maxWidth))
        {
            workingX = x;
            y += fontSize + 8;
        }
        
        //To do: For some strange reason, calling renderText instead of repeating results in a garbled mess.
        wordLength = strlen(wordbuffer);
        for(unsigned int j = 0; j < wordLength; )
        {
            unitCount = decode_utf8(&codepoint, reinterpret_cast<const uint8_t *>(&wordbuffer[j]));
            if(unitCount <= 0)
                break;

            j += unitCount;
            if(processSpecialCharacters(codepoint, color, &workingColor))
                continue;

            glyph *g = getGlyph(renderer, codepoint, fontSize);
            if(g != NULL)
            {
                SDL_Rect sourceRect = { 0, 0, g->width, g->height };
                SDL_Rect destinationRect = { workingX + g->left, y + (fontSize - g->top), g->width, g->height };
                SDL_SetTextureColorMod(g->tex, getRed(workingColor), getGreen(workingColor), getBlue(workingColor));
                SDL_RenderCopy(renderer, g->tex, &sourceRect, &destinationRect);
                workingX += g->advanceX;
            }
        }
        i += wordLength;
    }
}

int systemFont::getTextWidth(SDL_Renderer *renderer, std::string text, int fontSize)
{
    int textWidth = 0;
    uint32_t codepoint = 0, unitCount = 0, colorMod = 0;
    resizeFont(fontSize);
    for(unsigned int i = 0; i < text.length(); )
    {
        unitCount = decode_utf8(&codepoint, reinterpret_cast<const uint8_t *>(&text.c_str()[i]));
        if(unitCount <= 0)
            break;

        i += unitCount;
        if(processSpecialCharacters(codepoint, COLOR_WHITE, &colorMod))
            continue;

        glyph *g = getGlyph(renderer, codepoint, fontSize);
        if(g != NULL)
            textWidth += g->advanceX;
    }
    return textWidth;
}

void systemFont::resizeFont(int fontSize)
{
    for(int i = 0; i < m_TotalFonts; i++)
    {
        FT_Set_Char_Size(m_Faces[i], 0, fontSize * 64, 90, 90);
    }
}

FT_GlyphSlot systemFont::loadGlyph(uint32_t c, FT_Int32 flags)
{
    for(int i = 0; i < m_TotalFonts; i++)
    {
        FT_UInt cInd = 0;
        if( (cInd = FT_Get_Char_Index(m_Faces[i], c)) != 0 && FT_Load_Glyph(m_Faces[i], cInd, flags) == 0)
        {
            return m_Faces[i]->glyph;
        }
    }
    return NULL;
}

glyph *systemFont::getGlyph(SDL_Renderer *renderer, uint32_t c, int size)
{
    //If it's already loaded and rendered, just return it
    if(m_GlyphMap.find(std::make_pair(c, size)) != m_GlyphMap.end())
    {
        return &m_GlyphMap[std::make_pair(c, size)];
    }
    
    FT_GlyphSlot glyphSlot = loadGlyph(c, FT_LOAD_RENDER);
    FT_Bitmap bitmap = glyphSlot->bitmap;
    if(bitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
    {
        return NULL;
    }

    //Prepare buffer to convert to surface
    size_t bitmapSize = bitmap.rows * bitmap.width;
    uint8_t *bitmapPointer = reinterpret_cast<uint8_t *>(bitmap.buffer);
    uint32_t *glyphBuffer = new uint32_t[bitmapSize];
    uint32_t basePixel = 0xFFFFFF00;
    //Use bitmap values to fill out alpha of texture
    for(unsigned int i = 0; i < bitmapSize; i++)
    {
        glyphBuffer[i] = basePixel | *bitmapPointer++;
    }
    

    //Create SDL_Surface from buffer
    SDL_Surface *glyphSurface = SDL_CreateRGBSurfaceFrom(glyphBuffer, bitmap.width, bitmap.rows, 32, sizeof(uint32_t) * bitmap.width, redMask, greenMask, blueMask, alphaMask);
    std::string glyphName = std::to_string(c) + "_" + std::to_string(size);
    SDL_Texture *glyphTexture = m_GlyphTextureManager->textureCreateFromSurface(glyphName, renderer, glyphSurface);

    //Free stuff
    SDL_FreeSurface(glyphSurface);
    delete[] glyphBuffer;

    //Add it to glyph map
    m_GlyphMap[std::make_pair(c, size)] = {(uint16_t)bitmap.width, (uint16_t)bitmap.rows, (int)glyphSlot->advance.x >> 6, glyphSlot->bitmap_top, glyphSlot->bitmap_left, glyphTexture };

    //Return it now
    return &m_GlyphMap[std::make_pair(c, size)];
}