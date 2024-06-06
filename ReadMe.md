# HOME AUTOMATION FIRMWARE

- In ESP32, pin `27` is for `STATUS_BUZZER` and pin `2` is for `BUILTIN_LED`.
- In ESP8266, pin `D0` is for `STATUS_BUZZER` and pin `D4` is for `BUILTIN_LED`.

## Setup Device

- Firmware

```python
python -m esptool ...
```

- File System

```python
python -m esptool ...
```

## ESP8266 Pinout

```c
    D0  = 16; // STATUS_BUZZER
    D1  = 5;
    D2  = 4;
    D3  = 0; # FLASH BUTTON
    D4  = 2;  // BUILTIN_LED
    D5  = 14; # HSCLK
    D6  = 12; # HMISO
    D7  = 13; # HMOSI
    D8  = 15; # HCS
```

![NodeMCU Pinout](extras/pinout_diagrams/esp8266-nodemcu/pinout.png)
[Learn More about NodeMCU Pins](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/)

## ESP32 Pinout

```c

```

## STATUS_BUZZER ERROR CODES

- Continous Beep -> Updating Firmware
- `1 Beep` -> API Call Received
- `2 Beeps` -> Connecting to WiFi
- `3 Beeps:`
  1. Failed to mount LittleFS
  2. Failed to open `config.json` OR `config.json` not found
  3. Failed to parse `config.json`
- `4 Beeps` -> Invalid pin found in `config.json`
- `5 Beeps` -> Failed to connect to MQTT Server.

## MQTT Topic Structure

## Base Topic

```plaintext
user/{{user_id}}/home/{{home_id}}/node/{{node_id}}/
```

## Keep Alive

### Request Keep Alive

- **Topic:** `user/{{user_id}}/home/{{home_id}}/node/{{node_id}}/keepalive/request`
- **Payload:**

  ```json
  {
    "origin": "(from where keep alive message was requested)"
  }
  ```

### Keep Alive Response

- **Topic:** `user/{{user_id}}/home/{{home_id}}/node/{{node_id}}/keepalive`
- **Payload:**

  ```json
  {
    "origin": "{{node_id}}",
    "counter": "",
    "uptime": ""
  }
  ```

## Device Registration

### Register Device Request

- **Topic:** `user/{{user_id}}/home/{{home_id}}/node/{{node_id}}/device/register/request`
- **Payload:**

  ```json
  {
    "device_id": "",
    "channel": ""
  }
  ```

### Register Device Response

- **Topic:** `user/{{user_id}}/home/{{home_id}}/node/{{node_id}}/device/register`
- **Payload:**

  ```json
  {
    "status": "true/false",
    "message": ""
  }
  ```

## Device Unregistration

### Unregister Device Request

- **Topic:** `user/{{user_id}}/home/{{home_id}}/node/{{node_id}}/device/unregister/request`
- **Payload:**

  ```json
  {
    "device_id": ""
  }
  ```

### Unregister Device Response

- **Topic:** `user/{{user_id}}/home/{{home_id}}/node/{{node_id}}/device/unregister`
- **Payload:**

  ```json
  {
    "status": "true/false",
    "message": ""
  }
  ```

## Device Control

### Control Device

- **Topic:** `user/{{user_id}}/home/{{home_id}}/node/{{node_id}}/device/{{device_id}}/control`
- **Payload:**

  ```json
  {
    "device_id": "",
    "state": ""
  }
  ```

### Device State Reporting

- **Topic:** `user/{{user_id}}/home/{{home_id}}/node/{{node_id}}/device/{{device_id}}/state`
- **Payload:**

  ```json
  {
    "device_id": "",
    "state": ""
  }
  ```

## See Team Contributions here

[https://github.com/Jarvis-AI-project/JARVARIAN-FIRMWARE-HOME-AUTOMATION/graphs/contributors](https://github.com/Jarvis-AI-project/JARVARIAN-FIRMWARE-HOME-AUTOMATION/graphs/contributors)
