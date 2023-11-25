#pragma once

#include "gfx.h"

class avatoolState
{
    public:
        avatoolState() { }
        virtual ~avatoolState() { };

        virtual void update(uint64_t padDown);
        virtual void render(tex *frameBuffer, font *f);
};