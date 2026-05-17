#ifndef __GPS_MENU_H__
#define __GPS_MENU_H__

#include <MenuItemInterface.h>

class GpsMenu : public MenuItemInterface {
public:
    GpsMenu() : MenuItemInterface("GPS") {}

    void optionsMenu(void);
    void wardrivingMenu(void);
    void drawIcon(float scale);
    bool hasTheme() { return sasConfig.theme.gps; }
    String themePath() { return sasConfig.theme.paths.gps; }

private:
    void configMenu(void);
};

#endif
