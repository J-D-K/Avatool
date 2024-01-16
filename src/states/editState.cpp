#include <switch.h>
#include <string>
#include "graphics/graphics.hpp"
#include "states/editState.hpp"

// Names of textures
static const std::string TEX_NAME_ARROW = "arrow";
static const std::string TEX_NAME_ERROR = "errorIcon";
static const std::string TEX_NAME_TARGET = "targetIcon";
static const std::string TEX_NAME_SOURCE = "sourceIcon";
// Paths to avatar images
static const std::string s_TargetDirPath = "acc:/su/avators/";
static const std::string s_SourceDirPath = "sdmc:/avatars/";
// Text at the bottom right
static const std::string s_BottomGuideText = "\ue0e4\ue0e5 Change Target  \ue0e3 Copy avatars to SD  \ue0e0 Overwrite";

// This should check more
static bool sourceIconIsGood(SDL_Texture *source)
{
    int width, height;
    SDL_QueryTexture(source, NULL, NULL, &width, &height);
    return width == 256 && height == 256;
}

editState::editState(avatool *app) : m_App(app)
{
    // Create directories/assign
    m_TargetDirectory = std::make_unique<directoryListing>(s_TargetDirPath);
    m_SourceDirectory = std::make_unique<directoryListing>(s_SourceDirPath);

    // Full paths to target and source
    std::string currentTargetIcon = s_TargetDirPath + m_TargetDirectory->getDirectoryItemAt(0);
    std::string currentSourceIcon = s_SourceDirPath + m_SourceDirectory->getDirectoryItemAt(0);

    // Load textures, target source icons
    m_Arrow = m_App->getGraphics()->textureLoadFromFile(TEX_NAME_ARROW, "romfs:/arrow.png");
    m_ErrorIcon = m_App->getGraphics()->textureLoadFromFile(TEX_NAME_ERROR, "romfs:/errorIcon.png");
    m_TargetIcon = m_App->getGraphics()->textureLoadFromFile(TEX_NAME_TARGET, currentTargetIcon.c_str());
    m_SourceIcon = m_App->getGraphics()->textureLoadFromFile(TEX_NAME_SOURCE, currentSourceIcon.c_str());

    // Setup sourceMenu
    m_SourceMenu = std::make_unique<menu>(4, 4, 1212, 18, 3);
    for (int i = 0; i < m_SourceDirectory->getDirectoryListingCount(); i++)
    {
        m_SourceMenu->addOpt(m_SourceDirectory->getDirectoryItemAt(i));
    }

    //Create render target for menu
    m_MenuRenderTarget = m_App->getGraphics()->textureCreate("menuRenderTarget", 1280, 412, SDL_TEXTUREACCESS_STATIC | SDL_TEXTUREACCESS_TARGET);

    // Calculate the X of the button guide on the bottom
    m_ButtonGuideX = 1220 - (m_App->getGraphics()->getTextWidth(s_BottomGuideText, 18));
}

editState::~editState() {}

void editState::update()
{
    // Grab buttons pushed
    uint64_t padDown = m_App->padKeysDown();

    // Update menu, and if changed, load new source JPEG
    bool menuChanged = m_SourceMenu->update(padDown);
    if (menuChanged)
    {
        // For readability
        int sourceMenuSelected = m_SourceMenu->getSelected();
        std::string newSourceIconName = m_SourceDirectory->getDirectoryItemAt(sourceMenuSelected);
        std::string newSourceIconPath = s_SourceDirPath + newSourceIconName;
        m_SourceIcon = m_App->getGraphics()->textureLoadFromFile(TEX_NAME_SOURCE, newSourceIconPath.c_str());
    }

    // Perform actions for button presses
    if ((padDown & HidNpadButton_A) && sourceIconIsGood(m_SourceIcon))
    {
        // Setup
        int sourceMenuSelected = m_SourceMenu->getSelected();
        std::string sourceIconPath = s_SourceDirPath + m_SourceDirectory->getDirectoryItemAt(sourceMenuSelected);
        std::string targetIconPath = s_TargetDirPath + m_TargetDirectory->getDirectoryItemAt(m_CurrentTarget);

        // Copy file, then commit changes to device
        copyFile(sourceIconPath, targetIconPath);
        fsdevCommitDevice("acc");

        // Reload target to see changes
        m_TargetIcon = m_App->getGraphics()->textureLoadFromFile(TEX_NAME_TARGET, targetIconPath.c_str());
    }
    else if (padDown & HidNpadButton_Y)
    {
        // Copy all to SD card
        copyDirectory(s_TargetDirPath, s_SourceDirPath);

        // Reset menu, source directory
        m_SourceMenu->reset();
        m_SourceDirectory->reload();
        for (int i = 0; i < m_SourceDirectory->getDirectoryListingCount(); i++)
        {
            m_SourceMenu->addOpt(m_SourceDirectory->getDirectoryItemAt(i));
        }
    }
    else if (padDown & HidNpadButton_L)
    {
        // Check to wrap
        if (--m_CurrentTarget < 0)
        {
            m_CurrentTarget = m_TargetDirectory->getDirectoryListingCount() - 1;
        }

        // Load new target
        std::string newTargetPath = s_TargetDirPath + m_TargetDirectory->getDirectoryItemAt(m_CurrentTarget);
        m_TargetIcon = m_App->getGraphics()->textureLoadFromFile(TEX_NAME_TARGET, newTargetPath.c_str());
    }
    else if (padDown & HidNpadButton_R)
    {
        // Check to not go out of bounds
        if (++m_CurrentTarget > m_TargetDirectory->getDirectoryListingCount() - 1)
        {
            m_CurrentTarget = 0;
        }

        // Load new target
        std::string newTargetPath = s_TargetDirPath + m_TargetDirectory->getDirectoryItemAt(m_CurrentTarget);
        m_TargetIcon = m_App->getGraphics()->textureLoadFromFile(TEX_NAME_TARGET, newTargetPath.c_str());
    }
}

void editState::render()
{
    // Draw Target -> Source
    m_App->getGraphics()->renderTextureStretched(m_TargetIcon, NULL, 436, 98, 128, 128);
    m_App->getGraphics()->renderTexture(m_Arrow, NULL, 572, 98);
    if (sourceIconIsGood(m_SourceIcon))
    {
        m_App->getGraphics()->renderTextureStretched(m_SourceIcon, NULL, 708, 98, 128, 128);
    }
    else
    {
        m_App->getGraphics()->renderTextureStretched(m_ErrorIcon, NULL, 708, 98, 128, 128);
    }

    //Clear menu target
    m_App->getGraphics()->clearTexture(m_MenuRenderTarget, COLOR_TRANSPARENT);
    // Draw menu
    m_SourceMenu->render(m_App->getGraphics(), m_MenuRenderTarget);
    //Render target to screen
    m_App->getGraphics()->renderTexture(m_MenuRenderTarget, NULL, 30, 234);
    //Control guide
    m_App->getGraphics()->renderTextf(NULL, 18, COLOR_WHITE, m_ButtonGuideX, 673, s_BottomGuideText.c_str());
}