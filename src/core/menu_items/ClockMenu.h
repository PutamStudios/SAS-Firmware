#ifndef __CLOCK_MENU_H__
#define __CLOCK_MENU_H__

#include <MenuItemInterface.h>

class ClockMenu : public MenuItemInterface {
public:
    ClockMenu() : MenuItemInterface("Clock") {}

    void optionsMenu(void);
    void showSubMenu(void);
    void drawIcon(float scale);
    bool hasTheme() { return sasConfig.theme.clock; }
    String themePath() { return sasConfig.theme.paths.clock; }
};

#endif
