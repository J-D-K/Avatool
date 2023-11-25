#pragma once

#include "avatoolState.hpp"
#include "fs.hpp"
#include "menu.hpp"
#include "gfx.h"

class editState : public avatoolState
{
    public:
        editState();
        ~editState();

        void update(uint64_t padDown);
        void render(tex *framebuffer, font *f);

    private:
        const std::string targetDirPath = "acc:/su/avators/";
        const std::string sourceDirPath = "sdmc:/avatars/";
        int targetIndex = 0;
        tex *arrow = NULL, *targetIcon = NULL, *sourceIcon = NULL, *errorIcon = NULL;
        dirList targetDir, sourceDir;
        menu sourceMenu;
};