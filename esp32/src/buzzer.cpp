#include "buzzer.h"
#include "config.h"

#include <Arduino.h>

namespace {

bool active = false;
unsigned long stopTime = 0;

void writeBuzzer(bool turnOn) {
    if (turnOn) {
        digitalWrite(BUZZER_PIN, BUZZER_ACTIVE_LEVEL);
    } else {
        digitalWrite(
            BUZZER_PIN,
            BUZZER_ACTIVE_LEVEL == HIGH ? LOW : HIGH
        );
    }
}

}

namespace Buzzer {

void begin() {
    pinMode(BUZZER_PIN, OUTPUT);

    writeBuzzer(false);

    active = false;
    stopTime = 0;

    Serial.println("Buzzer san sang.");
}

void on() {
    writeBuzzer(true);

    active = true;
    stopTime = 0;
}

void off() {
    writeBuzzer(false);

    active = false;
    stopTime = 0;
}

void beep(unsigned long durationMs) {
    writeBuzzer(true);

    active = true;
    stopTime = millis() + durationMs;
}

void update() {
    if (!active || stopTime == 0) {
        return;
    }

    if ((long)(millis() - stopTime) >= 0) {
        off();
    }
}

bool isActive() {
    return active;
}

}