#include <Arduino.h>
#include <BleKeyboard.h>
#include <OneButton.h>

#include "ota.h"


BleKeyboard bleKeyboard("RallyController");

OneButton switchDownButton(0, true, true); // Advance roadbook
OneButton switchUpButton(1, true, true); // Retract roadbook
OneButton bottomButton(2, true, true); // Trip down
OneButton middleButton(3, true, true); // Trip up
OneButton upperButton(4, true, true);

bool isOperational = true;


void changeMode();
void roadbookForward();
void roadbookBackward();
void tripBackward();
void tripForward();
void extra();

void setup() {
    Serial.begin(115200);
    bleKeyboard.begin();

    switchDownButton.setLongPressIntervalMs(300);
    switchUpButton.setLongPressIntervalMs(300);
    
    switchDownButton.attachDuringLongPress(roadbookForward);
    switchUpButton.attachDuringLongPress(roadbookBackward);
    bottomButton.attachClick(tripBackward);
    middleButton.attachClick(tripForward);
    upperButton.attachClick(extra);
    
    upperButton.attachDoubleClick(changeMode);
}

void loop() {

    if (!isOperational)
        loopOTA();

    switchDownButton.tick();
    switchUpButton.tick();
    bottomButton.tick();
    middleButton.tick();
    upperButton.tick();
}

void changeMode() {
    Serial.println("Change mode");
    isOperational = !isOperational;
    if (isOperational) {
        endOTA();
        bleKeyboard.begin();
    } else {
        bleKeyboard.end();
        beginOTA();
    }
}

void roadbookForward() {
    bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
    Serial.println("Press roadbook forward");
}

void roadbookBackward() {
    bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
    Serial.println("Press roadbook back");
}

void tripForward() {
    bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
    Serial.println("Press trip forward");
}

void tripBackward() {
    bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
    Serial.println("Press trip back");
}

void extra() {
    Serial.println("Press extra");
}