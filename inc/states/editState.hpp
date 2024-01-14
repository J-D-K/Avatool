#pragma once
#include <memory>
#include <SDL2/SDL.h>
#include "avatool.hpp"
#include "avatoolState.hpp"
#include "fs.hpp"
#include "filesystem/directoryListing.hpp"
#include "ui/menu.hpp"

class editState : public avatoolState
{
    public:
        editState(avatool *app);
        ~editState();

        void update();
        void render();

    private:
        //Pointer to app for access
        avatool *m_App;
        //Directory listings
        std::unique_ptr<directoryListing> m_SourceDirectory, m_TargetDirectory;
        //Source menu
        std::unique_ptr<menu> m_SourceMenu;
        //Textures needed
        SDL_Texture *m_Arrow, *m_TargetIcon, *m_SourceIcon, *m_ErrorIcon, *m_MenuRenderTarget;
        //Current target in target directory
        int m_CurrentTarget = 0;
        //Position of the control text in the bottom right
        int m_ButtonGuideX = 0;
};