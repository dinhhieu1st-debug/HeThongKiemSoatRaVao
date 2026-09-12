#include "oled_display.h"
#include "config.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

namespace {

Adafruit_SSD1306 display(
    OLED_WIDTH,
    OLED_HEIGHT,
    &Wire,
    -1
);

bool enabled = false;
bool initialized = false;

void drawHeader(const String &title) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(title);
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
}

void drawObstacleStatus(bool obstacle) {
    display.setTextSize(1);
    display.setCursor(0, 50);
    display.print("Vat can: ");

    if (obstacle) {
        display.println("CO");
    } else {
        display.println("KHONG");
    }
}

}

namespace OLEDDisplay {

bool begin() {
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    Wire.setClock(100000);

    if (!display.begin(
            SSD1306_SWITCHCAPVCC,
            OLED_ADDRESS
        )) {
        Serial.println("Khong tim thay OLED!");
        return false;
    }

    initialized = true;
    enabled = true;

    display.clearDisplay();
    display.display();

    Serial.println("OLED san sang.");
    return true;
}

void wake() {
    if (!initialized || enabled) {
        return;
    }

    display.ssd1306_command(SSD1306_DISPLAYON);
    enabled = true;

    Serial.println("Da bat OLED.");
}

void sleep() {
    if (!initialized || !enabled) {
        return;
    }

    display.clearDisplay();
    display.display();
    display.ssd1306_command(SSD1306_DISPLAYOFF);

    enabled = false;

    Serial.println("Da tat OLED.");
}

bool isEnabled() {
    return enabled;
}

void showStartup() {
    if (!initialized) {
        return;
    }

    wake();

    drawHeader("RFID ACCESS DEMO");

    display.setCursor(0, 20);
    display.println("Dang khoi dong...");
    display.println("ESP32 + RC522");
    display.println("LM393 + OLED");

    display.display();
}

void showWaiting(bool obstacle) {
    if (!initialized) {
        return;
    }

    wake();

    drawHeader("HE THONG SAN SANG");

    display.setCursor(0, 20);
    display.println("Moi quet the RFID");
    display.println("Tai dau doc RC522");

    drawObstacleStatus(obstacle);

    display.display();
}

void showCard(const String &uid, bool obstacle) {
    if (!initialized) {
        return;
    }

    wake();

    drawHeader("DA DOC THE RFID");

    display.setCursor(0, 18);
    display.println("UID:");

    if (uid.length() <= 10) {
        display.setTextSize(2);
    } else {
        display.setTextSize(1);
    }

    display.setCursor(0, 30);
    display.println(uid);

    drawObstacleStatus(obstacle);

    display.display();
}

void showObstacle(bool obstacle) {
    if (!initialized) {
        return;
    }

    wake();

    drawHeader("CAM BIEN LM393");

    display.setTextSize(2);
    display.setCursor(0, 22);

    if (obstacle) {
        display.println("CO VAT CAN");
    } else {
        display.println("AN TOAN");
    }

    display.setTextSize(1);
    display.setCursor(0, 50);
    display.println("Moi quet the RFID");

    display.display();
}

void showError(const String &message) {
    if (!initialized) {
        return;
    }

    wake();

    drawHeader("LOI HE THONG");

    display.setCursor(0, 20);
    display.println(message);

    display.display();
}

}