// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// Path: include/Constants.hpp

#include <map>

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#define STATUS_BUZZER 27
#define RESET_LED 2
#define NVS_RESET_PIN 0
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CONFIG_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define VERSION_CHARACTERISTIC_UUID "82bf15cb-ce87-4696-9698-a68391599b5f"
#define BLE_DEVICE_NAME "Stapes_"

#define HARDWARE_CHIP "ESP32"
#define HARDWARE_VERSION "0.0.1"
#define FIRMWARE_VERSION "0.1.0"

// COLORS
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

// BACKGROUND COLORS
#define BG_RED "\x1B[41m"
#define BG_GREEN "\x1B[42m"
#define BG_BLUE "\x1B[44m"
#define BG_MAGENTA "\x1B[45m"
#define BG_CYAN "\x1B[46m"
#define BG_WHITE "\x1B[47m"
#define BG_ORANGE "\x1B[48;5;208m"

struct ChannelPins {
    uint8_t inputPin;
    uint8_t outputPin;
};

std::map<uint8_t, ChannelPins> channelMap = {
    {1, {3, 4}},
    {2, {5, 16}},
    {3, {12, 17}},
    {4, {14, 18}},
    {5, {15, 19}},
    {6, {32, 21}},
    {7, {33, 22}},
    {8, {34, 23}},
    {9, {35, 25}},
    {10, {36, 26}},
    {11, {39, 27}}
};

#endif // CONSTANTS_HPP
