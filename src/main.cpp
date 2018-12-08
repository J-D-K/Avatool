#include <switch.h>
#include <stdio.h>
#include <sys/stat.h>

#include "gfx.h"
#include "cons.h"
#include "dir.h"
#include "avasel.h"

console info(28);
font *shared;

bool shutdownMount()
{
    info.out(shared, "Attempting to shut down %account%... ");
    if(R_SUCCEEDED(pmshellTerminateProcessByTitleId(0x010000000000001E)))
    {
        info.out(shared, "^Succeeded^!");
        info.nl();

        info.out(shared, "Attempting to mount #0x8000000000000010#... ");
        FsFileSystem acc;
        if(R_SUCCEEDED(fsMount_SystemSaveData(&acc, 0x8000000000000010)))
        {
            info.out(shared, "^Succeeded^!");
            info.nl();
            fsdevMountDevice("account", acc);
            return true;
        }
        else
        {
            info.out(shared, "*Failed*.");
            info.nl();
        }
    }
    else
    {
        info.out(shared, "*Failed*");
        info.nl();
    }

    return false;
}

int main(int argc, const char *argv[])
{
    hidInitialize();
    pmshellInitialize();
    romfsInit();
    graphicsInit(1280, 720);

    shared = fontLoadSharedFonts();

    bool success = false;

    texClearColor(frameBuffer, clrCreateU32(0xFF2D2D2D));
    drawText("Avatool", frameBuffer, shared, 64, 38, 24);
    drawRect(frameBuffer, 30, 87, 1220, 1, clrCreateU32(0xFFFFFFFF));
    drawRect(frameBuffer, 30, 648, 1220, 1, clrCreateU32(0xFFFFFFFF));

    info.out(shared, "*WARNING*: This tool works by shutting down the %account% service on your Switch. This *will* lead to an eventual crash.");
    info.nl();
    info.out(shared, "Press A to continue. + to exit.");
    info.nl();

    while(appletMainLoop())
    {
        hidScanInput();

        uint64_t down = hidKeysDown(CONTROLLER_P1_AUTO);

        if(!success)
        {
            if(down & KEY_A && (success = shutdownMount()))
                avaSelPrep();
        }
        else
        {
            avaSel(down);
        }

        if(down & KEY_PLUS)
            break;

        gfxHandleBuffs();
    }

    fontDestroy(shared);

    pmshellExit();
    hidExit();
    romfsExit();
    graphicsExit();
}
