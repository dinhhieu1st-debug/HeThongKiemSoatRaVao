#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>

namespace OLEDDisplay {

bool begin();

void wake();
void sleep();
bool isEnabled();

void showStartup();
void showWaiting(bool obstacle);
void showCard(const String &uid, bool obstacle);
void showObstacle(bool obstacle);
void showError(const String &message);

}

#endif