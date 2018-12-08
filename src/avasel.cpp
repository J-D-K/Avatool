#include <switch.h>

#include "dir.h"
#include "menu.h"
#include "avasel.h"
#include "gfx.h"

dirList acc, sd;
menu    avaMenu;
tex *target, *source, *arrow;

extern font *shared;

void avaSelPrep()
{
    acc.assign("account:/su/avators/");
    sd.assign("sdmc:/avatar/");

    arrow  = texLoadPNGFile("romfs:/arrow.png");
    target = texLoadJPEGFile(std::string("account:/su/avators/" + acc.getItem(0)).c_str());
    source = texLoadJPEGFile(std::string("sdmc:/avatar/" + sd.getItem(0)).c_str());

    avaMenu.setParams(30, 234, 1220);
    for(unsigned i = 0; i < sd.getCount(); i++)
        avaMenu.addOpt(sd.getItem(i));

    //burn controls into framebuffer
    drawText("L/R = Change Target. A = Overwrite", frameBuffer, shared, 800, 672, 18);
}

void avaSel(const uint64_t& down)
{
    static int trgtInd = 0;

    //cheat
    drawRect(frameBuffer, 30, 88, 1220, 559, clrCreateU32(0xFF2D2D2D));
    texDrawSkipNoAlpha(target, frameBuffer, 436, 98);
    texDrawNoAlpha(arrow, frameBuffer, 572, 98);
    texDrawSkipNoAlpha(source, frameBuffer, 708, 98);

    bool ch = avaMenu.handleInput(down);
    if(ch)
    {
        texDestroy(source);
        source = texLoadJPEGFile(std::string("sdmc:/avatar/" + sd.getItem(avaMenu.getSelected())).c_str());
    }

    if(down & KEY_A)
    {
        std::string jpgIn = "sdmc:/avatar/" + sd.getItem(avaMenu.getSelected());
        std::string trgJpg = "account:/su/avators/" + acc.getItem(trgtInd);

        copyFile(jpgIn, trgJpg);
        fsdevCommitDevice("account");

        //Reload target to make sure it stuck
        texDestroy(target);
        target = texLoadJPEGFile(trgJpg.c_str());
    }
    else if(down & KEY_L)
    {
        trgtInd--;
        if(trgtInd < 0)
            trgtInd = acc.getCount() - 1;

        texDestroy(target);
        target = texLoadJPEGFile(std::string("account:/su/avators/" + acc.getItem(trgtInd)).c_str());
    }
    else if(down & KEY_R)
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
}
