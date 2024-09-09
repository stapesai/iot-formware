// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/*
 * @file include/MQTTManager.hpp
 * @brief This file defines the MQTTManager class, which handles
 * all communication with the MQTT broker.
 */
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
	void publishKeepAliveMessage(unsigned long& keepAliveCounter);
	void initializeMQTT();
	void connectToMQTT();
	void publishChannelState(int channelNumber, bool state, bool retained = true);

private:
	PubSubClient* mqttclient;
	NVSManager* config;
	Utils* utils;
	const std::string* mqttBroker;
	const std::string* mqttPort;
	const std::string* mqttUsername;
	const std::string* mqttPassword;
	const std::string* userId;
	const std::string nodeMac;
	const std::string keepAliveTopic;
};

#endif // MQTT_MANAGER_HPP