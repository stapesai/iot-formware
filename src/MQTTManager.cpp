// Path: src/MQTTManager.cpp

#include <Logger.hpp>
#include <MQTTManager.hpp>

// Constructor
MQTTManager::MQTTManager(PubSubClient* client, NVSManager* config, Utils* utils)
	: mqttclient(client)
	, mqtt_broker(&config->getMQTTBroker())
	, mqtt_port(&config->getMQTTPort())
	, mqtt_password(&config->getMQTTPassword())
	, user_id(&config->getUserId())
	, utils(utils)
{
	mqtt_client_id = utils->generateName();
}

void MQTTManager::publish_keep_alive_message(unsigned long& keep_alive_counter)
{
	std::map<std::string, std::string> kv_map = {
		{"ORIGIN", mqtt_client_id},
		{"COUNTER", std::to_string(keep_alive_counter)},
		{"UPTIME", std::to_string(millis() / 1000)} // uptime in seconds
	};

	std::string kv_string = utils->convertToKVString(kv_map);

	if(mqttclient->publish("keep_alive", kv_string.c_str()))
	{
		Logger::log(INFO,
					GREEN "Keep-alive message published successfully: " + kv_string + RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "Failed to publish keep-alive message." RESET_COLOR);
	}
}

void MQTTManager::mqttCallback(char* topic, byte* payload, unsigned short int length) {};

void MQTTManager::initializeMQTT()
{
	mqttclient->setServer(mqtt_broker->c_str(), atoi(mqtt_port->c_str()));
	mqttclient->setKeepAlive(60);
	mqttclient->setCallback([this](char* topic, uint8_t* payload, unsigned int length) {
		this->mqttCallback(topic, payload, length);
	});
}

void MQTTManager::connectToMQTT()
{
	Logger::log(INFO, YELLOW "Connecting to MQTT Broker" RESET_COLOR);

	if(mqttclient->connect(mqtt_client_id.c_str(), mqtt_client_id.c_str(), mqtt_password->c_str()))
	{
		mqttclient->subscribe(user_id->c_str());
		Logger::log(INFO, GREEN "Connected to MQTT Broker" RESET_COLOR);
		Logger::log(INFO, BLUE "Subscribed to topic: " + String(user_id->c_str()) + RESET_COLOR);
	}
	else
	{
		Logger::log(DEBUG,
					YELLOW "MQTT Broker: " + String(mqtt_broker->c_str()) + ":" +
						String(mqtt_port->c_str()) + RESET_COLOR);
		Logger::log(DEBUG,
					YELLOW "MQTT Client ID: " + String(mqtt_client_id.c_str()) + RESET_COLOR);
		Logger::log(DEBUG, YELLOW "MQTT User ID: " + String(mqtt_client_id.c_str()) + RESET_COLOR);
		Logger::log(DEBUG, YELLOW "MQTT Password: " + String(mqtt_password->c_str()) + RESET_COLOR);
		Logger::log(WARN, ORANGE "MQTT connection failed" RESET_COLOR);
		Logger::log(ERROR, RED "Error code: " + String(mqttclient->state()) + RESET_COLOR);
	}
}
