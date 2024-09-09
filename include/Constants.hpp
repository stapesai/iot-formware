// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/*
 * @file include/Constants.hpp
 * @brief This file contains various constants used throughout the project.
 * 
 * These constants include pin definitions, UUIDs for BLE services and 
 * characteristics, device names, firmware and hardware versions, and color 
 * codes for console output.
 */
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

// Pin Definitions
#define STATUS_BUZZER 27
#define RESET_LED 2
#define NVS_RESET_PIN 0

// BLE UUIDs
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CONFIG_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define VERSION_CHARACTERISTIC_UUID "82bf15cb-ce87-4696-9698-a68391599b5f"
#define BLE_DEVICE_NAME "Stapes_"

// Version Information
#define HARDWARE_CHIP "ESP32"
#define HARDWARE_VERSION "0.0.1"
#define FIRMWARE_VERSION "0.1.0"

// Console Color Codes
#define RESET_COLOR "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"
#define BLACK "\x1B[30m"
#define ORANGE "\x1B[38;5;208m"

// Background Color Codes
#define BG_RED "\x1B[41m"
#define BG_GREEN "\x1B[42m"
#define BG_BLUE "\x1B[44m"
#define BG_MAGENTA "\x1B[45m"
#define BG_CYAN "\x1B[46m"
#define BG_WHITE "\x1B[47m"
#define BG_ORANGE "\x1B[48;5;208m"

#endif // CONSTANTS_HPP
