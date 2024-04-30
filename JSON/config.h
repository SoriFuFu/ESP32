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

  StaticJsonDocument<2048> &getConfigDoc()
  {
    return configDoc;
  }

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
    saveConfig();
  }

  bool getWifiStatus()
  {
    return configDoc["Wifi"]["status"];
  }

  void setWifiStatus(bool status)
  {
    configDoc["Wifi"]["status"] = status;
    saveConfig();
  }

  String getWifiSSID()
  {
    return configDoc["Wifi"]["ssid"].as<String>();
  }

  void setWifiSSID(String ssid)
  {
    configDoc["Wifi"]["ssid"] = ssid;
    saveConfig();
  }

  String getWifiPassword()
  {
    return configDoc["Wifi"]["password"].as<String>();
  }

  void setWifiPassword(String password)
  {
    configDoc["Wifi"]["password"] = password;
    saveConfig();
  }

  bool getWifiStaticIp()
  {
    return configDoc["Wifi"]["staticIp"];
  }

  void setWifiStaticIp(bool staticIp)
  {
    if (staticIp)
    {
      configDoc["Wifi"]["staticIp"] = true;
    }
    else
    {
      configDoc["Wifi"]["staticIp"] = false;
      String StaticIPPrueva = configDoc["Wifi"]["staticIp"];
      Serial.print("IP estática configurada en: ");
      Serial.println(StaticIPPrueva);
    }
    saveConfig();
  }

  IPAddress getWifiIP()
  {
    String ipString = configDoc["Wifi"]["ip"].as<String>();
    IPAddress ip;
    ip.fromString(ipString);
    return ip;
  }

  IPAddress getWifiSubnet()
  {
    String subnetString = configDoc["Wifi"]["subnet"].as<String>();
    IPAddress subnet;
    subnet.fromString(subnetString);
    return subnet;
  }

  IPAddress getWifiGateway()
  {
    String gatewayString = configDoc["Wifi"]["gateway"].as<String>();
    IPAddress gateway;
    gateway.fromString(gatewayString);
    Serial.print("Gateway: ");
    Serial.println(gateway);
    return gateway;
  }

  void setWifiIP(String ip)
  {
    configDoc["Wifi"]["ip"] = ip;
    saveConfig();
  }

  void setWifiSubnet(String subnet)
  {
    configDoc["Wifi"]["subnet"] = subnet;
    saveConfig();
  }

  void setWifiGateway(String gateway)
  {
    configDoc["Wifi"]["gateway"] = gateway;
    saveConfig();
  }

  bool getApActive()
  {
    return configDoc["AP"]["active"];
  }
  void setApActive(bool active)
  {
    if (active)
    {
      configDoc["AP"]["active"] = true;
    }
    else
    {
      configDoc["AP"]["active"] = false;
    }
    saveConfig();
  }

  bool getApStatus()
  {
    return configDoc["AP"]["status"];
  }
  void setApStatus(bool status)
  {
    configDoc["AP"]["status"] = status;
    saveConfig();
  }

  // Métodos para obtener y actualizar valores AP
  String getApSSID()
  {
    return configDoc["AP"]["ssid"].as<String>();
  }
  void setApSSID(String ssid)
  {
    configDoc["AP"]["ssid"] = ssid;
    saveConfig();
  }

  String getApPassword()
  {
    return configDoc["AP"]["password"].as<String>();
  }
  void setApPassword(String password)
  {
    configDoc["AP"]["password"] = password;
    saveConfig();
  }

  // Métodos para obtener y actualizar valores de K1
  bool getK1Active()
  {
    return configDoc["Relay"]["K1"]["active"];
  }

  void setK1Active(bool active)
  {
    if (active)
    {
      configDoc["Relay"]["K1"]["active"] = true;
      saveConfig();
      Serial.println("Relé K1 activado");
    }
    else
    {
      configDoc["Relay"]["K1"]["active"] = false;
      saveConfig();
      Serial.println("Relé K1 desactivado");
    }
  }

  String getK1Mode()
  {
    return configDoc["Relay"]["K1"]["mode"].as<String>();
  }

  void setK1Mode(String mode)
  {
    configDoc["Relay"]["K1"]["mode"] = mode;
    saveConfig();
  }

  String getK1Name()
  {
    return configDoc["Relay"]["K1"]["name"].as<String>();
  }

  void setK1Name(String name)
  {
    configDoc["Relay"]["K1"]["name"] = name;
    saveConfig();
    Serial.print("Nombre del relé K1 actualizado a ");
    Serial.println(name);
  }

  bool getK1State()
  {
    return configDoc["Relay"]["K1"]["state"];
  }
  void setK1State(bool state)
  {
    configDoc["Relay"]["K1"]["state"] = state;
    saveConfig();
  }
  unsigned long getK1Timer()
  {
    return configDoc["Relay"]["K1"]["timer"];
  }
  void setK1Timer(int timer)
  {
    configDoc["Relay"]["K1"]["timer"] = timer;
    saveConfig();
  }

  unsigned long getK1ActivationTime()
  {
    return configDoc["Relay"]["K1"]["activationTime"];
  }

  void setK1ActivationTime(unsigned long activationTime)
  {
    configDoc["Relay"]["K1"]["activationTime"] = activationTime;
    saveConfig();
  }

  unsigned long getK1DeactivationTime()
  {
    return configDoc["Relay"]["K1"]["deactivationTime"];
  }

  void setK1DeactivationTime(unsigned long deactivationTime)
  {
    configDoc["Relay"]["K1"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }

  // Métodos para obtener y actualizar valores de K2
  bool getK2Active()
  {
    return configDoc["Relay"]["K2"]["active"];
  }

  void setK2Active(bool active)
  {
    if (active)
    {
      configDoc["Relay"]["K2"]["active"] = true;
      saveConfig();
      Serial.println("Relé K2 activado");
    }
    else
    {
      configDoc["Relay"]["K2"]["active"] = false;
      saveConfig();
      Serial.println("Relé K2 desactivado");
    }
  }

  String getK2Mode()
  {
    return configDoc["Relay"]["K2"]["mode"].as<String>();
  }

  void setK2Mode(String mode)
  {
    configDoc["Relay"]["K2"]["mode"] = mode;
    saveConfig();
  }

  String getK2Name()
  {
    return configDoc["Relay"]["K2"]["name"].as<String>();
  }

  void setK2Name(String name)
  {
    configDoc["Relay"]["K2"]["name"] = name;
    saveConfig();
    Serial.print("Nombre del relé K2 actualizado a ");
    Serial.println(name);
  }

  bool getK2State()
  {
    return configDoc["Relay"]["K2"]["state"];
  }
  void setK2State(bool state)
  {
    configDoc["Relay"]["K2"]["state"] = state;
    saveConfig();
  }
  unsigned long getK2Timer()
  {
    return configDoc["Relay"]["K2"]["timer"];
  }
  void setK2Timer(int timer)
  {
    configDoc["Relay"]["K2"]["timer"] = timer;
    saveConfig();
  }

  unsigned long getK2ActivationTime()
  {
    return configDoc["Relay"]["K2"]["activationTime"];
  }

  void setK2ActivationTime(unsigned long activationTime)
  {
    configDoc["Relay"]["K2"]["activationTime"] = activationTime;
    saveConfig();
  }

  unsigned long getK2DeactivationTime()
  {
    return configDoc["Relay"]["K2"]["deactivationTime"];
  }

  void setK2DeactivationTime(unsigned long deactivationTime)
  {
    configDoc["Relay"]["K2"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }

  // Métodos para obtener y actualizar valores de K3
  bool getK3Active()
  {
    return configDoc["Relay"]["K3"]["active"];
  }

  void setK3Active(bool active)
  {
    if (active)
    {
      configDoc["Relay"]["K3"]["active"] = true;
      saveConfig();
      Serial.println("Relé K3 activado");
    }
    else
    {
      configDoc["Relay"]["K3"]["active"] = false;
      saveConfig();
      Serial.println("Relé K3 desactivado");
    }
  }

  String getK3Mode()
  {
    return configDoc["Relay"]["K3"]["mode"].as<String>();
  }

  void setK3Mode(String mode)
  {
    configDoc["Relay"]["K3"]["mode"] = mode;
    saveConfig();
  }

  String getK3Name()
  {
    return configDoc["Relay"]["K3"]["name"].as<String>();
  }

  void setK3Name(String name)
  {
    configDoc["Relay"]["K3"]["name"] = name;
    saveConfig();
    Serial.println("Nombre del relé K3 actualizado a ");
    Serial.println(name);
  }

  bool getK3State()
  {
    return configDoc["Relay"]["K3"]["state"];
  }
  void setK3State(bool state)
  {
    configDoc["Relay"]["K3"]["state"] = state;
    saveConfig();
  }
  unsigned long getK3Timer()
  {
    return configDoc["Relay"]["K3"]["timer"];
  }
  void setK3Timer(int timer)
  {
    configDoc["Relay"]["K3"]["timer"] = timer;
    saveConfig();
  }

  unsigned long getK3ActivationTime()
  {
    return configDoc["Relay"]["K3"]["activationTime"];
  }

  void setK3ActivationTime(unsigned long activationTime)
  {
    configDoc["Relay"]["K3"]["activationTime"] = activationTime;
    saveConfig();
  }

  unsigned long getK3DeactivationTime()
  {
    return configDoc["Relay"]["K3"]["deactivationTime"];
  }

  void setK3DeactivationTime(unsigned long deactivationTime)
  {
    configDoc["Relay"]["K3"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }

  // Métodos para obtener y actualizar valores de K4
  bool getK4Active()
  {
    return configDoc["K4"]["active"];
  }

  void setK4Active(bool active)
  {
    if (active)
    {
      configDoc["Relay"]["K4"]["active"] = true;
      saveConfig();
      Serial.println("Relé K4 activado");
    }
    else
    {
      configDoc["Relay"]["K4"]["active"] = false;
      saveConfig();
      Serial.println("Relé K4 desactivado");
    }

    
  }

  String getK4Mode()
  {
    return configDoc["Relay"]["K4"]["mode"].as<String>();
  }

  void setK4Mode(String mode)
  {
    configDoc["Relay"]["K4"]["mode"] = mode;
    saveConfig();
  }

  String getK4Name()
  {
    return configDoc["Relay"]["K4"]["name"].as<String>();
  }

  void setK4Name(String name)
  {
    configDoc["Relay"]["K4"]["name"] = name;
    saveConfig();
    Serial.println("Nombre del relé K4 actualizado a ");
    Serial.println(name);
  }

  bool getK4State()
  {
    return configDoc["Relay"]["K4"]["state"];
  }
  void setK4State(bool state)
  {
    configDoc["Relay"]["K4"]["state"] = state;
    saveConfig();
  }
  unsigned long getK4Timer()
  {
    return configDoc["Relay"]["K4"]["timer"];
  }
  void setK4Timer(int timer)
  {
    configDoc["Relay"]["K4"]["timer"] = timer;
    saveConfig();
  }

  unsigned long getK4ActivationTime()
  {
    return configDoc["Relay"]["K4"]["activationTime"];
  }

  void setK4ActivationTime(unsigned long activationTime)
  {
    configDoc["Relay"]["K4"]["activationTime"] = activationTime;
    saveConfig();
  }

  unsigned long getK4DeactivationTime()
  {
    return configDoc["Relay"]["K4"]["deactivationTime"];
  }

  void setK4DeactivationTime(unsigned long deactivationTime)
  {
    configDoc["Relay"]["K4"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }
};

#endif
