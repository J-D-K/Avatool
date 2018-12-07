#include <switch.h>
#include <stdio.h>
#include <sys/stat.h>

#include "gfx.h"
#include "cons.h"
#include "dir.h"

console info(28);
font *shared;

int main(int argc, const char *argv[])
{
    hidInitialize();
    pmshellInitialize();
    graphicsInit(1280, 720);

    shared = fontLoadSharedFonts();

    texClearColor(frameBuffer, clrCreateU32(0xFF2D2D2D));
    drawText("Avatool", frameBuffer, shared, 64, 38, 24);
    drawRect(frameBuffer, 30, 87, 1220, 1, clrCreateU32(0xFFFFFFFF));
    drawRect(frameBuffer, 30, 648, 1220, 1, clrCreateU32(0xFFFFFFFF));

    info.out(shared, "Press A to export current avatars to #sdmc:/avatar/#, X to import from #sdmc:/avatar/#. Plus to Exit.");
    info.nl();

    while(appletMainLoop())
    {
        hidScanInput();

        uint64_t down = hidKeysDown(CONTROLLER_P1_AUTO);

        if(down & KEY_A)
        {
            info.out(shared, "Attempting to stop _account_... ");
            if(R_SUCCEEDED(pmshellTerminateProcessByTitleId(0x010000000000001E)))
            {
                info.out(shared, "^Succeeded!^");
                info.nl();

                FsFileSystem acc;
                info.out(shared, "Attempting to open 0x8000000000000010... ");
                if(R_SUCCEEDED(fsMount_SystemSaveData(&acc, 0x8000000000000010)))
                {
                    info.out(shared, "^Succeeded!^");
                    info.nl();

                    fsdevMountDevice("account", acc);

                    delDir("sdmc:/avatar");
                    mkdir("sdmc:/avatar", 777);

                    copyDirToDir("account:/su/avators/", "sdmc:/avatar/");

                    fsdevUnmountDevice("account");
                }
                else
                {
                    info.out(shared, "*Failed.*");
                    info.nl();
                }
            }
            else
            {
                info.out(shared, "*Failed*");
                info.nl();
            }
        }
        else if(down & KEY_X)
        {
            info.out(shared, "Attempting to stop _account_... ");
            if(R_SUCCEEDED(pmshellTerminateProcessByTitleId(0x010000000000001E)))
            {
                info.out(shared, "^Succeeded!^");
                info.nl();

                FsFileSystem acc;
                info.out(shared, "Attempting to open 0x8000000000000010... ");
                if(R_SUCCEEDED(fsMount_SystemSaveData(&acc, 0x8000000000000010)))
                {
                    info.out(shared, "^Succeeded!^");
                    info.nl();

                    fsdevMountDevice("account", acc);

                    copyDirToDir("sdmc:/avatar/", "account:/su/avators/");

                    info.out(shared, "Committing to system... ");
                    if(R_SUCCEEDED(fsdevCommitDevice("account")))
                    {
                        info.out(shared, "^Succeeded!^");
                        info.nl();
                    }
                    else
                    {
                        info.out(shared, "*Failed.*");
                        info.nl();
                    }
                    fsdevUnmountDevice("account");
                }
                else
                {
                    info.out(shared, "*Failed*");
                    info.nl();
                }
            }
            else
            {
                info.out(shared, "*Failed*");
                info.nl();
            }
        }
        else if(down & KEY_PLUS)
            break;

        gfxHandleBuffs();
    }

    fontDestroy(shared);

    pmshellExit();
    hidExit();
    graphicsExit();
}
