#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>
#include <SPIFFS.h>

class ConfigManager
{
private:
    StaticJsonDocument<2048> configDoc;
    const String configFilePath = "/config.json"; // Ruta estática del archivo JSON

public:
    ConfigManager() {}

     void initSPIFFS()
    {
        if (!SPIFFS.begin())
        {
            Serial.println("Failed to mount file system");
            return;
        }
    }

    void loadConfig()
    {
        File configFile = SPIFFS.open(configFilePath, "r");
        if (!configFile)
        {
            Serial.println("Failed to open config file");
            return;
        }

        DeserializationError error = deserializeJson(configDoc, configFile);
        if (error)
        {
            Serial.println("Failed to parse config file");
            return;
        }

        configFile.close();
    }

    void saveConfig()
    {
        File configFile = SPIFFS.open(configFilePath, "w");
        if (!configFile)
        {
            Serial.println("Failed to open config file for writing");
            return;
        }

        serializeJson(configDoc, configFile);
        configFile.close();
    }

    // Métodos para obtener y actualizar valores de Wifi
    bool getWifiActive()
    {
        return configDoc["Wifi"]["active"];
    }

    void setWifiActive(bool active)
    {
        configDoc["Wifi"]["active"] = active;
    }

    String getWifiSSID()
    {
        return configDoc["Wifi"]["ssid"].as<String>();
    }

    void setWifiSSID(String ssid)
    {
        configDoc["Wifi"]["ssid"] = ssid;
    }

    String getWifiPassword()
    {
        return configDoc["Wifi"]["password"].as<String>();
    }

    void setWifiPassword(String password)
    {
        configDoc["Wifi"]["password"] = password;
    }

    String getWifiIP()
    {
        return configDoc["Wifi"]["ip"].as<String>();
    }

    void setWifiIP(String ip)
    {
        configDoc["Wifi"]["ip"] = ip;
    }

    String getWifiSubnet()
    {
        return configDoc["Wifi"]["subnet"].as<String>();
    }

    void setWifiSubnet(String subnet)
    {
        configDoc["Wifi"]["subnet"] = subnet;
    }

    String getWifiGateway()
    {
        return configDoc["Wifi"]["Gateway"].as<String>();
    }

    void setWifiGateway(String gateway)
    {
        configDoc["Wifi"]["Gateway"] = gateway;
    }

    // Métodos para obtener y actualizar valores AP
    String getApSSID()
    {
        return configDoc["AP"]["ssid"].as<String>();
    }
    void setApSSID(String ssid)
    {
        configDoc["AP"]["ssid"] = ssid;
    }

    String getApPassword()
    {
        return configDoc["AP"]["password"].as<String>();
    }
    void setApPassword(String password)
    {
        configDoc["AP"]["password"] = password;
    }

    // Métodos para obtener y actualizar valores de K1
    bool getK1Active()
    {
        return configDoc["K1"]["active"];
    }

    void setK1Active(bool active)
    {
        configDoc["K1"]["active"] = active;
    }

    String getK1Mode()
    {
        return configDoc["K1"]["mode"].as<String>();
    }

    void setK1Mode(String mode)
    {
        configDoc["K1"]["mode"] = mode;
    }

    String getK1Name()
    {
        return configDoc["K1"]["name"].as<String>();
    }

    void setK1Name(String name)
    {
        configDoc["K1"]["name"] = name;
    }

    bool getK1State()
    {
        return configDoc["K1"]["state"];
    }
    void setK1State(bool state)
    {
        configDoc["K1"]["state"] = state;
    }
    unsigned long getK1Timer()
    {
        return configDoc["K1"]["timer"];
    }
    void setK1Timer(int timer)
    {
        configDoc["K1"]["timer"] = timer;
    }

    unsigned long getK1ActivationTime()
    {
        return configDoc["K1"]["activationTime"];
    }

    void setK1ActivationTime(unsigned long activationTime)
    {
        configDoc["K1"]["activationTime"] = activationTime;
    }

    unsigned long getK1DeactivationTime()
    {
        return configDoc["K1"]["deactivationTime"];
    }

    void setK1DeactivationTime(unsigned long deactivationTime)
    {
        configDoc["K1"]["deactivationTime"] = deactivationTime;
    }
};

#endif
