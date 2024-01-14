#include <switch.h>
#include "ui/menu.hpp"

menu::menu(const int &x, const int &y, const int &rectWidth, const int &fontSize, const int &maxScroll) : m_X(x), m_Y(y), m_OriginalY(y), m_RectWidth(rectWidth), m_FontSize(fontSize), m_MaxScroll(maxScroll)
{
    m_RectHeight = m_FontSize + 30;
    m_Selected = 0;
}

menu::menu(const int &x, const int &y, const int &rectWidth, const int &fontSize, const int &maxScroll, const std::string *menuOpts, const int &menuOptCount) : menu(x, y, rectWidth, fontSize, maxScroll)
{
    for (int i = 0; i < menuOptCount; i++)
    {
        m_Options.push_back(menuOpts[i]);
    }
}

bool menu::update(const uint64_t &padDown)
{
    if (m_Options.empty())
    {
        return false;
    }
    bool hasChanged = false;

    // Input handling
    int menuSize = m_Options.size() - 1;
    int previousSelected = m_Selected;
    if (padDown & HidNpadButton_AnyUp && --m_Selected < 0)
    {
        m_Selected = menuSize;
    }
    else if (padDown & HidNpadButton_AnyDown && ++m_Selected > menuSize)
    {
        m_Selected = 0;
    }

    if(previousSelected != m_Selected)
    {
        hasChanged = true;
    }

    // Calculate if scrolling needs to happen
    if (m_Selected <= m_MaxScroll)
    {
        m_TargetY = m_OriginalY;
    }
    else if (m_Selected >= (menuSize - m_MaxScroll) && menuSize > m_MaxScroll * 2)
    {
        m_TargetY = m_OriginalY + -(m_RectHeight * (menuSize - (m_MaxScroll * 2)));
    }
    else if (m_Selected > m_MaxScroll && m_Selected < (menuSize - m_MaxScroll))
    {
        m_TargetY = -(m_RectHeight * (m_Selected - m_MaxScroll));
    }

    // Calculate scroll amount
    if (m_Y != m_TargetY)
    {
        float addY = (float)((float)m_TargetY - (float)m_Y) / 2.5; // Animation transition scaling is static for avatool
        m_Y += addY;
    }

    return hasChanged;
}

void menu::render(graphics *gfx, SDL_Texture *target)
{
    if (m_Options.empty())
    {
        return;
    }

    // Color shifting bounding box
    if (m_ColorShift && (m_ColorMod += 6) >= 0x72)
    {
        m_ColorShift = false;
    }
    else if (!m_ColorShift && (m_ColorMod -= 3) <= 0x00)
    {
        m_ColorShift = true;
    }

    int targetHeight;
    SDL_QueryTexture(target, NULL, NULL, NULL, &targetHeight);
    for (int i = 0; i < (int)m_Options.size(); i++)
    {
        if (m_Selected == i)
        {
            gfx->renderRectFill(target, createColor(0x00, (0x88 + m_ColorMod), (0xC5 + m_ColorMod / 2), 0xFF), m_X - 4, (m_Y + (i * m_RectHeight)) - 4, m_RectWidth, m_RectHeight);
        }
        gfx->renderTextf(target, m_FontSize, COLOR_WHITE, m_X + 8, m_Y + (i * m_RectHeight) + (m_RectHeight / 2 - m_FontSize / 2), m_Options.at(i).c_str());
    }
}

void menu::addOpt(const std::string &newOpt)
{
    m_Options.push_back(newOpt);
}