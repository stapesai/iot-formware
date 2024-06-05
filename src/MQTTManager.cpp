// Path: src/MQTTManager.cpp

#include <ArduinoJson.h>
#include <MQTTManager.h>

// Constructor
MQTTManager::MQTTManager(PubSubClient *client, NVSManager *config, Utils *utils)
{
    mqttclient = client;
    manage_config = config;
    generate_client_id = utils;
    mqtt_client_id = generate_client_id->generateName();
    mqtt_broker = &manage_config->getMQTTBroker();
    mqtt_port = &manage_config->getMQTTPort();
    mqtt_username = &manage_config->getMQTTUser();
    mqtt_password = &manage_config->getMQTTPassword();
    mqtt_topic = &manage_config->getMQTTTopic();
}

void MQTTManager::publish_keep_alive_message(unsigned long &keep_alive_counter)
{
    StaticJsonDocument<256> doc;
    doc["origin"] = mqtt_client_id;
    doc["messageType"] = "keep_alive";
    doc["message"]["keep_alive_counter"] = keep_alive_counter;
    doc["message"]["uptime"] = millis() / 1000; // uptime in seconds

    String docString;
    serializeJson(doc, docString);
    mqttclient->publish(mqtt_topic->c_str(), docString.c_str());
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
    Serial.println("Broker: " + String(mqtt_broker->c_str()) + "\n Port: " + String(atoi(mqtt_port->c_str())) + "\n ClientID: " + String(mqtt_client_id.c_str()) + "\n Username: " + String(mqtt_username->c_str()) + "\n Password: " + String(mqtt_password->c_str()) + "\n Topic: " + String(mqtt_topic->c_str()));

    mqttclient->setServer(mqtt_broker->c_str(), atoi(mqtt_port->c_str()));
    mqttclient->setKeepAlive(60);
    mqttclient->setCallback([this](char *topic, uint8_t *payload, unsigned int length)
                            { this->mqttCallback(topic, payload, length); });

    if (mqttclient->connect(mqtt_client_id.c_str(), mqtt_username->c_str(), mqtt_password->c_str()))
    {
        mqttclient->subscribe(mqtt_topic->c_str());
        Serial.println("Connected to MQTT Broker");
        Serial.print("Subscribed to topic: ");
        Serial.println(mqtt_topic->c_str());
    }
    else
    {
        Serial.println("MQTT connection failed");
        Serial.print("Error code: ");
        Serial.println(mqttclient->state());
        // statusBuzzer(5, 100);
    }
}
