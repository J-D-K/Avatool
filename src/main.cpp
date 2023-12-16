#include <memory>
#include <switch.h>
#include "avatool.hpp"

int main(int argc, const char *argv[])
{
    std::unique_ptr<avatool> avatoolApp = std::make_unique<avatool>();
    while(avatoolApp->isRunning())
    {
        avatoolApp->update();
        avatoolApp->render();
    }
    return 0;
}