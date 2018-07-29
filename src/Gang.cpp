//
// Created by Avi Zrachya on 28/04/2018.
//

#include <Arduino.h>
#include <ArduinoLog.h>
#include "Configuration.h"
#include "Event.h"
#include "Gang.h"
#include <time.h>


Gang::Gang(std::string name, uint8_t relayPin, uint8_t keyPin) {
    lastKeyPinState = -1;
    setName(name);
    setRelayPin(relayPin);
    setKeyPin(keyPin);
    Event::subscribe("Gang::onLoop", ON_LOOP, std::bind(&Gang::onLoop, this, std::placeholders::_1, std::placeholders::_2));
}

void Gang::setState(GangState state) {
    this->state = state;
}

void Gang::setName(std::string name) {
    this->name = name;
}

void Gang::setRelayPin(uint8_t relayPin) {
    this->relayPin = relayPin;
    pinMode(this->relayPin, OUTPUT);
}

void Gang::setKeyPin(uint8_t keyPin) {
    this->keyPin = keyPin;
    pinMode(this->keyPin, INPUT_PULLUP);
}

void Gang::onLoop(int eventId, void *data) {
    checkKeyState();
}

void Gang::checkKeyState() {
    currentKeyPinState = digitalRead(keyPin);
    if (currentKeyPinState != lastKeyPinState) {
        lastKeyPinState = currentKeyPinState;
        onKeyStateChange();
    }
}

void Gang::onKeyStateChange() {
    Log.notice("Key %s: %d\n", name.c_str(), currentKeyPinState);
    if (currentKeyPinState == PRESSED) {
        toggleState();
    }
}

void Gang::toggleState() {
    (state == ON) ? turnOff() : turnOn();
}

void Gang::turnOn() {
    setState(ON);
    Log.notice("%s Turn On\n", name.c_str());
    turnRelayOn();
}

void Gang::turnOff() {
    setState(OFF);
    Log.notice("%s Turn Off\n", name.c_str());
    turnRelayOff();
}

void Gang::turnRelayOn() {
    digitalWrite(relayPin, HIGH);
}

void Gang::turnRelayOff() {
    digitalWrite(relayPin, LOW);
}

