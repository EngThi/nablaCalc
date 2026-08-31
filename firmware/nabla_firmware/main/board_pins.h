#pragma once

#include "driver/gpio.h"

// ============================================================================
// ∇Calc Hardware (ESP32-S3-WROOM-1-N16R8)
// ============================================================================

// I2C Peripherals (MCP23017 Keypad Expander)
#define I2C_MASTER_SDA_IO GPIO_NUM_4
#define I2C_MASTER_SCL_IO GPIO_NUM_5
#define I2C_MASTER_FREQ_HZ 400000 // 400 kHz Fast Mode
#define MCP23017_I2C_ADDR 0x20

// Display SPI (selected MSP2202 / ILI9341 2.2" 320x240 module)
#define LCD_SPI_MOSI GPIO_NUM_21
#define LCD_SPI_MISO GPIO_NUM_3
#define LCD_SPI_SCK GPIO_NUM_47
#define LCD_PIN_CS GPIO_NUM_38
#define LCD_PIN_DC GPIO_NUM_40
#define LCD_PIN_RST GPIO_NUM_41
#define LCD_PIN_BCKL GPIO_NUM_42
// Routed on the main PCB; touch support on the exact seller variant remains unverified.
#define TOUCH_PIN_CS GPIO_NUM_1
#define TOUCH_PIN_IRQ GPIO_NUM_2

// MicroSD Card SPI
#define SD_PIN_CS GPIO_NUM_39

// Camera DVP Interface (OV5640 5MP)
#define CAM_PIN_VSYNC GPIO_NUM_6
#define CAM_PIN_HREF GPIO_NUM_7
#define CAM_PIN_PCLK GPIO_NUM_12
#define CAM_PIN_XCLK GPIO_NUM_15
#define CAM_PIN_D0 GPIO_NUM_10
#define CAM_PIN_D1 GPIO_NUM_14
#define CAM_PIN_D2 GPIO_NUM_8
#define CAM_PIN_D3 GPIO_NUM_9
#define CAM_PIN_D4 GPIO_NUM_11
#define CAM_PIN_D5 GPIO_NUM_18
#define CAM_PIN_D6 GPIO_NUM_17
#define CAM_PIN_D7 GPIO_NUM_16
#define CAM_PIN_RESET GPIO_NUM_13
#define CAM_PIN_PWDN GPIO_NUM_48
#define CAM_PIN_PWR_EN GPIO_NUM_44

// Native USB CDC / JTAG
#define USB_PIN_DM GPIO_NUM_19
#define USB_PIN_DP GPIO_NUM_20
