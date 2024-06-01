// Path: include/MQTTManager.h




#include <PubSubClient.h>
#include <ManageConfig.h>   
class MQTTManager
{
private:
    // TODO: Do not take the values from the config object directly, instead take pointers to the values.
    PubSubClient *mqttclient;
    ManageConfig *manage_config;
    const char *mqtt_broker ;
    const char *mqtt_port ;
    const char *mqtt_client_id ;
    const char *mqtt_username ;
    const char *mqtt_password;
    // const char *user_uuid;
    // TODO: Remove MQTT topic and user user's UUID
    const char *mqtt_topic ;

public:
    // TODO: Move this function to utils.h
    MQTTManager(PubSubClient *client,ManageConfig *config);
    // format client function it defintion should be in the source file
    std::string formatClientID(std::string &macAddress);
    void publish_keep_alive_message(unsigned long &keep_alive_counter);
    void mqttCallback(char *topic, byte *payload, unsigned short int length);
    void connectToMQTT();

};