// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// Path: src/MQTTManager.cpp

#include <MQTTManager.hpp>

// Constructor
MQTTManager::MQTTManager(PubSubClient* client, NVSManager* config, Utils* utils)
	: utils(utils)
	, mqttclient(client)
	, config(config)
	, mqtt_broker(&config->getMQTTBroker())
	, mqtt_port(&config->getMQTTPort())
	, mqtt_username(&config->getMQTTUsername())
	, mqtt_password(&config->getMQTTPassword())
	, user_id(&config->getUserId())
	, node_mac(utils->getMacAddr())
	, keep_alive_topic("user/" + *user_id + "/node/" + node_mac + "/keepalive")
{ }

void MQTTManager::publish_keep_alive_message(unsigned long& keep_alive_counter)
{
	std::map<std::string, std::string> kv_map = {
		{"ORIGIN", node_mac}, {"COUNTER", std::to_string(keep_alive_counter)}, {"UPTIME", std::to_string(millis() / 1000)} // uptime in seconds
	};

	std::string kv_string = utils->convertToKVString(kv_map);

	if(mqttclient->publish(keep_alive_topic.c_str(), kv_string.c_str()))
	{
		Logger::log(INFO, GREEN "Keep-alive message published successfully: " + kv_string + RESET_COLOR);
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
		publish_keep_alive_message(temp);
		Logger::log(INFO, GREEN "Keep alive request received" RESET_COLOR);
	}

	// Check if the topic is a channel control request
	if(topicStr.endsWith("/control"))
	{
		std::vector<std::string> topic_parts = utils->splitString(topicStr.c_str(), '/');
		std::string channel_id = topic_parts[topic_parts.size() - 2];

		std::map<std::string, std::string> kv_map = utils->convertToKVMap(payloadStr.c_str());
		bool target_state = (kv_map["STATE"] == "1" || kv_map["STATE"] == "ON");

		Logger::log(DEBUG, GREEN "Channel ID: " + channel_id + RESET_COLOR);
		Logger::log(DEBUG, GREEN "Target State: " + String(target_state) + RESET_COLOR);

		if(config->controlChannel(channel_id, target_state))
		{
			Logger::log(INFO, GREEN "Channel controlled successfully: " + channel_id + RESET_COLOR);
			publishChannelState(channel_id, target_state);
		}
		else
		{
			Logger::log(ERROR, RED "Failed to control Channel ID: " + channel_id + RESET_COLOR);
		}
	}
}

void MQTTManager::publishChannelState(const std::string& channel_id, bool state)
{
	std::string topic = "user/" + *user_id + "/node/" + node_mac + "/channel/" + channel_id + "/state";
	std::string payload = state ? "ON" : "OFF";

	if(mqttclient->publish(topic.c_str(), payload.c_str()))
	{
		Logger::log(INFO, GREEN "Channel state published successfully: " + channel_id + " - " + payload + RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "Failed to publish channel state: " + channel_id + RESET_COLOR);
	}
}

void MQTTManager::initializeMQTT()
{
	mqttclient->setServer(mqtt_broker->c_str(), atoi(mqtt_port->c_str()));
	mqttclient->setKeepAlive(60);
	// TODO: Can this be done more elegantly?
	mqttclient->setCallback([&](char* topic, byte* payload, unsigned short int length) { this->mqttCallback(topic, payload, length); });
}

void MQTTManager::connectToMQTT()
{
	Logger::log(INFO, YELLOW "Connecting to MQTT Broker" RESET_COLOR);

	if(mqttclient->connect(node_mac.c_str(), mqtt_username->c_str(), mqtt_password->c_str()))
	{
		Logger::log(INFO, GREEN "Connected to MQTT Broker" RESET_COLOR);

		// Subscribe to topics
		// 1. Keep alive request topic (user/<user_id>/node/<node_id>/keepalive/request)
		const std::string keep_alive_request_topic = "user/" + *user_id + "/node/" + node_mac + "/keepalive/request";
		mqttclient->subscribe(keep_alive_request_topic.c_str());
		Logger::log(INFO, BLUE "Subscribed to topic: " + keep_alive_request_topic + RESET_COLOR);

		// 2. Channel control request topic (user/<user_id>/node/<node_id>/channel/<channel_id>/control/request)
		const std::string channel_control_request_topic = "user/" + *user_id + "/node/" + node_mac + "/channel/" + "+" + "/control";
		mqttclient->subscribe(channel_control_request_topic.c_str());
		Logger::log(INFO, BLUE "Subscribed to topic: " + channel_control_request_topic + RESET_COLOR);
	}
	else
	{
		Logger::log(DEBUG, YELLOW "MQTT Broker: " + String(mqtt_broker->c_str()) + ":" + String(mqtt_port->c_str()) + RESET_COLOR);
		Logger::log(DEBUG, YELLOW "MQTT Client ID: " + String(node_mac.c_str()) + RESET_COLOR);
		Logger::log(DEBUG, YELLOW "MQTT Username: " + String(mqtt_username->c_str()) + RESET_COLOR);
		Logger::log(DEBUG, YELLOW "MQTT Password: " + String(mqtt_password->c_str()) + RESET_COLOR);
		Logger::log(WARN, ORANGE "MQTT connection failed" RESET_COLOR);
		Logger::log(ERROR, RED "Error code: " + String(mqttclient->state()) + RESET_COLOR);
	}
}
