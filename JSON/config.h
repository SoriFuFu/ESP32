#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>
#include <SPIFFS.h>

class ConfigManager {
private:
  StaticJsonDocument<2048> configDoc;
  const String configFilePath = "/config.json";  // Ruta estática del archivo JSON

public:
  ConfigManager() {}

  StaticJsonDocument<2048> &getConfigDoc() {
    return configDoc;
  }

  void initSPIFFS() {
    if (!SPIFFS.begin()) {
      Serial.println("Failed to mount file system");
      return;
    }
  }

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
    saveConfig();
  }

  bool getWifiStatus() {
    return configDoc["Wifi"]["status"];
  }

  void setWifiStatus(bool status) {
    configDoc["Wifi"]["status"] = status;
    saveConfig();
  }

  String getWifiSSID() {
    return configDoc["Wifi"]["ssid"].as<String>();
  }

  void setWifiSSID(String ssid) {
    configDoc["Wifi"]["ssid"] = ssid;
    saveConfig();
  }

  String getWifiPassword() {
    return configDoc["Wifi"]["password"].as<String>();
  }

  void setWifiPassword(String password) {
    configDoc["Wifi"]["password"] = password;
    saveConfig();
  }

  bool getWifiStaticIp() {
    return configDoc["Wifi"]["staticIp"];
  }

  void setWifiStaticIp(bool staticIp) {
    if (staticIp) {
      configDoc["Wifi"]["staticIp"] = true;
    } else {
      configDoc["Wifi"]["staticIp"] = false;
      String StaticIPPrueva = configDoc["Wifi"]["staticIp"];
    Serial.print("IP estática configurada en: ");
    Serial.println(StaticIPPrueva);
    }
    saveConfig();

  }

  IPAddress getWifiIP() {
    String ipString = configDoc["Wifi"]["ip"].as<String>();
    IPAddress ip;
    ip.fromString(ipString);
    return ip;
  }

  IPAddress getWifiSubnet() {
    String subnetString = configDoc["Wifi"]["subnet"].as<String>();
    IPAddress subnet;
    subnet.fromString(subnetString);
    return subnet;
  }

  IPAddress getWifiGateway() {
    String gatewayString = configDoc["Wifi"]["Gateway"].as<String>();
    IPAddress gateway;
    gateway.fromString(gatewayString);
    return gateway;
  }


  void setWifiIP(String ip) {
    configDoc["Wifi"]["ip"] = ip;
    saveConfig();
  }

  void setWifiSubnet(String subnet) {
    configDoc["Wifi"]["subnet"] = subnet;
    saveConfig();
  }

  void setWifiGateway(String gateway) {
    configDoc["Wifi"]["gateway"] = gateway;
    saveConfig();
  }

  bool getApActive() {
    return configDoc["AP"]["active"];
  }
  void setApActive(bool active) {
   if (active) {
      configDoc["AP"]["active"] = true;
    } else {
      configDoc["AP"]["active"] = false;
    }
    saveConfig();
  }

  bool getApStatus() {
    return configDoc["AP"]["status"];
  }
  void setApStatus(bool status) {
    configDoc["AP"]["status"] = status;
    saveConfig();
  }

  // Métodos para obtener y actualizar valores AP
  String getApSSID() {
    return configDoc["AP"]["ssid"].as<String>();
  }
  void setApSSID(String ssid) {
    configDoc["AP"]["ssid"] = ssid;
    saveConfig();
  }

  String getApPassword() {
    return configDoc["AP"]["password"].as<String>();
  }
  void setApPassword(String password) {
    configDoc["AP"]["password"] = password;
    saveConfig();
  }

  // Métodos para obtener y actualizar valores de K1
  bool getK1Active() {
    return configDoc["K1"]["active"];
  }

  void setK1Active(bool active) {
    configDoc["K1"]["active"] = active;
    saveConfig();
  }

  String getK1Mode() {
    return configDoc["K1"]["mode"].as<String>();
  }

  void setK1Mode(String mode) {
    configDoc["K1"]["mode"] = mode;
    saveConfig();
  }

  String getK1Name() {
    return configDoc["K1"]["name"].as<String>();
  }

  void setK1Name(String name) {
    configDoc["K1"]["name"] = name;
    saveConfig();
  }

  bool getK1State() {
    return configDoc["K1"]["state"];
  }
  void setK1State(bool state) {
    configDoc["K1"]["state"] = state;
    saveConfig();
  }
  unsigned long getK1Timer() {
    return configDoc["K1"]["timer"];
  }
  void setK1Timer(int timer) {
    configDoc["K1"]["timer"] = timer;
    saveConfig();
  }

  unsigned long getK1ActivationTime() {
    return configDoc["K1"]["activationTime"];
  }

  void setK1ActivationTime(unsigned long activationTime) {
    configDoc["K1"]["activationTime"] = activationTime;
    saveConfig();
  }

  unsigned long getK1DeactivationTime() {
    return configDoc["K1"]["deactivationTime"];
  }

  void setK1DeactivationTime(unsigned long deactivationTime) {
    configDoc["K1"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }

    // Métodos para obtener y actualizar valores de K2
  bool getK2Active() {
    return configDoc["K2"]["active"];
  }

  void setK2Active(bool active) {
    configDoc["K2"]["active"] = active;
    saveConfig();
  }

  String getK2Mode() {
    return configDoc["K2"]["mode"].as<String>();
  }

  void setK2Mode(String mode) {
    configDoc["K2"]["mode"] = mode;
    saveConfig();
  }

  String getK2Name() {
    return configDoc["K2"]["name"].as<String>();
  }

  void setK2Name(String name) {
    configDoc["K2"]["name"] = name;
    saveConfig();
  }

  bool getK2State() {
    return configDoc["K2"]["state"];
  }
  void setK2State(bool state) {
    configDoc["K2"]["state"] = state;
    saveConfig();
  }
  unsigned long getK2Timer() {
    return configDoc["K2"]["timer"];
  }
  void setK2Timer(int timer) {
    configDoc["K2"]["timer"] = timer;
    saveConfig();
  }

  unsigned long getK2ActivationTime() {
    return configDoc["K2"]["activationTime"];
  }

  void setK2ActivationTime(unsigned long activationTime) {
    configDoc["K2"]["activationTime"] = activationTime;
    saveConfig();
  }

  unsigned long getK2DeactivationTime() {
    return configDoc["K2"]["deactivationTime"];
  }

  void setK2DeactivationTime(unsigned long deactivationTime) {
    configDoc["K2"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }

    // Métodos para obtener y actualizar valores de K3
  bool getK3Active() {
    return configDoc["K3"]["active"];
  }

  void setK3Active(bool active) {
    configDoc["K3"]["active"] = active;
    saveConfig();
  }

  String getK3Mode() {
    return configDoc["K3"]["mode"].as<String>();
  }

  void setK3Mode(String mode) {
    configDoc["K3"]["mode"] = mode;
    saveConfig();
  }

  String getK3Name() {
    return configDoc["K3"]["name"].as<String>();
  }

  void setK3Name(String name) {
    configDoc["K3"]["name"] = name;
    saveConfig();
  }

  bool getK3State() {
    return configDoc["K3"]["state"];
  }
  void setK3State(bool state) {
    configDoc["K3"]["state"] = state;
    saveConfig();
  }
  unsigned long getK3Timer() {
    return configDoc["K3"]["timer"];
  }
  void setK3Timer(int timer) {
    configDoc["K3"]["timer"] = timer;
    saveConfig();
  }

  unsigned long getK3ActivationTime() {
    return configDoc["K3"]["activationTime"];
  }

  void setK3ActivationTime(unsigned long activationTime) {
    configDoc["K3"]["activationTime"] = activationTime;
    saveConfig();
  }

  unsigned long getK3DeactivationTime() {
    return configDoc["K3"]["deactivationTime"];
  }

  void setK3DeactivationTime(unsigned long deactivationTime) {
    configDoc["K3"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }
  
    // Métodos para obtener y actualizar valores de K4
  bool getK4Active() {
    return configDoc["K4"]["active"];
  }

  void setK4Active(bool active) {
    configDoc["K4"]["active"] = active;
    saveConfig();
  }

  String getK4Mode() {
    return configDoc["K4"]["mode"].as<String>();
  }

  void setK4Mode(String mode) {
    configDoc["K4"]["mode"] = mode;
    saveConfig();
  }

  String getK4Name() {
    return configDoc["K4"]["name"].as<String>();
  }

  void setK4Name(String name) {
    configDoc["K4"]["name"] = name;
    saveConfig();
  }

  bool getK4State() {
    return configDoc["K4"]["state"];
  }
  void setK4State(bool state) {
    configDoc["K4"]["state"] = state;
    saveConfig();
  }
  unsigned long getK4Timer() {
    return configDoc["K4"]["timer"];
  }
  void setK4Timer(int timer) {
    configDoc["K4"]["timer"] = timer;
    saveConfig();
  }

  unsigned long getK4ActivationTime() {
    return configDoc["K4"]["activationTime"];
  }

  void setK4ActivationTime(unsigned long activationTime) {
    configDoc["K4"]["activationTime"] = activationTime;
    saveConfig();
  }

  unsigned long getK4DeactivationTime() {
    return configDoc["K4"]["deactivationTime"];
  }

  void setK4DeactivationTime(unsigned long deactivationTime) {
    configDoc["K4"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }
};

#endif
