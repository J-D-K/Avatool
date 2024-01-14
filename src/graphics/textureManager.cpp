#include <algorithm>
#include <SDL2/SDL_image.h>
#include "graphics/textureManager.hpp"

static SDL_Texture *loadJPEGMem(SDL_Renderer *renderer, const byte *data, size_t dataSize)
{
    SDL_Texture *ret = NULL;
    SDL_RWops   *jpg = SDL_RWFromConstMem(data, dataSize);
    SDL_Surface *surface = IMG_LoadJPG_RW(jpg);
    if(surface)
    {
        ret = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    SDL_RWclose(jpg);
    return ret;
}

static SDL_Texture *loadPNGMem(SDL_Renderer *renderer, const byte *data, size_t dataSize)
{
    SDL_Texture *ret = NULL;
    SDL_RWops   *png = SDL_RWFromConstMem(data, dataSize);
    SDL_Surface *surface = IMG_LoadPNG_RW(png);
    if(surface)
    {
        ret = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    SDL_RWclose(png);
    return ret;
}

textureManager::~textureManager()
{
    for(auto& t : m_TextureMap)
    {
        SDL_DestroyTexture(t.second);
    }
}

SDL_Texture *textureManager::textureCreate(std::string textureName, SDL_Renderer *renderer, int width, int height, int textureAccess)
{
    //Destroy the texture and recreate it in this case
    if(textureIsLoaded(textureName))
    {
        SDL_DestroyTexture(m_TextureMap[textureName]);
    }

    SDL_Texture *ret = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, textureAccess, width, height);
    if(ret)
    {
        SDL_SetTextureBlendMode(ret, SDL_BLENDMODE_BLEND);
        m_TextureMap[textureName] = ret;
    }
    return ret;
}

SDL_Texture *textureManager::textureCreateFromSurface(std::string textureName, SDL_Renderer *renderer, SDL_Surface *surface)
{
    SDL_Texture *ret = SDL_CreateTextureFromSurface(renderer, surface);
    if(ret)
    {
        SDL_SetTextureBlendMode(ret, SDL_BLENDMODE_BLEND);
        m_TextureMap[textureName] = ret;
    }
    return ret;
}

SDL_Texture *textureManager::textureLoadFromFile(std::string textureName, SDL_Renderer *renderer, const char *path)
{
    //Avatool differs here from JKSV since I'm swapping icons
    if(textureIsLoaded(textureName))
    {
        SDL_DestroyTexture(m_TextureMap[textureName]);
    }

    SDL_Texture *ret = NULL;
    SDL_Surface *surface = IMG_Load(path);
    ret = SDL_CreateTextureFromSurface(renderer, surface);
    if(ret)
    {
        SDL_SetTextureBlendMode(ret, SDL_BLENDMODE_BLEND);
        m_TextureMap[textureName] = ret;
    }
    SDL_FreeSurface(surface);
    return ret;
}

SDL_Texture *textureManager::textureLoadFromMem(std::string textureName, SDL_Renderer *renderer, imageTypes imgType, const byte *data, size_t dataSize)
{
    if(textureIsLoaded(textureName))
    {
        return m_TextureMap[textureName];
    }

    SDL_Texture *ret = NULL;
    switch(imgType)
    {
        case IMG_FMT_JPG:
            ret = loadJPEGMem(renderer, data, dataSize);
            break;

        case IMG_FMT_PNG:
            ret = loadPNGMem(renderer, data, dataSize);
            break;
    }

    if(ret)
    {
        SDL_SetTextureBlendMode(ret, SDL_BLENDMODE_BLEND);
        m_TextureMap[textureName] = ret;
    }
    return ret;
}

bool textureManager::textureIsLoaded(std::string textureName)
{
    if(m_TextureMap.find(textureName) != m_TextureMap.end())
    {
        return true;
    }
    return false;
}