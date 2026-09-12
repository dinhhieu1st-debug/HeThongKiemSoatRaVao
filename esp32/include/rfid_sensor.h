#ifndef RFID_SENSOR_H
#define RFID_SENSOR_H

#include <Arduino.h>

namespace RFIDSensor {

bool begin();

void wake();
void sleep();

bool isEnabled();
bool readCard(String &uid);

}

#endif