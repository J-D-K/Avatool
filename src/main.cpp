#include <switch.h>
#include "avatool.hpp"

int main(int argc, const char *argv[])
{
    //Remove
    socketInitializeDefault();
    nxlinkStdio();

    avatool *avatoolApp = new avatool(1280, 720);
    while(avatoolApp->isRunning())
    {
        avatoolApp->update();
        avatoolApp->render();
    }
    delete avatoolApp;
    printf("delete avatoolApp\n");
    return 0;
}