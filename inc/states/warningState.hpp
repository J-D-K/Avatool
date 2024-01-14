#pragma once
#include <string>
#include <stack>
#include "avatool.hpp"
#include "avatoolState.hpp"

class warningState : public avatoolState
{
    public:
        warningState(avatool *app);
        ~warningState();

        void update();
        void render();

    private:
        //Pointer to main app the access members
        avatool *m_App;
        //Terminates the services needed to read and write to account save data
        bool terminateAndMount();
        //Returns if there's actually avatar images on SD
        bool sdHasAvatars();
};