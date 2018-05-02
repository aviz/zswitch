//
// Created by Avi Zrachya on 27/04/2018.
//

#ifndef ZSWITCH_VALUE_H
#define ZSWITCH_VALUE_H

#include <cstdlib>
#include <string>
#include <ArduinoJson.h>


template <typename T> class Value {
protected:
    std::string name;
    T value;
    T oldValue;

public:
    void toJson(JsonObject &jsonObject) {
        return toJson(jsonObject, value);
    };

    void fromJson(JsonObject &jsonObject) {
        fromJson(jsonObject, value);
    };

    bool changed() {
        return value != oldValue;
    };

    void setValue(T value) {
        this->value = value;
    };

    T getValue() {
        return value;
    };

    T get() {
        return value;
    };

    T& operator=(const T value) {
        this->value = value;
        return this->value;
    };

    void commitChanges() {
        oldValue = value;
    };

    void setName(std::string &name) {
        this->name = name;
    }

    std::string getName() {
        return this->name;
    }


private:
    void toJson(JsonObject &jsonObject, std::string &value) {
        jsonObject[this->name.c_str()] = value.c_str();
    }
    void toJson(JsonObject &jsonObject, int &value) {
        jsonObject[this->name.c_str()] = value;
    }
    void fromJson(JsonObject &jsonObject, std::string &value) {
        value = (char *)jsonObject[name.c_str()].as<char *>();
    }
    void fromJson(JsonObject &jsonObject, int &value) {
        value = (int)jsonObject[name.c_str()].as<int>();
    }
};


#endif //ZSWITCH_VALUE_H
