#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

// BAUDRATE
const long BAUDRATE = 115200;

// PARPADEO
const int BLINK_OK = 250;


#define LINEA_BLANCO  F("                    ")

// PINOUT
// PINOUT
const int DHT_PIN = GPIO_NUM_33;        // ADC1_5
const int PIN_ADC = GPIO_NUM_32;        // ADC1_4
const int PIN_ENC_A = GPIO_NUM_18;      // 
const int PIN_ENC_B = GPIO_NUM_5;       //
const int PIN_ENC_PUSH = GPIO_NUM_19;   //
const int PIN_LED_G = GPIO_NUM_23;      // 
//const int PIN_LED_G = GPIO_NUM_1;       // TX0!, mejor no usar.
//const int PIN_SERVO_1 = GPIO_NUM_4;   // ADC2_0
//const int PIN_SERVO_2 = GPIO_NUM_15;  // ADC2_3
//const int PIN_SERVO_3 = GPIO_NUM_13;  // ADC2_4
//const int PIN_SERVO_4 = GPIO_NUM_12;  // ADC2_5
//const int PIN_SERVO_5 = GPIO_NUM_14;  // ADC2_6


const int I2C_ADDRESS = 0x3C;
// OLED
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;


#endif