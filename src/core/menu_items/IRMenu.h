#ifndef __IR_MENU_H__
#define __IR_MENU_H__

#include <MenuItemInterface.h>

class IRMenu : public MenuItemInterface {
public:
    IRMenu() : MenuItemInterface("IR") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    bool hasTheme() { return sasConfig.theme.ir; }
    String themePath() { return sasConfig.theme.paths.ir; }

private:
    void configMenu(void);
};

#endif
