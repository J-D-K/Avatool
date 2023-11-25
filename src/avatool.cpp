#include <switch.h>

#include "avatool.hpp"
#include "states/warningState.hpp"
#include "states/editState.hpp"

void debOutputFile(const char *name)
{
    FILE *deb = fopen(name, "w");
    fclose(deb);
}

avatool::avatool(int windowWidth, int windowHeight)
{
    //Init services + old JKSV gfx.c
    romfsInit();
    hidInitialize();
    pmshellInitialize();
    graphicsInit(1280, 720);

    //Load system font
    this->systemFont = fontLoadSharedFonts();

    //Init gamepad
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&this->gamepad);

    appStateStack.push(new warningState(&this->appStateStack));
    printf("appStateStack: %p\n", &this->appStateStack);

    this->running = true;
}

avatool::~avatool()
{
    //Free memory for any states left
    while(!this->appStateStack.empty())
    {
        delete this->appStateStack.top();
        this->appStateStack.pop();
    }
    printf("Free stack\n");

    //Free font
    fontDestroy(this->systemFont);
    printf("Free font\n");

    //Exit gfx + services
    graphicsExit();
    pmshellExit();
    hidExit();
    romfsExit();
    printf("Exit services\n");
}

void avatool::update()
{
    //printf("avatool::update\n");
    //Update gamepad, I only care about down for this app
    padUpdate(&this->gamepad);
    uint64_t gamePadDown = padGetButtonsDown(&this->gamepad);

    //Exit on +
    if(gamePadDown & HidNpadButton_Plus)
        this->running = false;

    //Update state at top of app stack
    this->appStateStack.top()->update(gamePadDown);
}

void avatool::render()
{
    //printf("avatool::render\n");
    gfxBeginFrame();
    this->renderBaseApp();
    appStateStack.top()->render(frameBuffer, this->systemFont);
    gfxEndFrame();
}

void avatool::renderBaseApp()
{
    texClearColor(frameBuffer, clrCreateU32(0xFF2D2D2D));
    drawText("Avatool", frameBuffer, this->systemFont, 64, 38, 24, clrCreateU32(0xFFFFFFFF));
    drawRect(frameBuffer, 30, 87, 1220, 1, clrCreateU32(0xFFFFFFFF));
    drawRect(frameBuffer, 30, 648, 1220, 1, clrCreateU32(0xFFFFFFFF));
}