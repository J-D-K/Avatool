#include <string>
#include "states/warningState.hpp"
#include "states/editState.hpp"

static const std::string s_WarningMessage = "*WARNING*: This tool works by shutting down the account service on your Switch. This *will* lead to an eventual crash. Avatool will also not start unless it finds images in your #sdmc:/avatars/# folder. Press \ue0e0 to continue or \ue0ef to exit.";

warningState::warningState(avatool *app) : m_App(app) {}

warningState::~warningState() {}

void warningState::update()
{
    uint64_t padDown = m_App->padKeysDown();

    if (padDown & HidNpadButton_A && terminateAndMount())
    {
        m_App->getAppStack()->push(std::make_unique<editState>(m_App));
    }
}

void warningState::render()
{
    m_App->getGraphics()->renderRectFill(NULL, COLOR_DIM_BACKGROUND, 0, 0, 1280, 720);
    m_App->getGraphics()->renderRectFill(NULL, COLOR_DIALOG_BOX, 280, 180, 720, 360);
    m_App->getGraphics()->renderTextfWrap(NULL, 18, COLOR_WHITE, 306, 196, 688, s_WarningMessage.c_str());
}

bool warningState::terminateAndMount()
{
    if(!sdHasAvatars())
    {
        return false;
    }

    Result accountShutdown = pmshellTerminateProgram(0x010000000000001E);
    Result olscShutdown = pmshellTerminateProgram(0x010000000000003E);
    if (R_FAILED(accountShutdown) || R_FAILED(olscShutdown))
    {
        return false;
    }

    // Sleeping helps with mounting save filesystem
    svcSleepThread(1e+9);

    FsFileSystem accountFilesystem;
    Result mountAccount = fsOpen_SystemSaveData(&accountFilesystem, FsSaveDataSpaceId_System, 0x8000000000000010, (AccountUid){0});
    if (R_FAILED(mountAccount))
    {
        return false;
    }
    else
    {
        fsdevMountDevice("acc", accountFilesystem);
    }

    return true;
}

bool warningState::sdHasAvatars()
{
    directoryListing checkSD("sdmc:/avatars/");
    if(checkSD.getDirectoryListingCount() > 0)
    {
        return true;
    }
    return false;
}