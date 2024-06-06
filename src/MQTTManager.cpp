// Path: src/MQTTManager.cpp

#include <ArduinoJson.h>
#include <Logger.hpp>
#include <MQTTManager.hpp>
// Constructor
MQTTManager::MQTTManager(PubSubClient* client, NVSManager* config, Utils* utils)
{
	mqttclient = client;
	manage_config = config;
	generate_client_id = utils;
	mqtt_client_id = generate_client_id->generateName();
	mqtt_broker = &manage_config->getMQTTBroker();
	mqtt_port = &manage_config->getMQTTPort();
	mqtt_username = &manage_config->getMQTTUser();
	mqtt_password = &manage_config->getMQTTPassword();
	mqtt_topic = &manage_config->getMQTTTopic();
}

void MQTTManager::publish_keep_alive_message(unsigned long& keep_alive_counter)
{
	StaticJsonDocument<256> doc;
	doc["origin"] = mqtt_client_id;
	doc["messageType"] = "keep_alive";
	doc["message"]["keep_alive_counter"] = keep_alive_counter;
	doc["message"]["uptime"] = millis() / 1000; // uptime in seconds

	String docString;
	serializeJson(doc, docString);
	mqttclient->publish(mqtt_topic->c_str(), docString.c_str());
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
	if(mqttclient->connect(mqtt_client_id.c_str(), mqtt_username->c_str(), mqtt_password->c_str()))
	{
		mqttclient->subscribe(mqtt_topic->c_str());
		Logger::log(INFO, GREEN "Connected to MQTT Broker" RESET_COLOR);
		Logger::log(INFO, BLUE "Subscribed to topic: " + String(mqtt_topic->c_str()) + RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "MQTT connection failed" RESET_COLOR);
		Logger::log(ERROR, RED "Error code: " + String(mqttclient->state()) + RESET_COLOR);
	}
}
