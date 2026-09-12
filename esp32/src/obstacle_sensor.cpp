#include "obstacle_sensor.h"
#include "config.h"

namespace {

bool currentState = false;
bool changed = false;

int lastRawState = HIGH;
unsigned long lastChangeTime = 0;

bool convertRawState(int rawState) {
    return rawState == OBSTACLE_ACTIVE_LEVEL;
}

}

namespace ObstacleSensor {

void begin() {
    pinMode(OBSTACLE_PIN, INPUT);

    int rawState = digitalRead(OBSTACLE_PIN);

    lastRawState = rawState;
    currentState = convertRawState(rawState);

    Serial.print("Trang thai LM393: ");

    if (currentState) {
        Serial.println("CO VAT CAN");
    } else {
        Serial.println("KHONG CO VAT CAN");
    }
}

void update() {
    changed = false;

    int rawState = digitalRead(OBSTACLE_PIN);

    if (rawState != lastRawState) {
        lastRawState = rawState;
        lastChangeTime = millis();
    }

    if (millis() - lastChangeTime >= SENSOR_DEBOUNCE_MS) {
        bool newState = convertRawState(rawState);

        if (newState != currentState) {
            currentState = newState;
            changed = true;
        }
    }
}

bool hasObstacle() {
    return currentState;
}

bool stateChanged() {
    return changed;
}

}