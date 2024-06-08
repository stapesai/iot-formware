// Path: src/MQTTManager.cpp

#include <Logger.hpp>
#include <MQTTManager.hpp>

// Constructor
MQTTManager::MQTTManager(PubSubClient* client, NVSManager* config, Utils* utils)
	: utils(utils)
	, mqttclient(client)
	, config(config)
	, mqtt_broker(&config->getMQTTBroker())
	, mqtt_port(&config->getMQTTPort())
	, mqtt_password(&config->getMQTTPassword())
	, user_id(&config->getUserId())
	, home_id(&config->getHomeId())
	, node_mac(utils->getMacAddr())
{ }

void MQTTManager::publish_keep_alive_message(unsigned long& keep_alive_counter)
{
	std::map<std::string, std::string> kv_map = {
		{"ORIGIN", node_mac},
		{"COUNTER", std::to_string(keep_alive_counter)},
		{"UPTIME", std::to_string(millis() / 1000)} // uptime in seconds
	};

	std::string kv_string = utils->convertToKVString(kv_map);

	const std::string keep_alive_topic =
		"user/" + *user_id + "/home/" + *home_id + "/node/" + node_mac + "/keepalive";
	if(mqttclient->publish(keep_alive_topic.c_str(), kv_string.c_str()))
	{
		Logger::log(INFO,
					GREEN "Keep-alive message published successfully: " + kv_string + RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "Failed to publish keep-alive message." RESET_COLOR);
	}
}

void MQTTManager::mqttCallback(char* topic, byte* payload, unsigned short int length)
{
	String topicStr = String(topic);
	String payloadStr = String((char*)payload);

	Logger::log(INFO, GREEN "Message arrived [" + topicStr + "]" RESET_COLOR);
	Logger::log(INFO, GREEN "Payload: " + payloadStr + RESET_COLOR);
};

void MQTTManager::initializeMQTT()
{
	mqttclient->setServer(mqtt_broker->c_str(), atoi(mqtt_port->c_str()));
	mqttclient->setKeepAlive(60);
	mqttclient->setCallback(mqttCallback);
}

void MQTTManager::connectToMQTT()
{
	Logger::log(INFO, YELLOW "Connecting to MQTT Broker" RESET_COLOR);

	if(mqttclient->connect(node_mac.c_str(), node_mac.c_str(), mqtt_password->c_str()))
	{
		Logger::log(INFO, GREEN "Connected to MQTT Broker" RESET_COLOR);

		// Subscribe to topics
		// 1. Keep alive request topic (user/<user_id>/home/<home_id>/node/<node_id>/keepalive/request)
		const std::string keep_alive_request_topic =
			"user/" + *user_id + "/home/" + *home_id + "/node/" + node_mac + "/keepalive/request";
		mqttclient->subscribe(keep_alive_request_topic.c_str());
		Logger::log(INFO, BLUE "Subscribed to topic: " + keep_alive_request_topic + RESET_COLOR);

		// 2. Device registration request topic (user/<user_id>/home/<home_id>/node/<node_id>/device/register/request)
		const std::string device_registration_request_topic = "user/" + *user_id + "/home/" +
															  *home_id + "/node/" + node_mac +
															  "/device/register/request";
		mqttclient->subscribe(device_registration_request_topic.c_str());
		Logger::log(INFO,
					BLUE "Subscribed to topic: " + device_registration_request_topic + RESET_COLOR);

		// 3. Device unregistration request topic (user/<user_id>/home/<home_id>/node/<node_id>/device/unregister/request)
		const std::string device_unregistration_request_topic = "user/" + *user_id + "/home/" +
																*home_id + "/node/" + node_mac +
																"/device/unregister/request";
		mqttclient->subscribe(device_unregistration_request_topic.c_str());
		Logger::log(
			INFO, BLUE "Subscribed to topic: " + device_unregistration_request_topic + RESET_COLOR);

		// 4. Device control request topic (user/<user_id>/home/<home_id>/node/<node_id>/device/<device_id>/control/request)
		// We get registered device ids from the NVS `REG_DEVICE_IDS` key (it can also be null) (`;` separated)
		const std::string reg_device_ids = config->getRegisteredDevicesIds();
		std::vector<std::string> device_ids;
		if(reg_device_ids.length() > 0)
		{
			device_ids = utils->splitString(reg_device_ids, ';');
		}
		for(const auto& device_id : device_ids)
		{
			const std::string device_control_request_topic = "user/" + *user_id + "/home/" +
															 *home_id + "/node/" + node_mac +
															 "/device/" + device_id + "/control";
			mqttclient->subscribe(device_control_request_topic.c_str());
			Logger::log(INFO,
						BLUE "Subscribed to topic: " + device_control_request_topic + RESET_COLOR);
		}
	}
	else
	{
		Logger::log(DEBUG,
					YELLOW "MQTT Broker: " + String(mqtt_broker->c_str()) + ":" +
						String(mqtt_port->c_str()) + RESET_COLOR);
		Logger::log(DEBUG, YELLOW "MQTT Client ID: " + String(node_mac.c_str()) + RESET_COLOR);
		Logger::log(DEBUG, YELLOW "MQTT Password: " + String(mqtt_password->c_str()) + RESET_COLOR);
		Logger::log(WARN, ORANGE "MQTT connection failed" RESET_COLOR);
		Logger::log(ERROR, RED "Error code: " + String(mqttclient->state()) + RESET_COLOR);
	}
}
