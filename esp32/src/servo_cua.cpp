#include "servo_cua.h"
#include "config.h"

#include <ESP32Servo.h>

namespace
{

Servo servoCua;

bool daKhoiTao = false;
bool cuaDangMo = false;

}

namespace ServoCua
{

void begin()
{
    servoCua.setPeriodHertz(50);

    const int kenhPwm = servoCua.attach(
        SERVO_PIN,
        500,
        2400
    );

    daKhoiTao = servoCua.attached();

    Serial.print("Servo GPIO: ");
    Serial.println(SERVO_PIN);
    Serial.print("Servo PWM channel: ");
    Serial.println(kenhPwm);
    Serial.print("Servo attach: ");
    Serial.println(daKhoiTao ? "THANH CONG" : "THAT BAI");

    if (daKhoiTao)
    {
        dongCua();
    }
}

void moCua()
{
    if (!daKhoiTao)
    {
        return;
    }

    servoCua.write(
        SERVO_GOC_MO
    );

    Serial.print("Servo MO, goc: ");
    Serial.print(SERVO_GOC_MO);
    Serial.print(", xung us: ");
    Serial.println(servoCua.readMicroseconds());

    cuaDangMo = true;
}

void dongCua()
{
    if (!daKhoiTao)
    {
        return;
    }

    servoCua.write(
        SERVO_GOC_DONG
    );

    Serial.print("Servo DONG, goc: ");
    Serial.print(SERVO_GOC_DONG);
    Serial.print(", xung us: ");
    Serial.println(servoCua.readMicroseconds());

    cuaDangMo = false;
}

bool dangMo()
{
    return cuaDangMo;
}

}
