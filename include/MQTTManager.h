// Path: include/MQTTManager.h

#include <PubSubClient.h>
#include <NVSManager.h>
#include <utils.h>

class MQTTManager
{
private:
    PubSubClient *mqttclient;
    NVSManager *manage_config;
    Utils *generate_client_id;
    std::string mqtt_client_id;
    const std::string *mqtt_broker;
    const std::string *mqtt_port;
    const std::string *mqtt_username;
    const std::string *mqtt_password;
    const std::string *mqtt_topic;
    // TODO: Remove MQTT topic and user user's UUID
    // const char *user_uuid;

public:
    MQTTManager(PubSubClient *client, NVSManager *config, Utils *utils);
    void publish_keep_alive_message(unsigned long &keep_alive_counter);
    void mqttCallback(char *topic, byte *payload, unsigned short int length);
    void connectToMQTT();
};