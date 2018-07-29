//
// Created by Avi Zrachya on 28/04/2018.
//

#ifndef ZSWITCH_GANG_H
#define ZSWITCH_GANG_H

#include <string>

enum GangState {
    ON,
    OFF
};
enum KeyState {
    PRESSED,
    UNPRESSED
};

class Gang {
private:
    GangState state;
    std::string name;
    uint8_t relayPin;
    uint8_t keyPin;
    int currentKeyPinState;
    int lastKeyPinState;

    void setState(GangState state);
    void onLoop(int eventId, void *data);
    void checkKeyState();
    void onKeyStateChange();
    void toggleState();
    void turnOn();
    void turnOff();
    void turnRelayOn();
    void turnRelayOff();

public:
    Gang(std::string name, uint8_t relayPin, uint8_t keyPin);

    void setName(std::string name);
    void setRelayPin(uint8_t relayPin);
    void setKeyPin(uint8_t keyPin);
};


#endif //ZSWITCH_GANG_H
