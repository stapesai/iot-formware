// Path: include/MQTTManager.hpp

#ifndef MQTT_MANAGER_HPP
#define MQTT_MANAGER_HPP

#include <Constants.hpp>
#include <NVSManager.hpp>
#include <PubSubClient.h>
#include <Utils.hpp>

class MQTTManager
{
private:
	PubSubClient* mqttclient;
	NVSManager* manage_config;
	Utils* generate_client_id;
	std::string mqtt_client_id;
	const std::string* mqtt_broker;
	const std::string* mqtt_port;
	const std::string* mqtt_password;
	const std::string* user_id;
	// TODO: Remove MQTT topic and user user's UUID
	

public:
	MQTTManager(PubSubClient* client, NVSManager* config, Utils* utils);
	void publish_keep_alive_message(unsigned long& keep_alive_counter);
	void mqttCallback(char* topic, byte* payload, unsigned short int length);
	void initializeMQTT();
	void connectToMQTT();
};

#endif // MQTT_MANAGER_HPP
