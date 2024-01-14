#pragma once
#include <switch.h>
#include <memory>
#include <stack>
#include "avatoolState.hpp"
#include "graphics.hpp"
#include "type.hpp"

class avatool
{
    public:
        avatool();
        ~avatool();

        //For main loop
        bool isRunning() { return m_IsRunning; }
        //Updates input and top of app stack
        void update();
        //Renders top of app stack
        void render();

        //renders header and footer
        void renderBaseApp();

        //Input
        uint64_t padKeysDown() { return padGetButtonsDown(&m_Gamepad); }

        //Getters
        graphics *getGraphics() { return m_Graphics.get(); }
        std::stack<std::unique_ptr<avatoolState>> *getAppStack() { return &m_AppStateStack; }

    private:
        //Whether app is running
        bool m_IsRunning = false;
        //Graphics/rendering class
        std::unique_ptr<graphics> m_Graphics;
        //App main stack
        std::stack<std::unique_ptr<avatoolState>> m_AppStateStack;
        //Switch pad struct
        PadState m_Gamepad;
        //Inits m_Gamepad
        void inputInitialize();
        //Updates m_Gamepad
        void updateInput() { padUpdate(&m_Gamepad); }
};
