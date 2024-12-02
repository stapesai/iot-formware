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
{ }

void MQTTManager::publish_keep_alive_message(unsigned long& keep_alive_counter)
{
	std::map<std::string, std::string> kv_map = {
		{"ORIGIN", node_mac}, {"COUNTER", std::to_string(keep_alive_counter)}, {"UPTIME", std::to_string(millis() / 1000)} // uptime in seconds
	};

	std::string kv_string = utils->convertToKVString(kv_map);

	const std::string keep_alive_topic = "user/" + *user_id + "/node/" + node_mac + "/keepalive";

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

	// 1. Check if the topic is a keep alive request
	if(topicStr.endsWith("/keepalive/request"))
	{
		// Publish the keep alive message
		// FIXME: Use global keep alive counter
		if(payloadStr == "GET_STATUS")
		{
			Logger::log(DEBUG, GREEN "Keep alive counter requested" RESET_COLOR);
			unsigned long temp = 0;
			publish_keep_alive_message(temp);
			return;
		}
	}

	// 2. Check if the topic is a channel control request
	if(topicStr.endsWith("/control"))
	{
		std::vector<std::string> topic_parts = utils->splitString(topicStr.c_str(), '/');
		std::string channel_number = topic_parts[topic_parts.size() - 2];

		if(topic_parts.size() < 5)
		{
			Logger::log(ERROR, RED "Invalid topic structure for channel control" RESET_COLOR);
			return;
		}

		bool target_state = (payloadStr == "ON");

		Logger::log(DEBUG, GREEN "Channel Number: " + channel_number + RESET_COLOR);
		Logger::log(DEBUG, GREEN "Target State: " + String(target_state) + RESET_COLOR);

		int channel_num;
		try
		{
			channel_num = std::stoi(channel_number);
		}
		catch(const std::exception& e)
		{
			Logger::log(ERROR, RED "Invalid channel number: " + channel_number + RESET_COLOR);
			return;
		}

		if(config->controlChannel(channel_num, target_state))
		{
			Logger::log(INFO, GREEN "Channel controlled successfully: " + channel_number + RESET_COLOR);
			publishChannelState(channel_num, target_state);
		}
		else
		{
			Logger::log(ERROR, RED "Failed to control Channel Number: " + channel_number + RESET_COLOR);
		}
	}

	// 3. Check if the topic is a channel state request
	if(topicStr.endsWith("/state/request"))
	{
		if(payloadStr != "GET_STATE")
		{
			Logger::log(ERROR, RED "Invalid payload for channel state request" RESET_COLOR);
			return;
		}

		std::vector<std::string> topic_parts = utils->splitString(topicStr.c_str(), '/');
		if(topic_parts.size() < 5)
		{
			Logger::log(ERROR, RED "Invalid topic structure for channel state request" RESET_COLOR);
			return;
		}

		std::string channel_number = topic_parts[topic_parts.size() - 3];
		int channel_num;
		try
		{
			channel_num = std::stoi(channel_number);
		}
		catch(const std::exception& e)
		{
			Logger::log(ERROR, RED "Invalid channel number: " + channel_number + RESET_COLOR);
			return;
		}

		bool state = config->getChannelState(channel_num);

		Logger::log(DEBUG, GREEN "Channel Number: " + channel_number + RESET_COLOR);
		Logger::log(DEBUG, GREEN "State: " + String(state) + RESET_COLOR);

		publishChannelState(channel_num, state);
	}
}

void MQTTManager::publishChannelState(int channel_number, bool state)
{
	std::string topic = "user/" + *user_id + "/node/" + node_mac + "/channel/" + std::to_string(channel_number) + "/state";
	std::string payload = state ? "ON" : "OFF";

	if(mqttclient->publish(topic.c_str(), payload.c_str()))
	{
		Logger::log(INFO, GREEN "Channel state published successfully: " + std::to_string(channel_number) + " - " + payload + RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "Failed to publish channel state: " + std::to_string(channel_number) + RESET_COLOR);
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

		// 2. Channel control topic (user/<user_id>/node/<node_id>/channel/<channel_number>/control)
		const std::string channel_control_request_topic = "user/" + *user_id + "/node/" + node_mac + "/channel/" + "+" + "/control";
		mqttclient->subscribe(channel_control_request_topic.c_str());
		Logger::log(INFO, BLUE "Subscribed to topic: " + channel_control_request_topic + RESET_COLOR);

		// 3. Channel request state topic (user/<user_id>/node/<node_id>/channel/<channel_number>/state/request)
		const std::string channel_state_request_topic = "user/" + *user_id + "/node/" + node_mac + "/channel/" + "+" + "/state/request";
		mqttclient->subscribe(channel_state_request_topic.c_str());
		Logger::log(INFO, BLUE "Subscribed to topic: " + channel_state_request_topic + RESET_COLOR);
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
