#pragma once

#include <string>
#include <vector>

#include "gfx.h"

class console
{
    public:
        console(int maxLines){ maxl = maxLines; lines.push_back(""); }
        void out(const font *f, const std::string& s)
        {
            lines[cLine] += s;

            //cheat
            drawRect(frameBuffer, 30, 88, 1220, 560, clrCreateU32(0xFF2D2D2D));

            for(unsigned i = 0; i < lines.size(); i++)
                drawText(lines[i].c_str(), frameBuffer, f, 38, 98 + (i * 20), 16);

            gfxHandleBuffs();
        }

        void nl()
        {
            lines.push_back("");
            if((int)lines.size() == maxl)
            {
                lines.erase(lines.begin());
            }
            cLine = lines.size() - 1;
        }

    private:
        std::vector<std::string> lines;
        int maxl, cLine = 0;
};
