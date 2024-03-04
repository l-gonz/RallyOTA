#include <Arduino.h>
#include <BleKeyboard.h>
#include <OneButton.h>

#include "ota.h"

const int SWITCH_DOWN = 0;
const int SWITCH_UP = 1;
const int BOTTOM_BUTTON = 2;
const int MIDDLE_BUTTON = 3;
const int UPPER_BUTTON = 4;

// const MediaKeyReport KEY_ASSISTANT = {2, 71};

enum OperationState {
    Roadbook,
    Navigation,
    OTA
};

void toggleOTAMode();
void cycleOpMode();
void switchUpPress(void *button);
void switchDownPress(void *button);
void bottomPress(void *button);
void middlePress(void *button);
void modeButtonOnClick();

OperationState state = Roadbook;

BleKeyboard bleKeyboard("RallyControl_0");

OneButton switchDownButton(SWITCH_DOWN, true, true); // Advance roadbook
OneButton switchUpButton(SWITCH_UP, true, true); // Retract roadbook
OneButton bottomButton(BOTTOM_BUTTON, true, true); // Trip down
OneButton middleButton(MIDDLE_BUTTON, true, true); // Trip up
OneButton upperButton(UPPER_BUTTON, true, true); // Mode change

int count = 0;


void setup() {
    Serial.begin(115200);
    bleKeyboard.begin();

    switchUpButton.setPressMs(10);
    switchDownButton.setPressMs(10);
    switchUpButton.attachLongPressStart(switchUpPress, &switchUpButton);
    switchUpButton.attachLongPressStop(switchUpPress, &switchUpButton);
    switchDownButton.attachLongPressStart(switchDownPress, &switchDownButton);
    switchDownButton.attachLongPressStop(switchDownPress, &switchDownButton);

    bottomButton.setPressMs(10);
    middleButton.setPressMs(10);
    bottomButton.attachLongPressStart(bottomPress, &bottomButton);
    bottomButton.attachLongPressStop(bottomPress, &bottomButton);
    middleButton.attachLongPressStart(middlePress, &middleButton);
    middleButton.attachLongPressStop(middlePress, &middleButton);


    upperButton.setClickMs(500);
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

void switchUpPress(void *button) {
    switch (state) {
        case Roadbook:
            if (((OneButton *)button)->state() == 1) // Press start
                bleKeyboard.press(KEY_MEDIA_PREVIOUS_TRACK);
            else
                bleKeyboard.release(KEY_MEDIA_PREVIOUS_TRACK);
            break;
        case Navigation:
            if (((OneButton *)button)->state() == 1) // Press start
                bleKeyboard.press(KEY_UP_ARROW);
            else
                bleKeyboard.release(KEY_UP_ARROW);
            break;
    }
}

void switchDownPress(void *button) {
    switch (state) {
        case Roadbook:
            if (((OneButton *)button)->state() == 1) // Press start
                bleKeyboard.press(KEY_MEDIA_NEXT_TRACK);
            else
                bleKeyboard.release(KEY_MEDIA_NEXT_TRACK);
            break;
        case Navigation:
            if (((OneButton *)button)->state() == 1) // Press start
                bleKeyboard.press(KEY_DOWN_ARROW);
            else
                bleKeyboard.release(KEY_DOWN_ARROW);
            break;
    }
}

void middlePress(void *button) {
    switch (state) {
        case Roadbook:
            if (((OneButton *)button)->state() == 1) // Press start
                bleKeyboard.press(KEY_MEDIA_VOLUME_UP);
            else
                bleKeyboard.release(KEY_MEDIA_VOLUME_UP);
            Serial.println("Press odo forward");
            break;
        case Navigation:
            if (((OneButton *)button)->state() == 1) // Press start
                bleKeyboard.press('+');
            else
                bleKeyboard.release('+');
            Serial.println("Press plus zoom");
            break;
    }
}

void bottomPress(void *button) {
    switch (state) {
        case Roadbook:
            if (((OneButton *)button)->state() == 1) // Press start
                bleKeyboard.press(KEY_MEDIA_VOLUME_DOWN);
            else
                bleKeyboard.release(KEY_MEDIA_VOLUME_DOWN);
            Serial.println("Press odo back");
            break;
        case Navigation:
            if (((OneButton *)button)->state() == 1) // Press start
                bleKeyboard.press('-');
            else
                bleKeyboard.release('-');
            Serial.println("Press minus zoom");
            break;
    }
}

void modeButtonOnClick() {
    switch (state) {
        case Roadbook:
            bleKeyboard.write('F');
            Serial.println("Reset");
            break;
        case Navigation:
            bleKeyboard.write('C');
            Serial.println("Center");
            break;
    }
}