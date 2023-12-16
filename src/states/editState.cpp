#include <switch.h>
#include <string>
#include "graphics.hpp"
#include "states/editState.hpp"

//Names of textures
static const std::string TEX_NAME_ARROW = "arrow";
static const std::string TEX_NAME_ERROR = "errorIcon";
static const std::string TEX_NAME_TARGET = "targetIcon";
static const std::string TEX_NAME_SOURCE = "sourceIcon";
//Paths to avatar images
static const std::string targetDirPath = "acc:/su/avators/";
static const std::string sourceDirPath = "sdmc:/avatars/";
//Text at the bottom right
static const std::string bottomGuide = "\ue0e4\ue0e5 Change Target  \ue0e3 Copy avatars to SD  \ue0e0 Overwrite";

//This should check more
static bool sourceIconIsGood(SDL_Texture *source)
{
    int width, height;
    SDL_QueryTexture(source, NULL, NULL, &width, &height);
    return width == 256 && height == 256;
}

editState::editState(graphics *gfx) : gfx(gfx)
{
    //Assign directories
    targetDir.assign(targetDirPath);
    sourceDir.assign(sourceDirPath);
    std::string currentTargetIcon = targetDirPath + targetDir.getItem(0);
    std::string currentSourceIcon = sourceDirPath + sourceDir.getItem(0); 

    //Load textures, target source icons
    arrow = gfx->textureLoadFromFile(TEX_NAME_ARROW, "romfs:/arrow.png");
    errorIcon = gfx->textureLoadFromFile(TEX_NAME_ERROR, "romfs:/errorIcon.png");
    targetIcon = gfx->textureLoadFromFile(TEX_NAME_TARGET, currentTargetIcon.c_str());
    sourceIcon = gfx->textureLoadFromFile(TEX_NAME_SOURCE, currentSourceIcon.c_str());

    //Setup sourceMenu
    sourceMenu.setParams(30, 234, 1220);
    for(unsigned i = 0; i < sourceDir.getCount(); i++)
        sourceMenu.addOpt(sourceDir.getItem(i));

    //Calculate the X of the button guide on the bottom
    buttonGuideX = 1220 - (gfx->getTextWidth(bottomGuide, 18));
}

editState::~editState()
{

}

void editState::update(const uint64_t& padDown)
{   
    //Update menu, and if changed, load new source JPEG
    bool menuChanged = sourceMenu.handleInput(padDown);
    if(menuChanged)
    {
        //For readability
        int sourceMenuSelected = sourceMenu.getSelected();
        std::string newSourceIconName = sourceDir.getItem(sourceMenuSelected);
        std::string newSourceIconPath = sourceDirPath + newSourceIconName;
        sourceIcon = gfx->textureLoadFromFile(TEX_NAME_SOURCE, newSourceIconPath.c_str());
    }

    //Perform actions for button presses
    if((padDown & HidNpadButton_A) && sourceIconIsGood(sourceIcon))
    {
        //Setup
        int sourceMenuSelected = sourceMenu.getSelected();
        std::string sourceIconPath = sourceDirPath + sourceDir.getItem(sourceMenuSelected);
        std::string targetIconPath = targetDirPath + targetDir.getItem(targetIndex);

        //Copy file, then commit changes to device
        copyFile(sourceIconPath, targetIconPath);
        fsdevCommitDevice("acc");

        //Reload target to see changes
        targetIcon = gfx->textureLoadFromFile(TEX_NAME_TARGET, targetIconPath.c_str());
    }
    else if(padDown & HidNpadButton_Y)
    {
        //Copy all to SD card
        copyDirToDir(targetDirPath, sourceDirPath);

        //Reset menu, source directory
        sourceMenu.reset();
        sourceDir.rescan();
        for(unsigned i = 0; i < sourceDir.getCount(); i++)
            sourceMenu.addOpt(sourceDir.getItem(i));
    }
    else if(padDown & HidNpadButton_L)
    {
        //Check to wrap
        if(--targetIndex < 0)
            targetIndex = targetDir.getCount() - 1;

        //Load new target
        std::string newTargetPath = targetDirPath + targetDir.getItem(targetIndex);
        targetIcon = gfx->textureLoadFromFile(TEX_NAME_TARGET ,newTargetPath.c_str());
    }
    else if(padDown & HidNpadButton_R)
    {
        //Check to not go out of bounds
        if(++targetIndex > (int)targetDir.getCount() - 1)
            targetIndex = 0;

        //Load new target
        std::string newTargetPath = targetDirPath + targetDir.getItem(targetIndex);
        targetIcon = gfx->textureLoadFromFile(TEX_NAME_TARGET, newTargetPath.c_str());
    }
}

void editState::render(graphics *gfx)
{
    //Draw Target -> Source
    gfx->renderTextureStretched(targetIcon, NULL, 436, 98, 128, 128);
    gfx->renderTexture(arrow, NULL, 572, 98);
    if(sourceIconIsGood(sourceIcon))
    {
        gfx->renderTextureStretched(sourceIcon, NULL, 708, 98, 128, 128);
    }
    else
    {
        gfx->renderTextureStretched(errorIcon, NULL, 708, 98, 128, 128);
    }
    
    //Draw menu
    sourceMenu.draw(gfx, COLOR_WHITE);
    gfx->renderTextf(NULL, 18, COLOR_WHITE, buttonGuideX, 673, bottomGuide.c_str());
}