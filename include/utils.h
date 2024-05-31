// Path: include/utils.h

#define STATUS_BUZZER 27
#define RESET_LED 2
#define NVS_RESET_PIN 0

void statusBuzzer(int times, int delayTime)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(STATUS_BUZZER, HIGH);
    delay(delayTime);
    digitalWrite(STATUS_BUZZER, LOW);
    delay(delayTime);
  }
}
