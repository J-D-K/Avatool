#pragma once
#include <SDL2/SDL.h>
#include "avatoolState.hpp"
#include "fs.hpp"
#include "menu.hpp"
#include "graphics.hpp"

class editState : public avatoolState
{
    public:
        editState(graphics *gfx);
        ~editState();

        void update(const uint64_t& padDown);
        void render(graphics *gfx);

    private:
        int targetIndex = 0;
        SDL_Texture *arrow, *targetIcon, *sourceIcon, *errorIcon;
        dirList targetDir, sourceDir;
        menu sourceMenu;
        graphics *gfx;
};