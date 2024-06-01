// Path: include/utils.h
// Utility functions and constants.

#ifndef UTILS_H
#define UTILS_H

#define STATUS_BUZZER 27
#define RESET_LED 2
#define NVS_RESET_PIN 0
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


class Utils
{
public:
    static void statusBuzzer(int times, int delayTime);
    static bool isNVSEmpty();
    static void resetModule();


};


// void statusBuzzer(int times, int delayTime)
// {
//   for (int i = 0; i < times; i++)
//   {
//     digitalWrite(STATUS_BUZZER, HIGH);
//     delay(delayTime);
//     digitalWrite(STATUS_BUZZER, LOW);
//     delay(delayTime);
//   }
// }

#endif // UTILS_H
