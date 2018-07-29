//
// Created by Avi Zrachya on 24/11/2016.
//

#ifndef ZSWITCH_CONFIG_H
#define ZSWITCH_CONFIG_H


#include <string>
#include <map>
#include <functional>
#include <list>
#include "Value.h"


#define MAX_PASS_LENGTH 16
#define MAX_SSID_LENGTH 128
#define MAX_VERSION 1

typedef struct {
    Value<int> version;
    Value<std::string> ap_password;
    Value<std::string> ap_ssid;
    Value<std::string> wifi_ssid;
    Value<std::string> wifi_password;
} ConfigurationFields;


class Configuration {
private:
    bool serializeToConfig();
    bool deserialize(const char *data);
    bool loadConfigFile();
public:
    void save();

public:
    Configuration();

    void onInit(int eventId, void *data);
    void initializeValues();
    ConfigurationFields fields;

private:
    std::list<Value<std::string>> stringValues;
    std::list<Value<int>> intValues;

    void bind(std::string name, Value<std::string> &value);
    void bind(std::string name, Value<int> &value);

    void serializeToString(std::string &out);

    void fromJson(JsonObject &json);
    void toJson(JsonObject &json);
    void commit();
};

extern Configuration Config;

#endif //ZSWITCH_CONFIG_H

