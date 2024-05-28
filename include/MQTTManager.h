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
    void mqttCallback(char *topic, byte *payload, unsigned short int length)
    {

        Serial.print("Message arrived in topic: ");
        // Serial.println(topic);
        Serial.print("Message:");
        for (int i = 0; i < length; i++)
        {
            Serial.print((char)payload[i]);
        }
        Serial.println();
    };

    void connectToMQTT()
    {

        mqttclient.setServer(config.getSSID().c_str(), atoi(config.getMQTTPort().c_str()));
        mqttclient.setKeepAlive(60);
        mqttclient.setCallback(std::bind(&MQTTManager::mqttCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        if (mqttclient.connect(config.getSSID().c_str(), config.getMQTTUser().c_str(), config.getMQTTPassword().c_str()))
        {
            mqttclient.subscribe(config.getMQTTTopic().c_str());
            Serial.println("Connected to MQTT Broker");
            Serial.print("Subscribed to topic: ");
            Serial.println(config.getMQTTTopic().c_str());
        }
        else
        {
            Serial.println("MQTT connection failed");
            Serial.print("Error code: ");
            Serial.println(mqttclient.state());
            // statusBuzzer(5, 100);
        }
    }
};
