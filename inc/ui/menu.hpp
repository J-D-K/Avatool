#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "graphics.hpp"

class menu 
{
    public:
        menu(const int &x, const int &y, const int &rectWidth, const int &fontSize, const int &maxScroll);
        menu(const int &x, const int &y, const int &rectWidth, const int &fontSize, const int &maxScroll, const std::string *m_Options, const int &menuOptCount);

        //Returns true if choice has been changed
        bool update(const uint64_t &padDown);
        void render(graphics *gfx, SDL_Texture *target);

        void addOpt(const std::string &newOpt);

        void reset() { m_Options.clear(); }

        // Needed for inheritance... in JKSV only though
        int getX() const { return m_X; }
        int getY() const { return m_Y; }
        int getOriginalY() const { return m_OriginalY; }
        int getTargetY() const { return m_TargetY; }
        int getRectWidth() const { return m_RectWidth; }
        int getRectHeight() const { return m_RectHeight; }
        int getSelected() const { return m_Selected; }
        int getMaxScroll() const { return m_MaxScroll; }
        int getColorMod() const { return m_ColorMod; }
        int getMenuSize() const { return m_Options.size(); }

    private:
        //Whether color shifting is adding or substracting
        bool m_ColorShift = true;
        //How much color is being modified
        int m_ColorMod = 0;
        //Coordinates
        int m_X, m_Y, m_OriginalY, m_TargetY;
        //Selection rectangle width and height. Height is auto calculated.
        int m_RectWidth, m_RectHeight;
        //Font size to use on render()
        int m_FontSize;
        //Currently m_Selected item
        int m_Selected;
        //How many items down or up before menu starts scrolling
        int m_MaxScroll;
        //Vector of menu options
        std::vector<std::string> m_Options;
};