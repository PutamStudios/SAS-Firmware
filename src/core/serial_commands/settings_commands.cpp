#include "settings_commands.h"
#include <globals.h>

uint32_t settingsCallback(cmd *c) {
    Command cmd(c);

    Argument setting_name_arg = cmd.getArgument("setting_name");
    Argument setting_value_arg = cmd.getArgument("setting_value");
    String setting_name = setting_name_arg.getValue();
    String setting_value = setting_value_arg.getValue();
    setting_name.trim();
    setting_value.trim();

    JsonDocument jsonDoc = sasConfig.toJson();
    JsonObject setting = jsonDoc.as<JsonObject>();

    if (setting_name.length() == 0 && setting_value.length() == 0) {
        // no args, just prints current config
        serializeJsonPretty(jsonDoc, Serial);
        serialDevice->println("");
        return true;
    }

    if (setting[setting_name].isNull()) {
        serialDevice->println("Invalid field name: " + setting_name);
        return false;
    }

    if (setting_value.length() == 0) {
        serialDevice->print(setting_name + " = ");
        serialDevice->println(setting[setting_name].as<String>());
        return true;
    }

    // TODO: improve this logic and move to SASConfig
    if (setting_name == "priColor") sasConfig.setUiColor(setting_value.toInt());
    if (setting_name == "rot") sasConfigPins.setRotation(setting_value.toInt());
    if (setting_name == "dimmerSet") sasConfig.setDimmer(setting_value.toInt());
    if (setting_name == "bright") sasConfig.setBright(setting_value.toInt());
    if (setting_name == "tmz") sasConfig.setTmz(setting_value.toFloat());
    if (setting_name == "soundEnabled") sasConfig.setSoundEnabled(setting_value.toInt());
    if (setting_name == "wifiAtStartup") sasConfig.setWifiAtStartup(setting_value.toInt());
    if (setting_name == "webUI") {
        sasConfig.setWebUICreds(
            setting_value.substring(0, setting_value.indexOf(",")),
            setting_value.substring(setting_value.indexOf(",") + 1)
        );
    }
    if (setting_name == "wifiAp") {
        sasConfig.setWifiApCreds(
            setting_value.substring(0, setting_value.indexOf(",")),
            setting_value.substring(setting_value.indexOf(",") + 1)
        );
    }
    if (setting_name == "wifi") {
        sasConfig.addWifiCredential(
            setting_value.substring(0, setting_value.indexOf(",")),
            setting_value.substring(setting_value.indexOf(",") + 1)
        );
    }
    if (setting_name == "bleName") sasConfigPins.setBleName(setting_value);
    if (setting_name == "irTx") sasConfigPins.setIrTxPin(setting_value.toInt());
    if (setting_name == "irTxRepeats")
        sasConfigPins.setIrTxRepeats(static_cast<uint8_t>(setting_value.toInt()));
    if (setting_name == "irRx") sasConfigPins.setIrRxPin(setting_value.toInt());
    if (setting_name == "rfTx") sasConfigPins.setRfTxPin(setting_value.toInt());
    if (setting_name == "rfRx") sasConfigPins.setRfRxPin(setting_value.toInt());
    if (setting_name == "rfModule")
        sasConfigPins.setRfModule(static_cast<RFModules>(setting_value.toInt()));
    if (setting_name == "rfFreq" && setting_value.toFloat())
        sasConfigPins.setRfFreq(setting_value.toFloat());
    if (setting_name == "rfFxdFreq") sasConfigPins.setRfFxdFreq(setting_value.toInt());
    if (setting_name == "rfScanRange") sasConfigPins.setRfScanRange(setting_value.toInt());
    if (setting_name == "rfidModule")
        sasConfigPins.setRfidModule(static_cast<RFIDModules>(setting_value.toInt()));
    if (setting_name == "wigleBasicToken") sasConfig.setWigleBasicToken(setting_value);
    if (setting_name == "devMode") sasConfig.setDevMode(setting_value.toInt());
    if (setting_name == "disabledMenus") sasConfig.addDisabledMenu(setting_value);

    return true;
}

uint32_t factoryResetCallback(cmd *c) {
    sasConfig.factoryReset();
    serialDevice->println("Factory reset done");
    return true;
}

void createSettingsCommands(SimpleCLI *cli) {
    cli->addCommand("factory_reset", factoryResetCallback);

    Command cmd = cli->addCommand("set/tings", settingsCallback);
    cmd.addPosArg("setting_name", "");
    cmd.addPosArg("setting_value", "");
}
