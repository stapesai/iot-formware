#include <PubSubClient.h>
#include <vector>
#include <WiFi.h>

WiFiClient wifiClient;
PubSubClient mqttclient(wifiClient);

class MQTTManager
{
    private:
        const char *client_mac_id;
        const char *mqtt_broker;
        const char *mqtt_port;
        const char *mqtt_username;
        const char *mqtt_password;
        const char *user_uuid;
        const char *mqtt_topic;

    public:
        const char *validate() const
        {
            if (client_mac_id == nullptr || strlen(client_mac_id) == 0)
                return "clientID";
            if (mqtt_broker == nullptr || strlen(mqtt_broker) == 0)
                return "mqtt_broker";
            if (mqtt_port == nullptr || strlen(mqtt_port) == 0)
                return "mqtt_port";
            if (mqtt_username == nullptr || strlen(mqtt_username) == 0)
                return "mqtt_username";
            if (mqtt_password == nullptr || strlen(mqtt_password) == 0)
                return "mqtt_password";
            if (user_uuid == nullptr || strlen(user_uuid) == 0)
                return "user_uuid";
            return nullptr; // All fields are valid
        }
        void connectToMQTT(void (*callback)(char *, byte *, unsigned short int))
        {
            const char *server = mqtt_broker;
            int port = atoi(mqtt_port);
            mqttclient.setServer(server, port);
            mqttclient.setKeepAlive(60);
            mqttclient.setCallback(callback);

            if (mqttclient.connect(client_mac_id, mqtt_username, mqtt_password))
            {
                mqttclient.subscribe(mqtt_topic);
                Serial.println("Connected to MQTT Broker");
                Serial.print("Subscribed to topic: ");
                Serial.println(mqtt_topic);
            }
            else
            {
                Serial.println("MQTT connection failed");
                Serial.print("Error code: ");
                Serial.println(mqttclient.state());
                statusBuzzer(5, 100);
            }
        }

};

// void publish_keep_alive_message(unsigned long &keep_alive_counter)
// {
//     StaticJsonDocument<256> doc;
//     doc["origin"] = client_mac_id;
//     doc["messageType"] = "keep_alive";
//     doc["message"]["keep_alive_counter"] = keep_alive_counter;
//     doc["message"]["uptime"] = millis() / 1000; // uptime in seconds

//     String docString;
//     serializeJson(doc, docString);
//     mqttclient.publish(mqtt_topic, docString.c_str());
// }

// void publish_current_state_message(int8_t statusPin, int8_t state, String deviceName)
// {
//     StaticJsonDocument<256> doc;
//     doc["origin"] = client_mac_id;
//     doc["messageType"] = "current_state";
//     doc["message"]["device_id"] = deviceName;
//     if (state == -1)
//         doc["message"]["state"] = digitalRead(statusPin) ? "ON" : "OFF";
//     else
//         doc["message"]["state"] = state ? "ON" : "OFF";

//     String docString;
//     serializeJson(doc, docString);
//     Serial.println("Response: " + docString);
//     mqttclient.publish(mqtt_topic, docString.c_str());
// }

// void publish_error_message(String message)
// {
//     StaticJsonDocument<256> doc;
//     doc["origin"] = client_mac_id;
//     doc["messageType"] = "error";
//     doc["message"]["message"] = message;

//     String docString;
//     serializeJson(doc, docString);
//     Serial.println("Response: " + docString);
//     mqttclient.publish(mqtt_topic, docString.c_str());
// }

// void mqttCallback(char *topic, byte *payload, unsigned short int length)
// {
//     // Convert payload to a JSON string
//     String message;
//     for (unsigned int i = 0; i < length; i++)
//     {
//         message += (char)payload[i];
//     }

//     // Parse JSON string to extract message details
//     DynamicJsonDocument messageDoc(256);
//     DeserializationError error = deserializeJson( messageDoc, message);
//     if (error)
//     {
//         Serial.println("Not a valid JSON message");
//         publish_error_message("Not a valid JSON message");
//         return;
//     }

//     // Extract message details
//     if (messageDoc["origin"].as<String>().equals(client_mac_id))
//         return;

//     Serial.println("-----------------------");
//     Serial.println("Message arrived in topic: " + String(topic));
//     Serial.println("Message: " + String(message));

//     // Handle change_state messages
//     if (messageDoc["messageType"].as<String>().equals("change_state"))
//     {
//         Serial.println("Message type: change_state");

//         String deviceId = messageDoc["message"]["device_id"].as<String>();
//         String state = messageDoc["message"]["state"].as<String>();

//         // Loop through the devices array in the JSON document
//         for (size_t i = 0; i < devices.size(); i++)
//         {
//             if (deviceId.equals(String(devices[i].name.c_str())))
//             {
//                 String name = devices[i].name.c_str();

//                 Serial.println("Device found in config file");
//                 Serial.println("Device name: " + name);
//                 Serial.println("Device ControlPin: " + String(devices[i].controlPin));
//                 Serial.println("Device StatusPin: " + String(devices[i].statusPin));

//                 if (state.equals("ON"))
//                 {
//                     if (devices[i].state == 1)
//                     {
//                         Serial.println("Device is already ON");
//                         publish_error_message("Device is already ON");
//                         return;
//                     }
//                     // trigerTTP223(devices[i]);
//                     devices[i].state = 1;
//                     publish_current_state_message(-1, 1, name);
//                     return;
//                 }
//                 else if (state.equals("OFF"))
//                 {
//                     if (devices[i].state == 0)
//                     {
//                         Serial.println("Device is already OFF");
//                         publish_error_message("Device is already OFF");
//                         return;
//                     }
//                     // trigerTTP223(devices[i]);
//                     devices[i].state = 0;
//                     publish_current_state_message(-1, 0, name);
//                     return;
//                 }
//                 else
//                 {
//                     Serial.println("Invalid state" + state + ". Valid states are `ON` and `OFF`");
//                     publish_error_message("Invalid state" + state + ". Valid states are `ON` and `OFF`");
//                     return;
//                 }
//             }
//         }
//         Serial.println("Device (" + deviceId + ") not found in config file");
//         publish_error_message("Device (" + deviceId + ") not found in config file");
//         return;
//     }

//     // Handle current_state messages
//     if (messageDoc["messageType"].as<String>().equals("current_state"))
//     {
//         Serial.println("Message type: current_state");

//         String deviceId = messageDoc["message"]["device_id"].as<String>();

//         // Loop through the devices array in the JSON document
//         for (size_t i = 0; i < devices.size(); i++)
//         {
//             String name = devices[i].name.c_str();

//             if (deviceId.equals(name))
//             {
//                 Serial.println("Device found in config file");
//                 Serial.println("Device name: " + name);
//                 Serial.println("Device ControlPin: " + String(devices[i].controlPin));
//                 Serial.println("Device StatusPin: " + String(devices[i].statusPin));

//                 publish_current_state_message(devices[i].statusPin, -1, name);
//                 return;
//             }
//         }
//         Serial.println("Device (" + deviceId + ") not found in config file");
//         return;
//     }
// }