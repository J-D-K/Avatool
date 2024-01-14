#pragma once

#include "graphics.hpp"

class avatoolState
{
    public:
        avatoolState() { };
        virtual ~avatoolState() { };

        virtual void update();
        virtual void render();
};