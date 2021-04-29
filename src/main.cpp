#include <switch.h>

#include "gfx.h"
#include "cons.h"
#include "dir.h"
#include "avasel.h"

console info(28);
font *shared;
const char *ctrl = "\ue0e4\ue0e5 Change Target   \ue0e3 Dump all to SD   \ue0e0 Overwrite";

bool shutdownMount()
{
    info.out("Attempting to shut down account and olsc... ");
    if(R_SUCCEEDED(pmshellTerminateProgram(0x010000000000001E)))
    {
        info.out("^Succeeded^!");
        info.nl();

        //helps a bit
        pmshellTerminateProgram(0x010000000000003E);

        //Sleeping helps with mount
        svcSleepThread(1e+9);

        info.out("Attempting to mount #0x8000000000000010#... ");
        FsFileSystem acc;
        Result res = 0;
        if(R_SUCCEEDED((res = fsOpen_SystemSaveData(&acc, FsSaveDataSpaceId_System, 0x8000000000000010, (AccountUid) {0}))))
        {
            info.out("^Succeeded^!");
            info.nl();
            fsdevMountDevice("account", acc);
            return true;
        }
        else
        {
            char tmp[64];
            sprintf(tmp, "Failed: 0x%08X", (uint32_t)res);
            info.out(tmp);
            info.nl();
        }
    }
    else
    {
        info.out("*Failed*");
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

    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

    bool success = false;
    unsigned ctrlX = 1230 - textGetWidth(ctrl, shared, 18);

    info.out("*WARNING*: This tool works by shutting down the account service on your Switch. This *will* lead to an eventual crash.");
    info.nl();
    info.out("Press \ue0e0 to continue. \ue0ef to exit.");
    info.nl();

    while(appletMainLoop())
    {
        padUpdate(&pad);
        uint64_t down = padGetButtonsDown(&pad);

        gfxBeginFrame();
        texClearColor(frameBuffer, clrCreateU32(0xFF2D2D2D));
        if(!success)
        {
            if(down & HidNpadButton_A && (success = shutdownMount()))
                avaSelPrep();

            info.draw(shared);
        }
        else
        {
            avaSel(down);
            drawText(ctrl, frameBuffer, shared, ctrlX, 672, 18, clrCreateU32(0xFFFFFFFF));
        }

        if(down & HidNpadButton_Plus)
            break;

        drawText("Avatool", frameBuffer, shared, 64, 38, 24, clrCreateU32(0xFFFFFFFF));
        drawRect(frameBuffer, 30, 87, 1220, 1, clrCreateU32(0xFFFFFFFF));
        drawRect(frameBuffer, 30, 648, 1220, 1, clrCreateU32(0xFFFFFFFF));
        gfxEndFrame();
    }

    fontDestroy(shared);

    if(success)
        avaSelClean();

    pmshellExit();
    hidExit();
    romfsExit();
    graphicsExit();
}
