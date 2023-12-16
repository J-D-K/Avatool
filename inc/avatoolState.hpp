#pragma once

#include "graphics.hpp"

class avatoolState
{
    public:
        avatoolState() { }
        virtual ~avatoolState() { };

        virtual void update(const uint64_t& padDown);
        virtual void render(graphics *gfx);
};