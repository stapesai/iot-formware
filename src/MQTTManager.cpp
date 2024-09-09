// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/*
 * @file src/MQTTManager.cpp
 * @brief This file contains the implementation of the MQTTManager class.
 */
#include <MQTTManager.hpp>

// Constructor
MQTTManager::MQTTManager(PubSubClient* client, NVSManager* config, Utils* utils)
	: utils(utils)
	, mqttclient(client)
	, config(config)
	, mqttBroker(&config->getMQTTBroker())
	, mqttPort(&config->getMQTTPort())
	, mqttUsername(&config->getMQTTUsername())
	, mqttPassword(&config->getMQTTPassword())
	, userId(&config->getUserId())
	, nodeMac(utils->getMacAddr())
	, keepAliveTopic("user/" + *userId + "/node/" + nodeMac + "/keepalive")
{ }

void MQTTManager::publishKeepAliveMessage(unsigned long& keepAliveCounter)
{
	std::map<std::string, std::string> kvMap = {
		{"ORIGIN", nodeMac}, {"COUNTER", std::to_string(keepAliveCounter)}, {"UPTIME", std::to_string(millis() / 1000)} // uptime in seconds
	};

	std::string kvString = utils->convertToKVString(kvMap);

	if(mqttclient->publish(keepAliveTopic.c_str(), kvString.c_str()))
	{
		Logger::log(INFO, GREEN "Keep-alive message published successfully: " + kvString + RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "Failed to publish keep-alive message." RESET_COLOR);
	}
}

void MQTTManager::mqttCallback(char* topic, byte* payload, unsigned short int length)
{
	String topicStr = String(topic);
	std::string payloadStr(reinterpret_cast<char*>(payload), length);

	Logger::log(INFO, GREEN "Message arrived [" + topicStr + "]" RESET_COLOR);
	Logger::log(INFO, GREEN "Payload: " + payloadStr + RESET_COLOR);

	// Check if the topic is a keep alive request
	if(topicStr.endsWith("/keepalive/request"))
	{
		// Publish the keep alive message
		// FIXME: Use global keep alive counter
		unsigned long temp = 0;
		publishKeepAliveMessage(temp);
		Logger::log(INFO, GREEN "Keep alive request received" RESET_COLOR);
	}

	// Check if the topic is a channel control request
	if(topicStr.endsWith("/control"))
	{
		std::vector<std::string> topicParts = utils->splitString(topicStr.c_str(), '/');
		std::string channelNumber = topicParts[topicParts.size() - 2];

		bool targetState = (payloadStr == "ON");

		Logger::log(DEBUG, GREEN "Channel Number: " + channelNumber + RESET_COLOR);
		Logger::log(DEBUG, GREEN "Target State: " + String(targetState) + RESET_COLOR);

		if(config->controlChannel(std::stoi(channelNumber), targetState))
		{
			Logger::log(INFO, GREEN "Channel controlled successfully: " + channelNumber + RESET_COLOR);
			publishChannelState(std::stoi(channelNumber), targetState);
		}
		else
		{
			Logger::log(ERROR, RED "Failed to control Channel Number: " + channelNumber + RESET_COLOR);
		}
	}
}

void MQTTManager::publishChannelState(int channelNumber, bool state, bool retained)
{
	std::string topic = "user/" + *userId + "/node/" + nodeMac + "/channel/" + std::to_string(channelNumber) + "/state";
	std::string payload = state ? "ON" : "OFF";

	if(mqttclient->publish(topic.c_str(), payload.c_str(), retained))
	{
		Logger::log(INFO, GREEN "Channel state published successfully: " + std::to_string(channelNumber) + " - " + payload + RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "Failed to publish channel state: " + std::to_string(channelNumber) + RESET_COLOR);
	}
}

void MQTTManager::initializeMQTT()
{
	mqttclient->setServer(mqttBroker->c_str(), atoi(mqttPort->c_str()));
	mqttclient->setKeepAlive(60);
	// TODO: Can this be done more elegantly?
	mqttclient->setCallback([&](char* topic, byte* payload, unsigned short int length) { this->mqttCallback(topic, payload, length); });
}

void MQTTManager::connectToMQTT()
{
	Logger::log(INFO, YELLOW "Connecting to MQTT Broker" RESET_COLOR);

	if(mqttclient->connect(nodeMac.c_str(), mqttUsername->c_str(), mqttPassword->c_str()))
	{
		Logger::log(INFO, GREEN "Connected to MQTT Broker" RESET_COLOR);

		// Subscribe to topics
		// 1. Keep alive request topic
		// (user/<user_id>/node/<node_id>/keepalive/request)
		const std::string keepAliveRequestTopic = "user/" + *userId + "/node/" + nodeMac + "/keepalive/request";
		mqttclient->subscribe(keepAliveRequestTopic.c_str());
		Logger::log(INFO, BLUE "Subscribed to topic: " + keepAliveRequestTopic + RESET_COLOR);

		// 2. Channel control request topic
		// (user/<user_id>/node/<node_id>/channel/<channel_number>/control/request)
		const std::string channelControlRequestTopic = "user/" + *userId + "/node/" + nodeMac + "/channel/" + "+" + "/control";
		mqttclient->subscribe(channelControlRequestTopic.c_str());
		Logger::log(INFO, BLUE "Subscribed to topic: " + channelControlRequestTopic + RESET_COLOR);
	}
	else
	{
		Logger::log(DEBUG, YELLOW "MQTT Broker: " + String(mqttBroker->c_str()) + ":" + String(mqttPort->c_str()) + RESET_COLOR);
		Logger::log(DEBUG, YELLOW "MQTT Client ID: " + String(nodeMac.c_str()) + RESET_COLOR);
		Logger::log(DEBUG, YELLOW "MQTT Username: " + String(mqttUsername->c_str()) + RESET_COLOR);
		Logger::log(DEBUG, YELLOW "MQTT Password: " + String(mqttPassword->c_str()) + RESET_COLOR);
		Logger::log(WARN, ORANGE "MQTT connection failed" RESET_COLOR);
		Logger::log(ERROR, RED "Error code: " + String(mqttclient->state()) + RESET_COLOR);
	}
}
