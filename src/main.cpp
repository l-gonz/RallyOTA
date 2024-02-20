#include <Arduino.h>
#include <BleKeyboard.h>
#include <OneButton.h>

#include "main.h"
#include "ota.h"

const uint8_t KEY_ASSISTANT = 0x247;

enum OperationState {
    Roadbook,
    Navigation,
    OTA
};

void toggleOTAMode();
void cycleOpMode();
void switchDownOnTick();
void switchUpOnTick();
void lowButtonOnClick();
void upButtonOnClick();
void modeButtonOnClick();

OperationState state = Roadbook;

BleKeyboard bleKeyboard("RallyController");

OneButton switchDownButton(0, true, true); // Advance roadbook
OneButton switchUpButton(1, true, true); // Retract roadbook
OneButton bottomButton(2, true, true); // Trip down
OneButton middleButton(3, true, true); // Trip up
OneButton upperButton(4, true, true);


void setup() {
    Serial.begin(115200);
    bleKeyboard.begin();

    switchDownButton.setLongPressIntervalMs(300);
    switchUpButton.setLongPressIntervalMs(300);
    
    switchDownButton.attachDuringLongPress(switchDownOnTick);
    switchUpButton.attachDuringLongPress(switchUpOnTick);
    bottomButton.attachClick(lowButtonOnClick);
    middleButton.attachClick(upButtonOnClick);

    upperButton.attachClick(modeButtonOnClick);
    upperButton.attachLongPressStop(toggleOTAMode);
    upperButton.attachDoubleClick(cycleOpMode);
}

void loop() {

    if (state = OTA)
        loopOTA();

    switchDownButton.tick();
    switchUpButton.tick();
    bottomButton.tick();
    middleButton.tick();
    upperButton.tick();
}

void toggleOTAMode() {
    if (upperButton.getPressedMs() < 3000) return;

    switch (state) {
        case OTA:
            endOTA();
            bleKeyboard.begin();
            state = Roadbook;
            break;
        default:
            BLEDevice::deinit(true);
            beginOTA();
            state = OTA;
        break;
    }

    Serial.print("Change mode to ");
    Serial.println(state);
}

void cycleOpMode() {
    if (state == Roadbook)
        state = Navigation;
    else if (state == Navigation)
        state = Roadbook;

    Serial.print("Change mode to ");
    Serial.println(state);
}

void switchDownOnTick() {
    switch (state) {
        case Roadbook:
            bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
            Serial.println("Press roadbook forward");
            break;
        case Navigation:
            bleKeyboard.write(KEY_DOWN_ARROW);
            Serial.println("Press arrow down");
            break;
    }
}

void switchUpOnTick() {
    switch (state) {
        case Roadbook:
            bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
            Serial.println("Press roadbook back");
            break;
        case Navigation:
            bleKeyboard.write(KEY_UP_ARROW);
            Serial.println("Press arrow up");
            break;
    }
}

void upButtonOnClick() {
    switch (state) {
        case Roadbook:
            bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
            Serial.println("Press odo forward");
            break;
        case Navigation:
            bleKeyboard.write(KEY_NUM_PLUS);
            Serial.println("Press plus zoom");
            break;
    }
}

void lowButtonOnClick() {
    switch (state) {
        case Roadbook:
            bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
            Serial.println("Press odo back");
            break;
        case Navigation:
            bleKeyboard.write(KEY_NUM_MINUS);
            Serial.println("Press minus zoom");
            break;
    }
}

void modeButtonOnClick() {
    switch (state) {
        case Roadbook:
            bleKeyboard.write(KEY_ASSISTANT);
            Serial.println("Press assistant");
            break;
        case Navigation:
            bleKeyboard.write('C');
            Serial.println("Press arrow down");
            break;
    }
}