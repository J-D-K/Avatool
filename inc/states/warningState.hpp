#pragma once
#include <string>
#include <stack>
#include "avatoolState.hpp"
#include "type.hpp"

class warningState : public avatoolState
{
    public:
        //Needs appStateStack to push edit state
        warningState(graphics * gfx, avatoolStack *appStateStack);
        ~warningState();

        void update(const uint64_t& padDown);
        void render(graphics *gfx);

    private:
        bool terminateAndMount();
        graphics *gfx;
        avatoolStack *appStateStack;
};