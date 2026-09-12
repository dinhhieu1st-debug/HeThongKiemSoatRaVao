#ifndef BUZZER_H
#define BUZZER_H

namespace Buzzer {

void begin();

void on();
void off();

// Keu mot lan trong thoi gian chi dinh
void beep(unsigned long durationMs);

// Goi lien tuc trong loop de tat coi dung thoi gian
void update();

bool isActive();

}

#endif