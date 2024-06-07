// Path: include/MQTTManager.hpp

#ifndef MQTT_MANAGER_HPP
#define MQTT_MANAGER_HPP

#include <Constants.hpp>
#include <NVSManager.hpp>
#include <PubSubClient.h>
#include <Utils.hpp>

class MQTTManager
{
public:
	MQTTManager(PubSubClient* client, NVSManager* config, Utils* utils);
	static void mqttCallback(char* topic, byte* payload, unsigned short int length);
	void publish_keep_alive_message(unsigned long& keep_alive_counter);
	void initializeMQTT();
	void connectToMQTT();

private:
	Utils* utils;
	PubSubClient* mqttclient;
	bool parseTopic(const String& topic, String& user_id, String& home_id, String& node_id);
	const std::string* mqtt_broker;
	const std::string* mqtt_port;
	const std::string* mqtt_password;
	const std::string* user_id;
	std::string mqtt_client_id;
	std::string node_id;
	std::string home_id;
};

#endif // MQTT_MANAGER_HPP
