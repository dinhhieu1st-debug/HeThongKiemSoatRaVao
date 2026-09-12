#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==============================
// CHAN KET NOI RC522
// ==============================
#define RFID_SS_PIN 5
#define RFID_RST_PIN 27
#define RFID_SCK_PIN 18
#define RFID_MISO_PIN 19
#define RFID_MOSI_PIN 23

// ==============================
// CHAN KET NOI OLED SSD1306
// ==============================
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22
#define OLED_ADDRESS 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// ==============================
// CHAN CAM BIEN VAT CAN LM393
// ==============================
#define OBSTACLE_PIN 34

// LOW la co vat can
#define OBSTACLE_ACTIVE_LEVEL LOW

// Thoi gian chong nhieu cam bien
#define SENSOR_DEBOUNCE_MS 100

// ==============================
// CHAN BUZZER
// ==============================
#define BUZZER_PIN 26

// Buzzer chu dong: HIGH la keu, LOW la tat
#define BUZZER_ACTIVE_LEVEL HIGH

// ==============================
// CHAN SERVO DIEU KHIEN CUA
// ==============================
#define SERVO_PIN 25

// Goc servo khi cua dong
#define SERVO_GOC_DONG 0

// Goc servo khi cua mo
#define SERVO_GOC_MO 90

// ==============================
// CAC KHOANG THOI GIAN
// ==============================

// Cho quet the trong 15 giay
#define CARD_WAIT_TIMEOUT_MS 15000

// Xac nhan nguoi da roi sau 3 giay
#define PERSON_CLEAR_CONFIRM_MS 3000

// Hien UID tren OLED trong 3 giay
#define CARD_DISPLAY_TIME_MS 3000

#endif