#include <Arduino.h>
#include <BleKeyboard.h>
#include <OneButton.h>

#include "ota.h"

const MediaKeyReport KEY_ASSISTANT = {2, 71};

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

BleKeyboard bleKeyboard("RallyControl_0");

OneButton switchDownButton(0, true, true); // Advance roadbook
OneButton switchUpButton(1, true, true); // Retract roadbook
OneButton bottomButton(2, true, true); // Trip down
OneButton middleButton(3, true, true); // Trip up
OneButton upperButton(4, true, true);

int count = 0;


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

    if (state == OTA)
        loopOTA();
    else if (!bleKeyboard.isConnected() && count++ % 200 == 0)
        Serial.println("BLE not connected");

    switchDownButton.tick();
    switchUpButton.tick();
    bottomButton.tick();
    middleButton.tick();
    upperButton.tick();

    delay(10);
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
            bleKeyboard.end();
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
            bleKeyboard.write('+');
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
            bleKeyboard.write('-');
            Serial.println("Press minus zoom");
            break;
    }
}

void modeButtonOnClick() {
    switch (state) {
        case Roadbook:
        case Navigation:
            bleKeyboard.write('C');
            Serial.println("Press arrow down");
            break;
    }
}