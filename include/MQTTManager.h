#include <vector>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ManageConfig.h>

ManageConfig config;
WiFiClient wifiClient;
PubSubClient mqttclient(wifiClient);
String macAddress = WiFi.macAddress().c_str();

class MQTTManager
{
private:
    const char *mqtt_port = config.getMQTTPort().c_str();
    const char *mqtt_client_id = formatClientID();
    const char *mqtt_broker = config.getMQTTServer().c_str();
    const char *mqtt_username = config.getMQTTUser().c_str();
    const char *mqtt_password = config.getMQTTPassword().c_str();
    const char *user_uuid;
    const char *mqtt_topic = config.getMQTTTopic().c_str();

public:
    const char *formatClientID()
    {
        std::replace(macAddress.begin(), macAddress.end(), ':', '_'); // Replace all ':' characters with '_'
        return macAddress.c_str();
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

    void mqttCallback(char *topic, byte *payload, unsigned short int length){};

    void connectToMQTT()
    {
        Serial.println("Connecting to MQTT Broker");
        Serial.println("Broker: " + String(mqtt_broker) + " Port: " + String(mqtt_port) + " ClientID: " + String(mqtt_client_id) + " Username: " + String(mqtt_username) + " Password: " + String(mqtt_password));

        mqttclient.setServer(mqtt_broker, atoi(mqtt_port) );
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
