// Path: include/MQTTManager.hpp

#ifndef MQTT_MANAGER_HPP
#define MQTT_MANAGER_HPP


#include <Utils.hpp>
#include <Constants.hpp>
#include <NVSManager.hpp>
#include <PubSubClient.h>

class MQTTManager
{
private:
	PubSubClient* mqttclient;
	NVSManager* manage_config;
	Utils* generate_client_id;
	std::string mqtt_client_id;
	const std::string* mqtt_broker;
	const std::string* mqtt_port;
	const std::string* mqtt_username;
	const std::string* mqtt_password;
	const std::string* mqtt_topic;
	// TODO: Remove MQTT topic and user user's UUID
	// const char *user_uuid;

public:
	MQTTManager(PubSubClient* client, NVSManager* config, Utils* utils);
	void publish_keep_alive_message(unsigned long& keep_alive_counter);
	void mqttCallback(char* topic, byte* payload, unsigned short int length);
	void initializeMQTT();
	void connectToMQTT();
};

#endif // MQTT_MANAGER_HPP
