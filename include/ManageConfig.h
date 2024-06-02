// Path: include/ManageConfig.h

#ifndef MANAGECONFIG_H
#define MANAGECONFIG_H

#include <string>
#include <vector>
#include <array>

class ManageConfig
{
private:
    std::string wifi_ssid;
    std::string wifi_password;
    std::string mqtt_broker;
    std::string mqtt_port;
    std::string mqtt_user;
    std::string mqtt_password;
    std::string mqtt_topic;
    std::array<std::string *, 7> fields;
    const char *keys[7] = {"wifi_ssid", "wifi_password", "mqtt_broker", "mqtt_port", "mqtt_user", "mqtt_password", "mqtt_topic"};

public:
    ManageConfig();

    const std::string &getWiFiSSID() const;
    const std::string &getWiFiPassword() const;
    const std::string &getMQTTBroker() const;
    const std::string &getMQTTPort() const;
    const std::string &getMQTTUser() const;
    const std::string &getMQTTPassword() const;
    const std::string &getMQTTTopic() const;

    void setWiFiSSID(const std::string &value);
    void setWiFiPassword(const std::string &value);
    void setMQTTBroker(const std::string &value);
    void setMQTTPort(const std::string &value);
    void setMQTTUser(const std::string &value);
    void setMQTTPassword(const std::string &value);
    void setMQTTTopic(const std::string &value);

    std::vector<std::string> formatString(const std::string &str, char delim);

    void writeConfig();
    void readConfig();
};

#endif // MANAGECONFIG_H