#pragma once
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

        bool isRunning() { return running; }

        void update();
        void render();
        void renderBaseApp();

    private:
        bool running = false;
        std::unique_ptr<graphics> gfx;
        //Defined in type.hpp
        std::unique_ptr<avatoolStack> appStateStack;
        PadState gamepad;
};
