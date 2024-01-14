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
    m_Graphics = std::make_unique<graphics>();

    //Init gamepad
    inputInitialize();

    //Push initial warning
    m_AppStateStack.push(std::make_unique<warningState>(this));

    //We're running
    m_IsRunning = true;
}

avatool::~avatool()
{
    //Free memory for any states left
    while(!m_AppStateStack.empty())
    {
        m_AppStateStack.pop();
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
    //Update gamepad.
    updateInput();

    //Exit on +
    if(padKeysDown() & HidNpadButton_Plus)
    {
        m_IsRunning = false;
    }

    //Update state at top of app stack
    m_AppStateStack.top()->update();
}

void avatool::render()
{
    m_Graphics->beginFrame(COLOR_DEFAULT_CLEAR);
    renderBaseApp();
    m_AppStateStack.top()->render();
    m_Graphics->endFrame();
}

void avatool::renderBaseApp()
{
    m_Graphics->renderTextf(NULL, 38, COLOR_WHITE, 64, 24, "Avatool");
    m_Graphics->renderLine(NULL, COLOR_WHITE, 30, 87, 1220, 87);
    m_Graphics->renderLine(NULL, COLOR_WHITE, 30, 648, 1220, 648);
}

void avatool::inputInitialize()
{
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&m_Gamepad);
}