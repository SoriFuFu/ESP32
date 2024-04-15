#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>
#include <FS.h>

class ConfigManager {
private:
    StaticJsonDocument<2048> configDoc;
    const String configFilePath = "/config.json"; // Ruta estática del archivo JSON

public:
    ConfigManager() {}

    void loadConfig() {
        File configFile = SPIFFS.open(configFilePath, "r");
        if (!configFile) {
            Serial.println("Failed to open config file");
            return;
        }

        DeserializationError error = deserializeJson(configDoc, configFile);
        if (error) {
            Serial.println("Failed to parse config file");
            return;
        }

        configFile.close();
    }

    void saveConfig() {
        File configFile = SPIFFS.open(configFilePath, "w");
        if (!configFile) {
            Serial.println("Failed to open config file for writing");
            return;
        }

        serializeJson(configDoc, configFile);
        configFile.close();
    }

    // Métodos para obtener y actualizar valores de Wifi
    bool getWifiActive() {
        return configDoc["Wifi"]["active"];
    }

    void setWifiActive(bool active) {
        configDoc["Wifi"]["active"] = active;
    }

    String getWifiSSID() {
        return configDoc["Wifi"]["ssid"].as<String>();
    }

    void setWifiSSID(String ssid) {
        configDoc["Wifi"]["ssid"] = ssid;
    }

    String getWifiPassword() {
        return configDoc["Wifi"]["password"].as<String>();
    }

    void setWifiPassword(String password) {
        configDoc["Wifi"]["password"] = password;
    }

    String getWifiIP() {
        return configDoc["Wifi"]["ip"].as<String>();
    }

    void setWifiIP(String ip) {
        configDoc["Wifi"]["ip"] = ip;
    }

    String getWifiSubnet() {
        return configDoc["Wifi"]["subnet"].as<String>();
    }

    void setWifiSubnet(String subnet) {
        configDoc["Wifi"]["subnet"] = subnet;
    }

    String getWifiGateway() {
        return configDoc["Wifi"]["Gateway"].as<String>();
    }

    void setWifiGateway(String gateway) {
        configDoc["Wifi"]["Gateway"] = gateway;
    }

    // Métodos para obtener y actualizar valores de K1
    bool getK1Active() {
        return configDoc["K1"]["active"];
    }

    void setK1Active(bool active) {
        configDoc["K1"]["active"] = active;
    }

    String getK1Mode() {
        return configDoc["K1"]["mode"].as<String>();
    }

    void setK1Mode(String mode) {
        configDoc["K1"]["mode"] = mode;
    }

    String getK1Name() {
        return configDoc["K1"]["name"].as<String>();
    }

    void setK1Name(String name) {
        configDoc["K1"]["name"] = name;
    }

    // Métodos similares para otros campos según sea necesario

};

#endif
