#include <switch.h>

#include "dir.h"
#include "menu.h"
#include "avasel.h"
#include "gfx.h"
#include "cons.h"

dirList acc, sd;
menu    avaMenu;
tex *target, *source, *arrow, *nope;

extern font *shared;
extern console info;
static bool isValid = false;

static inline bool sourceIsGood()
{
    return source->width == 256 && source->height == 256;
}

void avaSelPrep()
{
    acc.assign("account:/su/avators/");
    sd.assign("sdmc:/avatar/");

    arrow  = texLoadPNGFile("romfs:/arrow.png");
    target = texLoadJPEGFile(std::string("account:/su/avators/" + acc.getItem(0)).c_str());
    source = texLoadJPEGFile(std::string("sdmc:/avatar/" + sd.getItem(0)).c_str());
    nope = texLoadPNGFile("romfs:/nope.png");

    if(sourceIsGood())
        isValid = true;

    avaMenu.setParams(30, 234, 1220);
    for(unsigned i = 0; i < sd.getCount(); i++)
        avaMenu.addOpt(sd.getItem(i));
}

void avaSel(const uint64_t& down)
{
    static int trgtInd = 0;

    //cheat
    drawRect(frameBuffer, 30, 88, 1220, 559, clrCreateU32(0xFF2D2D2D));
    texDrawSkipNoAlpha(target, frameBuffer, 436, 98);
    texDrawNoAlpha(arrow, frameBuffer, 572, 98);

    if(sourceIsGood())
        texDrawSkipNoAlpha(source, frameBuffer, 708, 98);
    else
        texDrawSkipNoAlpha(nope, frameBuffer, 708, 98);

    bool ch = avaMenu.handleInput(down);
    if(ch)
    {
        texDestroy(source);
        source = texLoadJPEGFile(std::string("sdmc:/avatar/" + sd.getItem(avaMenu.getSelected())).c_str());
    }

    if(down & HidNpadButton_A && sourceIsGood())
    {
        std::string jpgIn = "sdmc:/avatar/" + sd.getItem(avaMenu.getSelected());
        std::string trgJpg = "account:/su/avators/" + acc.getItem(trgtInd);

        copyFile(jpgIn, trgJpg);
        fsdevCommitDevice("account");

        //Reload target to make sure it stuck
        texDestroy(target);
        target = texLoadJPEGFile(trgJpg.c_str());
    }
    else if(down & HidNpadButton_Y)
    {
        //Copy all
        copyDirToDir("account:/su/avators/", "sdmc:/avatar/");

        //Reset and reinit menu
        avaMenu.reset();
        sd.rescan();
        for(unsigned i = 0; i < sd.getCount(); i++)
            avaMenu.addOpt(sd.getItem(i));
    }
    else if(down & HidNpadButton_L)
    {
        trgtInd--;
        if(trgtInd < 0)
            trgtInd = acc.getCount() - 1;

        texDestroy(target);
        target = texLoadJPEGFile(std::string("account:/su/avators/" + acc.getItem(trgtInd)).c_str());
    }
    else if(down & HidNpadButton_R)
    {
        trgtInd++;
        if(trgtInd > (int)acc.getCount() - 1)
            trgtInd = 0;

        texDestroy(target);
        target = texLoadJPEGFile(std::string("account:/su/avators/" + acc.getItem(trgtInd)).c_str());
    }

    avaMenu.draw(clrCreateU32(0xFFFFFFFF));
}

void avaSelClean()
{
    texDestroy(target);
    texDestroy(source);
    texDestroy(arrow);
    texDestroy(nope);
}
