#include <string>
#include <switch.h>

#include "gfx.h"
#include "menu.h"

extern font *shared;

void menu::setParams(const unsigned& _x, const unsigned& _y, const unsigned& _rW)
{
    x = _x;
    y = _y;
    rW = _rW;
    rY = _y;
}

void menu::addOpt(const std::string& add)
{
    opt.push_back(add);
}

void menu::editOpt(int ind, const std::string& ch)
{
    opt[ind] = ch;
}

menu::~menu()
{
    opt.clear();
}

bool menu::handleInput(const uint64_t& down)
{
    int size = opt.size() - 1;
    if(down & KEY_UP)
    {
        selected--;
        if(selected < 0)
            selected = size;

        if((start > selected)  && (start > 0))
            start--;
        if(size < 11)
            start = 0;
        if((selected - 10) > start)
            start = selected - 10;

        return true;
    }
    else if(down & KEY_DOWN)
    {
        selected++;
        if(selected > size)
            selected = 0;

        if((selected > (start + 9)) && ((start + 9) < size))
            start++;
        if(selected == 0)
            start = 0;

        return true;
    }
    else if(down & KEY_RIGHT)
    {
        selected += 5;
        if(selected > size)
            selected = size;
        if((selected - 10) > start)
            start = selected - 10;

        return true;
    }
    else if(down & KEY_LEFT)
    {
        selected -= 5;
        if(selected < 0)
            selected = 0;
        if(selected < start)
            start = selected;

        return true;
    }

    return false;
}

void menu::draw(const clr& textClr)
{
    if(clrAdd)
    {
        clrSh += 4;
        if(clrSh > 63)
            clrAdd = false;
    }
    else
    {
        clrSh--;
        if(clrSh == 0)
            clrAdd = true;
    }

    int length = 0;
    if((opt.size() - 1) < 10)
        length = opt.size();
    else
        length = start + 11;

    clr rectClr = clrCreateRGBA(0x00, 0x60 + clrSh, 0xBB + clrSh, 0xFF);

    for(int i = start; i < length; i++)
    {
        if(i == selected)
            drawRect(frameBuffer, x, y + ((i - start) * 36), rW, 32, rectClr);

        drawText(opt[i].c_str(), frameBuffer, shared, x + 8, (y + 8) + ((i - start) * 36), 18, clrCreateU32(0xFFFFFFFF));
    }
}

void menu::reset()
{
    opt.clear();

    fc = 0;
}

void menu::adjust()
{
    if(selected > (int)opt.size() - 1)
        selected = opt.size() - 1;

    if(opt.size() < 10)
        start = 0;
    else if(opt.size() > 10 && start + 10 > (int)opt.size() - 1)
        start--;
}


