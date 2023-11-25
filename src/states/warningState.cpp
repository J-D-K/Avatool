#include <string>
#include "states/warningState.hpp"
#include "states/editState.hpp"

static const std::string warningMessage = "*WARNING*: This tool works by shutting down the account service on your Switch. This *will* lead to an eventual crash.";
static const std::string bottomGuideText = "\ue0e0 Continue  \ue0ef Exit";

warningState::warningState(std::stack<avatoolState *> *appStateStack)
{
    this->outputConsole = new console(28);
    this->outputConsole->out(warningMessage);
    this->outputConsole->nl();
    this->appStateStack = appStateStack;
    printf("stack: %p\n", &this->appStateStack);
}

warningState::~warningState()
{
    delete this->outputConsole;
}

void warningState::update(uint64_t padDown)
{
    if(padDown & HidNpadButton_A && this->terminateAndMount())
    {
        this->appStateStack->pop();
        this->appStateStack->push(new editState());
        delete this;
    }
}

void warningState::render(tex *frameBuffer, font *f)
{
    this->outputConsole->draw(f);
    drawText(bottomGuideText.c_str(), frameBuffer, f, 1000, 673, 18, clrCreateU32(0xFFFFFFFF));
}

bool warningState::terminateAndMount()
{
    this->outputConsole->out("Attempting to shut down #account# and #olsc#... ");
    this->outputConsole->nl();
    Result accountShutdown = pmshellTerminateProgram(0x010000000000001E);
    Result olscShutdown = pmshellTerminateProgram(0x010000000000003E);
    if(R_FAILED(accountShutdown) || R_FAILED(olscShutdown))
    {
        this->outputConsole->out("Failed to terminate account and/or olsc processes.");
        this->outputConsole->nl();
        return false;
    }

    //Sleeping helps with mounting save filesystem
    svcSleepThread(1e+9);

    this->outputConsole->out("Attempting to mount #0x8000000000000010#... ");
    this->outputConsole->nl();
    FsFileSystem accountFilesystem;
    Result mountAccount = fsOpen_SystemSaveData(&accountFilesystem, FsSaveDataSpaceId_System, 0x8000000000000010, (AccountUid){0});
    if(R_FAILED(mountAccount))
    {
        this->outputConsole->out("Failed to mount account save filesystem.");
        this->outputConsole->nl();
        return false;
    }
    else
        fsdevMountDevice("acc", accountFilesystem);

    return true;
}