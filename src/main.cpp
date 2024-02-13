#include <Arduino.h>
#include <BleKeyboard.h>

#include "ota.h"

//Set the name of the bluetooth keyboard (that shows up in the bluetooth menu of your device)
BleKeyboard bleKeyboard("RallyController");

const int volDownButton = 0; // Trip down
const int volUpButton = 1; // Trip up
const int trackNextButton = 2; // RB down
const int trackPrevButton = 3; // RB up
const int extraButton = 4;

#define LED 8
#define NUM_BUTTONS 5

const uint8_t BUTTON_PINS[NUM_BUTTONS] = {volUpButton, volDownButton, trackPrevButton, trackNextButton, extraButton};
uint8_t lastStates[NUM_BUTTONS] = {HIGH, HIGH, HIGH, HIGH, HIGH};
bool isOperational = true;

void blinkLED();
bool changeMode();
void loopButtons(void);
uint8_t checkButtonPress(int i, uint8_t state);

void setup() {
    Serial.begin(115200);
    bleKeyboard.begin();

    pinMode(LED, OUTPUT);
    
    pinMode(volUpButton, INPUT_PULLUP);
    pinMode(volDownButton, INPUT_PULLUP);
    pinMode(trackPrevButton, INPUT_PULLUP);
    pinMode(trackNextButton, INPUT_PULLUP);
    pinMode(extraButton, INPUT_PULLUP);
}

void loop() {

    if (changeMode()) {
        isOperational = !isOperational;
        if (isOperational) {
            endOTA();
            bleKeyboard.begin();
        } else {
            bleKeyboard.end();
            beginOTA();
        }
    }

    if (isOperational) {
        loopButtons();
    } else {
        loopOTA();
    }

    blinkLED();
}

void blinkLED() {
    int delaySeconds;
    if (isOperational)
        delaySeconds = 3000;
    else
        delaySeconds = 200;

    digitalWrite(LED, HIGH);
    delay(delaySeconds);
    digitalWrite(LED, LOW);
    delay(delaySeconds);
}

void loopButtons(void) {
    if (!bleKeyboard.isConnected()) {
        Serial.println("Not connected");
        return;
    }

    for (int i = 0; i < NUM_BUTTONS; i++) {
        uint8_t state = digitalRead(BUTTON_PINS[i]);
        
        if (state == LOW && lastStates[i] == HIGH) {
            state = checkButtonPress(i, state);
            Serial.print("Pressed ");
            Serial.println(BUTTON_PINS[i]);
        }

        lastStates[i] = state;
    }
}

uint8_t checkButtonPress(int i, uint8_t state) {
    switch (BUTTON_PINS[i]) {
        case volUpButton:
            bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
            Serial.println("Press VOL UP");
            break;
        case volDownButton:
            bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
            Serial.println("Press VOL DOWN");
            break;
        case extraButton:
            // bleKeyboard.write(KEY_END);
            // isOperational = !isOperational;
            // Serial.println("Press extra key");
            break;
        case trackPrevButton:
            Serial.println("Start PREV TRACK");
            while (state == LOW) {
                bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
                delay(100);
                state = digitalRead(BUTTON_PINS[i]);
            }
            Serial.println("End PREV TRACK");
            break;
        case trackNextButton:
            Serial.println("Start NEXT TRACK");
            while (state == LOW) {
                bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
                delay(100);
                state = digitalRead(BUTTON_PINS[i]);
            }
            Serial.println("End NEXT TRACK");
            break;
    }

    return state;
}

bool changeMode(void) {
    uint8_t state = digitalRead(BUTTON_PINS[4]);
        
    if (state == LOW && lastStates[4] == HIGH) {
        Serial.print("Pressed ");
        Serial.println(BUTTON_PINS[4]);
        
        lastStates[4] = state;
        return true;
    }

    lastStates[4] = state;
    return false;
}