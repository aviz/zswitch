//
// Created by Avi Zrachya on 19/11/2016.
//

#include "WifiAP.h"
#include <ESP8266WiFi.h>
#include <ArduinoLog.h>
#include "Configuration.h"
#include "Event.h"

WifiAP::WifiAP() {
    Event::subscribe("WifiAP::onSetup", ON_SETUP, std::bind(&WifiAP::onSetup, this, std::placeholders::_1, std::placeholders::_2));
    Event::subscribe("WifiAP::onLoop", ON_LOOP, std::bind(&WifiAP::onLoop, this, std::placeholders::_1, std::placeholders::_2));
    Event::subscribe("WifiAP::onConfigurationChanged", ON_CONFIGURATION_CHANGED, std::bind(&WifiAP::onConfigurationChanged, this, std::placeholders::_1, std::placeholders::_2));
}

void WifiAP::setDefaults() {
    ssid = getWifiHost();

    // override from config.
    if (Config.fields.ap_ssid.get().length() > 0) {
        ssid = Config.fields.ap_ssid.get();
    }

    password = Config.fields.ap_password.get();
}

void WifiAP::onConfigurationChanged(int eventId, void *data) {

}

void WifiAP::onLoop(int eventId, void *data) {

}

void WifiAP::onSetup(int eventId, void *data) {
    setDefaults();
    WiFi.softAP(ssid.c_str(), password.c_str());
    IPAddress myIP = WiFi.softAPIP();

    Log.notice("AP: [%s] IP: [%s]\n", ssid.c_str(), myIP.toString().c_str());
}

std::string WifiAP::getWifiHost() {
    char wifiHost[32] = {0};
    uint8_t mac[6];

    WiFi.macAddress((uint8_t *) &mac);
    sprintf(wifiHost, "ZSwitch-%02X.%02X.%02X.%02X.%02X.%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return std::string(wifiHost);
}