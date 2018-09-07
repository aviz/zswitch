//
// Created by Avi Zrachya on 24/11/2016.
//

#include "Configuration.h"
#include "Event.h"
#include <ArduinoJson.h>
#include <FS.h>
#include <WString.h>
#include <ArduinoLog.h>
#include "Value.h"


void Configuration::onInit(int eventId, void *data) {
    SPIFFS.begin();
    initializeValues();
    bind("version", fields.version);
    bind("ap_password", fields.ap_password);
    bind("ap_ssid", fields.ap_ssid);
    bind("wifi_ssid", fields.wifi_ssid);
    bind("wifi_password", fields.wifi_password);
    bind("mqtt_host", fields.mqtt_host);
    bind("mqtt_topic", fields.mqtt_topic);
    Log.notice("-----------------------------\n");
    Log.notice("      Configuration Init     \n");
    Log.notice("-----------------------------\n");
    loadConfigFile();
    initializeValues();
    save();

}

bool Configuration::loadConfigFile() {
    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile) {
        Log.error("Failed to open config file\n");
        return false;
    }

    size_t size = configFile.size();

    char *file_content = (char *) malloc(size);

    if (file_content == NULL) {
        Log.error("Allocation error\n");
        return false;
    }

    configFile.readBytes(file_content, size);
    file_content[size] = '\0';

    Log.notice("Configuration: [%s]\n", file_content);

    bool rc = deserialize(file_content);
    free(file_content);

    return rc;
}

void Configuration::initializeValues() {
    fields.version = MAX_VERSION;
    fields.ap_password = "Admin";
    fields.ap_ssid = "admin";
    fields.wifi_ssid = "ZrachyaWifi";
    fields.wifi_password = "19021952";
    fields.mqtt_host = "192.168.1.10";
    fields.mqtt_topic = "zhome";
}

void Configuration::fromJson(JsonObject &json) {
    for(auto &val : intValues) {
        val.fromJson(json);
    }

    for(auto &val : stringValues) {
        val.fromJson(json);
    }
}

void Configuration::toJson(JsonObject &json) {
    for(auto &val : intValues) {
        val.toJson(json);
    }

    for(auto &val : stringValues) {
        val.toJson(json);
    }
}

void Configuration::commit() {
    for(auto &val : intValues) {
        val.commitChanges();
    }

    for(auto &val : stringValues) {
        val.commitChanges();
    }
}

bool Configuration::deserialize(const char *data) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(data);

    if (!json.success()) {
        Log.error("Failed to parse config file\n");
        return false;
    }

    fromJson(json);

    fields.version = MAX_VERSION;

    return true;
}

bool Configuration::serializeToConfig() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();


    toJson(json);

    fields.version = MAX_VERSION;

    File config_file = SPIFFS.open("/config.json", "w");
    if (!config_file) {
        Log.error("Failed to open config file for writing\n");
        return false;
    }

    json.printTo(config_file);
    config_file.close();

    return true;
}

void Configuration::serializeToString(std::string &out) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();


    toJson(json);
    fields.version = MAX_VERSION;

    String str(out.c_str());
    json.printTo(str);
}

void Configuration::save() {
    Log.notice("Configuration::save() called\n");
    serializeToConfig();
    Log.notice("Configuration::save() publish event\n");
    Event::publish(ON_CONFIGURATION_CHANGED);
    Log.notice("Configuration::save() commiting changes\n");
    commit();
}

Configuration::Configuration() {
    Event::subscribe("Configuration::onInit", ON_INIT, std::bind(&Configuration::onInit, this, std::placeholders::_1, std::placeholders::_2));
}

void Configuration::bind(std::string name, Value<std::string> &value) {
    value.setName(name);
    stringValues.push_back(value);
}

void Configuration::bind(std::string name, Value<int> &value) {
    value.setName(name);
    intValues.push_back(value);
}

Configuration Config;
