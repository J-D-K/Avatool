#include <switch.h>
#include <sys/stat.h>

#include "avatool.hpp"
#include "states/warningState.hpp"
#include "states/editState.hpp"

avatool::avatool()
{
    //Init services
    romfsInit();
    hidInitialize();
    pmshellInitialize();
    plInitialize(PlServiceType_User);

    mkdir("sdmc:/avatars/", 0777);

    //Init SDL for video
    SDL_Init(SDL_INIT_VIDEO);

    //Takes care of window/renderer and has all rendering drawing functions
    gfx = std::make_unique<graphics>();

    //App stack
    appStateStack = std::make_unique<avatoolStack>();

    //Init gamepad
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&gamepad);

    //Push initial warning
    appStateStack->push(new warningState(gfx.get(), appStateStack.get()));

    running = true;
}

avatool::~avatool()
{
    //Free memory for any states left
    while(!appStateStack->empty())
    {
        delete appStateStack->top();
        appStateStack->pop();
    }

    //Exit gfx + services
    plExit();
    pmshellExit();
    hidExit();
    romfsExit();
    SDL_Quit();
}

void avatool::update()
{
    //Update gamepad, I only care about down for this app
    padUpdate(&gamepad);
    uint64_t gamePadDown = padGetButtonsDown(&gamepad);

    //Exit on +
    if(gamePadDown & HidNpadButton_Plus)
    {
        running = false;
    }

    //Update state at top of app stack
    appStateStack->top()->update(gamePadDown);
}

void avatool::render()
{
    gfx->beginFrame(COLOR_DEFAULT_CLEAR);
    renderBaseApp();
    appStateStack->top()->render(gfx.get());
    gfx->endFrame();
}

void avatool::renderBaseApp()
{
    gfx->renderTextf(NULL, 38, COLOR_WHITE, 64, 24, "Avatool");
    gfx->renderLine(NULL, COLOR_WHITE, 30, 87, 1220, 87);
    gfx->renderLine(NULL, COLOR_WHITE, 30, 648, 1220, 648);
}