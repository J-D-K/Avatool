#pragma once
#include <string>
#include <stack>
#include "avatoolState.hpp"
#include "console.hpp"

class warningState : public avatoolState
{
    public:
        //Needs appStateStack to push edit state
        warningState(std::stack<avatoolState *> *appStateStack);
        ~warningState();

        void update(uint64_t padDown);
        void render(tex *framebuffer, font *f);

    private:
        bool terminateAndMount();
        console *outputConsole;
        std::stack<avatoolState *> *appStateStack;
};