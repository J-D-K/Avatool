#pragma once

#include <stack>
#include "avatoolState.hpp"
#include "gfx.h"

class avatool
{
    public:
        avatool(int windowWidth, int windowHeight);
        ~avatool();

        bool isRunning() { return running; }

        void update();
        void render();
        void renderBaseApp();

    private:
        bool running = false;
        font *systemFont = NULL;
        PadState gamepad;
        std::stack<avatoolState *> appStateStack;
};
