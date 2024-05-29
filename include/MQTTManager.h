#include <vector>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ManageConfig.h>

ManageConfig config;
WiFiClient wifiClient;
PubSubClient mqttclient(wifiClient);

class MQTTManager
{
private:
    int mqtt_port = 1883;
    const char *mqtt_client_id="ESP32";
    const char *mqtt_broker = "192.168.29.245";
    const char *mqtt_username = "test";
    const char *mqtt_password = "test";
    const char *user_uuid;
    const char *mqtt_topic = config.getMQTTTopic().c_str();

public:
    const char *validate() const
    {
        if (mqtt_client_id == nullptr || strlen(mqtt_client_id) == 0)
            return "clientID";
        if (mqtt_broker == nullptr || strlen(mqtt_broker) == 0)
            return "mqtt_broker";
        if (mqtt_port == 0)
            return "mqtt_port";
        if (mqtt_username == nullptr || strlen(mqtt_username) == 0)
            return "mqtt_username";
        if (mqtt_password == nullptr || strlen(mqtt_password) == 0)
            return "mqtt_password";
        if (user_uuid == nullptr || strlen(user_uuid) == 0)
            return "user_uuid";
        return nullptr; // All fields are valid
    }

    void publish_keep_alive_message(unsigned long &keep_alive_counter)
    {
        StaticJsonDocument<256> doc;
        doc["origin"] = mqtt_client_id;
        doc["messageType"] = "keep_alive";
        doc["message"]["keep_alive_counter"] = keep_alive_counter;
        doc["message"]["uptime"] = millis() / 1000; // uptime in seconds

        String docString;
        serializeJson(doc, docString);
        mqttclient.publish(mqtt_topic, docString.c_str());
    }

    void publish_current_state_message(int8_t statusPin, int8_t state, String deviceName)
    {
        StaticJsonDocument<256> doc;
        doc["origin"] = mqtt_client_id;
        doc["messageType"] = "current_state";
        doc["message"]["device_id"] = deviceName;
        if (state == -1)
            doc["message"]["state"] = digitalRead(statusPin) ? "ON" : "OFF";
        else
            doc["message"]["state"] = state ? "ON" : "OFF";

        String docString;
        serializeJson(doc, docString);
        Serial.println("Response: " + docString);
        mqttclient.publish(mqtt_topic, docString.c_str());
    }

    void publish_error_message(String message)
    {
        StaticJsonDocument<256> doc;
        doc["origin"] = mqtt_client_id;
        doc["messageType"] = "error";
        doc["message"]["message"] = message;

        String docString;
        serializeJson(doc, docString);
        Serial.println("Response: " + docString);
        mqttclient.publish(mqtt_topic, docString.c_str());
    }

    void mqttCallback(char *topic, byte *payload, unsigned short int length)
    {

        Serial.print("Message arrived in topic: ");
        Serial.println(topic);
        Serial.print("Message:");
        for (int i = 0; i < length; i++)
        {
            Serial.print((char)payload[i]);
        }
        Serial.println();
    };

    void connectToMQTT()
    {
        Serial.println("Connecting to MQTT Broker");
        Serial.println("Broker: " + String(mqtt_broker) + " Port: " + String(mqtt_port) + " ClientID: " + String(mqtt_client_id) + " Username: " + String(mqtt_username) + " Password: " + String(mqtt_password));

        mqttclient.setServer(mqtt_broker, mqtt_port);
        mqttclient.setKeepAlive(60);
        mqttclient.setCallback(std::bind(&MQTTManager::mqttCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        if (mqttclient.connect(mqtt_client_id, mqtt_username, mqtt_password))
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
