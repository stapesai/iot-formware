// Path: include/ManageConfig.h

#ifndef MANAGECONFIG_H
#define MANAGECONFIG_H

#include <string>
#include <vector>
#include <array>

class ManageConfig
{
private:
    const char *wifi_ssid;
    const char *wifi_password;
    const char *mqtt_server;
    const char *mqtt_port;
    const char *mqtt_user;
    const char *mqtt_password;
    const char *mqtt_topic;
    std::array<const char *, 7> fields;
    const char *keys[7] = {"wifi_ssid", "wifi_password", "mqtt_server", "mqtt_port", "mqtt_user", "mqtt_password", "mqtt_topic"};

public:
    ManageConfig();

    const char * getWiFiSSID() const;
    const char * getWiFiPassword() const;
    const char * getMQTTServer() const;
    const char * getMQTTPort() const;
    const char * getMQTTUser() const;
    const char * getMQTTPassword() const;
    const char * getMQTTTopic() const;

    void setWiFiSSID(const char * value);
    void setWiFiPassword(const char * value);
    void setMQTTServer(const char * value);
    void setMQTTPort(const char * value);
    void setMQTTUser(const char * value);
    void setMQTTPassword(const char * value);
    void setMQTTTopic(const char * value);

    std::vector<char *> formatString(const char * str, char delim);

    void writeConfig();
    void readConfig();
};

#endif // MANAGECONFIG_H
