#include <string>
#include "states/warningState.hpp"
#include "states/editState.hpp"

static const std::string warningMessage = "*WARNING*: This tool works by shutting down the account service on your Switch. This *will* lead to an eventual crash. Press \ue0e0 to continue or \ue0ef to exit.";

warningState::warningState(graphics*gfx, avatoolStack *appStateStack) : gfx(gfx), appStateStack(appStateStack)
{

}

warningState::~warningState()
{
    
}

void warningState::update(const uint64_t& padDown)
{
    if(padDown & HidNpadButton_A && terminateAndMount())
    {
        this->appStateStack->push(new editState(gfx));
    }
}

void warningState::render(graphics *gfx)
{
    gfx->renderRectFill(NULL, COLOR_DIM_BACKGROUND, 0, 0, 1280, 720);
    gfx->renderRectFill(NULL, COLOR_DIALOG_BOX, 280, 180, 720, 360);
    gfx->renderTextfWrap(NULL, 18, COLOR_WHITE, 306, 196, 688, warningMessage.c_str());
}

bool warningState::terminateAndMount()
{
    Result accountShutdown = pmshellTerminateProgram(0x010000000000001E);
    Result olscShutdown = pmshellTerminateProgram(0x010000000000003E);
    if(R_FAILED(accountShutdown) || R_FAILED(olscShutdown))
    {
        return false;
    }

    //Sleeping helps with mounting save filesystem
    svcSleepThread(1e+9);

    FsFileSystem accountFilesystem;
    Result mountAccount = fsOpen_SystemSaveData(&accountFilesystem, FsSaveDataSpaceId_System, 0x8000000000000010, (AccountUid){0});
    if(R_FAILED(mountAccount))
    {
        return false;
    }
    else
    {
        fsdevMountDevice("acc", accountFilesystem);
    }

    return true;
}