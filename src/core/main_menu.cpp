#include "main_menu.h"
#include "display.h"
#include "utils.h"
#include <globals.h>

MainMenu::MainMenu() {
    _menuItems = {
        &wifiMenu,
        &bleMenu,
#if !defined(LITE_VERSION)
        &ethernetMenu,
#endif
        &rfMenu,
        &rfidMenu,
        &irMenu,
#if defined(FM_SI4713) && !defined(LITE_VERSION)
        &fmMenu,
#endif
        &fileMenu,
        &gpsMenu,
        &nrf24Menu,
#if !defined(LITE_VERSION)
#if !defined(DISABLE_INTERPRETER)
        &scriptsMenu,
        &appsMenu,
#endif
        &loraMenu,
#endif
        &othersMenu,
        &clockMenu,
#if !defined(LITE_VERSION)
        &connectMenu,
#endif
        &configMenu,
    };

    _totalItems = _menuItems.size();
}

MainMenu::~MainMenu() {}

void MainMenu::begin(void) {
    returnToMenu = false;
    options = {};

    std::vector<String> l = sasConfig.disabledMenus;
    for (int i = 0; i < _totalItems; i++) {
        String itemName = _menuItems[i]->getName();
        if (find(l.begin(), l.end(), itemName) == l.end()) { // If menu item is not disabled
            options.push_back(
                Option(
                    _menuItems[i]->getName(),
                    [this, i]() { _menuItems[i]->optionsMenu(); },
                    false,
                    nullptr,
                    _menuItems[i]
                )
            );
        }
    }
    _currentIndex = loopOptions(options, MENU_TYPE_MAIN, "Main Menu", _currentIndex);
};

/*********************************************************************
**  Function: hideAppsMenu
**  Menu to Hide or show menus
**********************************************************************/

void MainMenu::hideAppsMenu() {
    auto items = this->getItems();
RESTART: // using gotos to avoid stackoverflow after many choices
    options.clear();
    for (auto item : items) {
        String label = item->getName();
        std::vector<String> l = sasConfig.disabledMenus;
        bool enabled = find(l.begin(), l.end(), label) == l.end();
        options.push_back({label, [this, label]() { sasConfig.addDisabledMenu(label); }, enabled});
    }
    options.push_back({"Show All", [=]() { sasConfig.disabledMenus.clear(); }, true});
    addOptionToMainMenu();
    loopOptions(options);
    sasConfig.saveFile();
    if (!returnToMenu) goto RESTART;
}
