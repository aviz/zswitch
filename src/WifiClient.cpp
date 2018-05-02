//
// Created by Avi Zrachya on 18/11/2016.
//

#include "WifiClient.h"
#include "Event.h"
#include "Configuration.h"
#include <ESP8266WiFi.h>
#include <ArduinoLog.h>


WifiClient::WifiClient() {
    Event::subscribe("WifiClient::onSetup", ON_SETUP, std::bind(&WifiClient::onSetup, this, std::placeholders::_1, std::placeholders::_2));
    Event::subscribe("WifiClient::onLoop", ON_LOOP, std::bind(&WifiClient::onLoop, this, std::placeholders::_1, std::placeholders::_2));
    Event::subscribe("WifiClient::onConfigurationChanged", ON_CONFIGURATION_CHANGED, std::bind(&WifiClient::onConfigurationChanged, this, std::placeholders::_1, std::placeholders::_2));

}

void WifiClient::setDefaults() {
    generateWifiHost();

}

void WifiClient::onSetup(int eventId, void *data) {
    setDefaults();
}


void WifiClient::onLoop(int eventId, void *data) {
    connectIfDisconnected();
    logStatusIfChanged();
}

void WifiClient::onConfigurationChanged(int eventId, void *data) {
    //std::unordered_map<std::string, boolean> *changed = static_cast<std::unordered_map<std::string, boolean> *>(data);
    if(Config.fields.wifi_password.changed() || Config.fields.wifi_ssid.changed()) {
        reconnect();
    }
}

void WifiClient::connectIfDisconnected() {
    if(!connected() && connectTimeElapsed() > 30) {
        connect();
    }
}

void WifiClient::logStatusIfChanged() {
    if(statusChanged()) {
        logStatus();
    }
}

void WifiClient::generateWifiHost() {
    uint8_t mac[6];

    WiFi.macAddress((uint8_t *) &mac);
    sprintf(wifiHost, "ZSwitch_%02X.%02X.%02X.%02X.%02X.%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void WifiClient::connect() {
    Log.notice("Connecting host: [%s] ssid: [%s] password: [%s]]\n", wifiHost, Config.fields.wifi_ssid.get().c_str(), Config.fields.wifi_password.get().c_str());
    WiFi.disconnect();
    WiFi.hostname(wifiHost);
    WiFi.begin(Config.fields.wifi_ssid.get().c_str(), Config.fields.wifi_password.get().c_str());
    connectTimeStart();
}

bool WifiClient::connected() {
    return WiFi.isConnected();
}

time_t WifiClient::connectTimeElapsed() {
    return time(NULL) - this->timeConnected;
}


time_t WifiClient::connectTimeStart() {
    return this->timeConnected = time(NULL);
}

bool WifiClient::statusChanged() {
    bool statusChanged = (lastStatus != WiFi.status());
    lastStatus = WiFi.status();
    return statusChanged;
}

void WifiClient::logStatus() {
    switch(WiFi.status()) {
        case WL_CONNECTED:
            Log.notice("WifiClient: Connected to [SSID:%s] [IP:%s]\n", Config.fields.wifi_ssid.get().c_str(), WiFi.localIP().toString().c_str());
            Event::publish(ON_WIFI_CONNECTED);
            break;
        case WL_CONNECT_FAILED:
            Log.error("WifiClient: Connection to [SSID:%s] failed\n", Config.fields.wifi_ssid.get().c_str());
            Event::publish(ON_WIFI_DISCONNECTED);
            break;
        case WL_CONNECTION_LOST:
            Log.error("WifiClient: Connection to [SSID:%s] lost\n", Config.fields.wifi_ssid.get().c_str());
            Event::publish(ON_WIFI_DISCONNECTED);
            break;
        case WL_DISCONNECTED:
            Log.error("WifiClient: Disconnected from [SSID:%s]\n", Config.fields.wifi_ssid.get().c_str());
            Event::publish(ON_WIFI_DISCONNECTED);
            break;
        default:
            Log.notice("WifiClient: Status changed to %d for [SSID:%s]\n", WiFi.status(), Config.fields.wifi_ssid.get().c_str());
    }
}

void WifiClient::reconnect() {
    connect();
}


