// Path: include/MQTTManager.hpp

#ifndef MQTT_MANAGER_HPP
#define MQTT_MANAGER_HPP

#include <Constants.hpp>
#include <Logger.hpp>
#include <NVSManager.hpp>
#include <PubSubClient.h>
#include <Utils.hpp>

class MQTTManager
{
public:
	MQTTManager(PubSubClient* client, NVSManager* config, Utils* utils);
	void mqttCallback(char* topic, byte* payload, unsigned short int length);
	void publish_keep_alive_message(unsigned long& keep_alive_counter);
	void initializeMQTT();
	void connectToMQTT();

private:
	PubSubClient* mqttclient;
	NVSManager* config;
	Utils* utils;
	const std::string* mqtt_broker;
	const std::string* mqtt_port;
	const std::string* mqtt_password;
	const std::string* user_id;
	const std::string* home_id;
	const std::string node_mac;
	const std::string keep_alive_topic;
};

#endif // MQTT_MANAGER_HPP
