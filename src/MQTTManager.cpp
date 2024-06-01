// Path: src/MQTTManager.cpp
// Description: This file contains the implementation of the MQTTManager class.

// TODO: Remove code from `MQTTManager.h` and implement the methods here
#include <vector>

#include <ArduinoJson.h>
#include <MQTTManager.h>


// Constructor
MQTTManager::MQTTManager(PubSubClient *client, ManageConfig *config)
{
    mqttclient = client;
    manage_config = config;
    mqtt_broker = manage_config->getMQTTServer();
    mqtt_port = manage_config->getMQTTPort();
    // mqtt_client_id = manage_config->getMQTTClientID();
    mqtt_username = manage_config->getMQTTUser();
    mqtt_password = manage_config->getMQTTPassword();
    mqtt_topic = manage_config->getMQTTTopic();
}

std::string MQTTManager::formatClientID(std::string &macAddress)
{
    std::replace(macAddress.begin(), macAddress.end(), ':', '_');
    return macAddress.c_str();
}

// TODO: This is a header file and you should not have the implementation here.
void MQTTManager::publish_keep_alive_message(unsigned long &keep_alive_counter)
{
    StaticJsonDocument<256> doc;
    doc["origin"] = mqtt_client_id;
    doc["messageType"] = "keep_alive";
    doc["message"]["keep_alive_counter"] = keep_alive_counter;
    doc["message"]["uptime"] = millis() / 1000; // uptime in seconds

    String docString;
    serializeJson(doc, docString);
    mqttclient->publish(mqtt_topic, docString.c_str());
}
void MQTTManager::mqttCallback(char *topic, byte *payload, unsigned short int length) {};

// TODO:  Move this to the source file.
// TODO: Make diffrent funcs for:
// 1. Initlialize MQTT
// 2. Connect to MQTT
// 3. Reconnect to MQTT
void MQTTManager::connectToMQTT()
{
    Serial.println("Connecting to MQTT Broker");
    Serial.println("Broker: " + String(mqtt_broker) + " Port: " + String(mqtt_port) + " ClientID: " + String(mqtt_client_id) + " Username: " + String(mqtt_username) + " Password: " + String(mqtt_password));

    mqttclient->setServer(mqtt_broker, atoi(mqtt_port));
    mqttclient->setKeepAlive(60);
    mqttclient->setCallback([this](char *topic, uint8_t *payload, unsigned int length)
                           { this->mqttCallback(topic, payload, length); });

    if (mqttclient->connect(mqtt_client_id, mqtt_username, mqtt_password))
    {
        mqttclient->subscribe(mqtt_topic);
        Serial.println("Connected to MQTT Broker");
        Serial.print("Subscribed to topic: ");
        Serial.println(mqtt_topic);
    }
    else
    {
        Serial.println("MQTT connection failed");
        Serial.print("Error code: ");
        Serial.println(mqttclient->state());
        // statusBuzzer(5, 100);
    }
}
