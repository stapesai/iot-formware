#ifndef MANAGECONFIG_H
#define MANAGECONFIG_H

#include <string>
#include <vector>
#include <array>

class ManageConfig
{
private:
    std::string ssid;
    std::string password;
    std::string mqtt_server;
    std::string mqtt_port;
    std::string mqtt_user;
    std::string mqtt_password;
    std::string mqtt_topic;
    std::array<std::string *, 7> fields;
    const char *keys[7] = {"ssid", "password", "mqtt_server", "mqtt_port", "mqtt_user", "mqtt_password", "mqtt_topic"};

public:
    ManageConfig();

    const std::string &getSSID() const;
    const std::string &getPassword() const;
    const std::string &getMQTTServer() const;
    const std::string &getMQTTPort() const;
    const std::string &getMQTTUser() const;
    const std::string &getMQTTPassword() const;
    const std::string &getMQTTTopic() const;

    void setSSID(const std::string &value);
    void setPassword(const std::string &value);
    void setMQTTServer(const std::string &value);
    void setMQTTPort(const std::string &value);
    void setMQTTUser(const std::string &value);
    void setMQTTPassword(const std::string &value);
    void setMQTTTopic(const std::string &value);

    std::vector<std::string> formatString(const std::string &str, char delim);

    void writeConfig();
    void readConfig();
};

#endif // MANAGECONFIG_H
