#ifndef __BLE_MENU_H__
#define __BLE_MENU_H__

#include <MenuItemInterface.h>

class BleMenu : public MenuItemInterface {
public:
    BleMenu() : MenuItemInterface("BLE") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    bool hasTheme() { return sasConfig.theme.ble; }
    String themePath() { return sasConfig.theme.paths.ble; }

private:
    void configMenu(void);
    void setBleNameMenu(void);
};

#endif
