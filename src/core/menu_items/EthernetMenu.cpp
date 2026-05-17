#include "EthernetMenu.h"
#if !defined(LITE_VERSION)
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "modules/ethernet/ARPScanner.h"
#include "modules/ethernet/DHCPStarvation.h"
#include "modules/ethernet/EthernetHelper.h"
#include "modules/ethernet/MACFlooding.h"

void EthernetMenu::start_ethernet() {
    eth = new EthernetHelper();
    if (!eth->setup()) {
        displayError("W5500 not found");
        delete eth;
        eth = nullptr;
        return;
    }
    while (!eth->is_connected()) { delay(100); }
}

void EthernetMenu::optionsMenu() {
    options = {
        {"Scan Hosts",
         [this]() {
             start_ethernet();
             if (eth != nullptr) {
                 run_arp_scanner();
                 eth->stop();
             } else {
                    displayError("W5500 not found");
             }
         }                        },
        {"DHCP Starvation",
         [this]() {
             start_ethernet();
             if (eth != nullptr) {
                 DHCPStarvation();
                 eth->stop();
             } else {
                    displayError("W5500 not found");
             }
         }                        },
        {"MAC Flooding",    [this]() {
             start_ethernet();
             if (eth != nullptr) {
                 MACFlooding();
                 eth->stop();
             } else {
                    displayError("W5500 not found");
             }
         }}
    };
    addOptionToMainMenu();

    delay(200);

    loopOptions(options, MENU_TYPE_SUBMENU, "Ethernet");
}

void EthernetMenu::drawIcon(float scale) {
    clearIconArea();

    int iconW = scale * 30;
    int iconH = scale * 40;

    int Y = iconCenterY - (scale * 28);

    int smallerH = scale * 16;

    int starterX = iconCenterX - iconW; // X of the first side
    int finalX = iconCenterX + iconW;

    int lineWidth = 2;

    // Draw the main socket structure
    tft.drawRect(starterX, Y, lineWidth, iconH, sasConfig.priColor);
    tft.drawRect(finalX, Y, lineWidth, iconH, sasConfig.priColor);
    tft.drawRect(starterX, Y, iconW * 2, lineWidth, sasConfig.priColor);

    // Draw the shorter side to close the first part of socket
    tft.drawRect(starterX, Y + iconH, smallerH, lineWidth, sasConfig.priColor);
    tft.drawRect(finalX - smallerH + lineWidth, Y + iconH, smallerH, lineWidth, sasConfig.priColor);

    // Draw the final enclosure
    tft.drawRect(starterX + smallerH, Y + iconH, lineWidth, smallerH, sasConfig.priColor);
    tft.drawRect(finalX - smallerH + lineWidth, Y + iconH, lineWidth, smallerH, sasConfig.priColor);

    // Draw the four cable pins
    int pinOffset = scale * 15;
    int pinH = scale * 16;
    if (pinOffset < 2) pinOffset = 2;
    if (pinH < 2) pinH = 2;
    for (size_t i = 0; i < 4; i++) {
        tft.drawRect(starterX + pinOffset + (i * pinOffset), Y, lineWidth, pinH, sasConfig.priColor);
    }

    // Close the socket
    tft.drawRect(
        starterX + smallerH,
        Y + iconH + smallerH,
        (iconW * 2) - (smallerH * 2) + 4,
        lineWidth,
        sasConfig.priColor
    );
}
#endif
