#include <switch.h>
#include <string>
#include "states/editState.hpp"

static const std::string bottomGuide = "\ue0e4\ue0e5 Change Target  \ue0e0 Overwrite";

//This should check more
static bool sourceIconIsGood(tex *sourceIcon)
{
    return sourceIcon->width == 256 && sourceIcon->height == 256;
}

editState::editState()
{
    printf("editState constructor\n");
    //Assign directories
    this->targetDir.assign(this->targetDirPath);
    this->sourceDir.assign(this->sourceDirPath);
    std::string currentTargetIcon = this->targetDirPath + this->targetDir.getItem(0);
    std::string currentSourceIcon = this->sourceDirPath + this->sourceDir.getItem(0); 
    printf("Assign dirs\n");

    //Load textures, target source icons
    this->arrow = texLoadPNGFile("romfs:/arrow.png");
    this->errorIcon = texLoadPNGFile("romfs:/nope.png");
    this->targetIcon = texLoadJPEGFile(currentTargetIcon.c_str());
    this->sourceIcon = texLoadJPEGFile(currentSourceIcon.c_str());
    printf("Load textures\n");

    //Setup sourceMenu
    sourceMenu.setParams(30, 234, 1220);
    for(unsigned i = 0; i < this->sourceDir.getCount(); i++)
        sourceMenu.addOpt(this->sourceDir.getItem(i));
    printf("Setup sourceMenu\n");
}

editState::~editState()
{
    texDestroy(this->targetIcon);
    texDestroy(this->sourceIcon);
    texDestroy(this->arrow);
    texDestroy(this->errorIcon);
}

void editState::update(uint64_t padDown)
{   
    //Update menu, and if changed, load new source JPEG
    bool menuChanged = this->sourceMenu.handleInput(padDown);
    if(menuChanged)
    {
        //For readability
        int sourceMenuSelected = this->sourceMenu.getSelected();
        std::string newSourceIconPath = this->sourceDir.getItem(sourceMenuSelected);
        std::string newSourceIcon = this->sourceDirPath + newSourceIconPath;
        texDestroy(this->sourceIcon);
        this->sourceIcon = texLoadJPEGFile(newSourceIcon.c_str());
    }

    //Perform actions for button presses
    if((padDown & HidNpadButton_A) && sourceIconIsGood(this->sourceIcon))
    {
        //Setup
        int sourceMenuSelected = this->sourceMenu.getSelected();
        std::string sourceIconPath = this->sourceDirPath + this->sourceDir.getItem(sourceMenuSelected);
        std::string targetIconPath = this->targetDirPath + this->targetDir.getItem(this->targetIndex);

        //Copy file, then commit changes to device
        copyFile(sourceIconPath, targetIconPath);
        fsdevCommitDevice("acc");

        //Reload target to see changes
        texDestroy(this->targetIcon);
        this->targetIcon = texLoadJPEGFile(targetIconPath.c_str());
    }
    else if(padDown & HidNpadButton_Y)
    {
        //Copy all to SD card
        copyDirToDir(this->targetDirPath, this->sourceDirPath);

        //Reset menu, source directory
        this->sourceMenu.reset();
        this->sourceDir.rescan();
        for(unsigned i = 0; i < this->sourceDir.getCount(); i++)
            this->sourceMenu.addOpt(this->sourceDir.getItem(i));
    }
    else if(padDown & HidNpadButton_L)
    {
        //Check to wrap
        if(--this->targetIndex < 0)
            this->targetIndex = this->targetDir.getCount() - 1;

        //Load new target
        std::string newTargetPath = this->targetDirPath + this->targetDir.getItem(this->targetIndex);
        texDestroy(this->targetIcon);
        this->targetIcon = texLoadJPEGFile(newTargetPath.c_str());
    }
    else if(padDown & HidNpadButton_R)
    {
        //Check to not go out of bounds
        if(++this->targetIndex > (int)this->targetDir.getCount() - 1)
            this->targetIndex = 0;

        //Load new target
        std::string newTargetPath = this->targetDirPath + this->targetDir.getItem(this->targetIndex);
        texDestroy(this->targetIcon);
        this->targetIcon = texLoadJPEGFile(newTargetPath.c_str());
    }
}

void editState::render(tex *frameBuffer, font *f)
{
    //Draw Target -> Source
    texDrawSkipNoAlpha(this->targetIcon, frameBuffer, 436, 98);
    texDrawNoAlpha(this->arrow, frameBuffer, 572, 98);
    if(sourceIconIsGood(this->sourceIcon))
        texDrawSkipNoAlpha(this->sourceIcon, frameBuffer, 708, 98);
    else
        texDrawSkipNoAlpha(this->errorIcon, frameBuffer, 708, 98);
    
    //Draw menu
    this->sourceMenu.draw(clrCreateU32(0xFFFFFFFF), f);
    drawText(bottomGuide.c_str(), frameBuffer, f, 840, 673, 18, clrCreateU32(0xFFFFFFFF));
}