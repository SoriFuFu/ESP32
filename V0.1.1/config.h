#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>
#include <SPIFFS.h>

class ConfigManager
{
private:
  StaticJsonDocument<1024> configDoc; 
  const String configFilePath = "/config.json"; // Ruta estática del archivo JSON

public:
  ConfigManager() {}

  StaticJsonDocument<1024> &getConfigDoc() // MÉTODO PARA OBTENER EL DOCUMENTO JSON
  {
    return configDoc;
  }

  void initSPIFFS() // MÉTODO PARA INICIAR EL SISTEMA DE ARCHIVOS SPIFFS
  {
    if (!SPIFFS.begin())
    {
      Serial.println("Failed to mount file system");
      return;
    }
  }

  void loadConfig() // MÉTODO PARA CARGAR LA CONFIGURACIÓN DESDE EL ARCHIVO JSON
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

  void saveConfig() // MÉTODO PARA GUARDAR LA CONFIGURACIÓN EN EL ARCHIVO JSON
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

  //*****GETTERS *****//
  bool getWifiActive() // MÉTODO PARA OBTENER EL ESTADO DEL WIFI
  {
    return configDoc["Wifi"]["active"];
  }

  bool getWifiStatus() // MÉTODO PARA OBTENER EL ESTADO DEL WIFI
  {
    return configDoc["Wifi"]["status"];
  }

  String getWifiSSID() // MÉTODO PARA OBTENER EL NOMBRE DE LA RED WIFI
  {
    return configDoc["Wifi"]["ssid"].as<String>();
  }

  String getWifiPassword() // MÉTODO PARA OBTENER LA CONTRASEÑA DE LA RED WIFI
  {
    return configDoc["Wifi"]["password"].as<String>();
  }

  bool getWifiStaticIp() // MÉTODO PARA OBTENER EL ESTADO DE LA IP ESTÁTICA
  {
    return configDoc["Wifi"]["staticIp"];
  }

  IPAddress getWifiIP() // MÉTODO PARA OBTENER LA DIRECCIÓN IP DEL DISPOSITIVO
  {
    String ipString = configDoc["Wifi"]["ip"].as<String>();
    IPAddress ip;
    ip.fromString(ipString);
    return ip;
  }

  IPAddress getWifiSubnet() // MÉTODO PARA OBTENER LA MÁSCARA DE SUBRED
  {
    String subnetString = configDoc["Wifi"]["subnet"].as<String>();
    IPAddress subnet;
    subnet.fromString(subnetString);
    return subnet;
  }

  IPAddress getWifiGateway() // MÉTODO PARA OBTENER LA PUERTA DE ENLACE
  {
    String gatewayString = configDoc["Wifi"]["gateway"].as<String>();
    IPAddress gateway;
    gateway.fromString(gatewayString);
    return gateway;
  }

  bool getApStatus() // MÉTODO PARA OBTENER EL ESTADO DEL PUNTO DE ACCESO
  {
    return configDoc["AP"]["status"];
  }

  bool getApActive() // MÉTODO PARA OBTENER EL ESTADO DEL PUNTO DE ACCESO
  {
    return configDoc["AP"]["active"];
  }

  String getApSSID() // MÉTODO PARA OBTENER EL NOMBRE DEL PUNTO DE ACCESO
  {
    return configDoc["AP"]["ssid"].as<String>();
  }

  String getApPassword() // MÉTODO PARA OBTENER LA CONTRASEÑA DEL PUNTO DE ACCESO
  {
    return configDoc["AP"]["password"].as<String>();
  }

  // MÉTODO DE K1
  bool getRelayActive(String relay) // MÉTODO PARA OBTENER EL ESTADO DEL RELÉ K1
  {
    return configDoc["Relay"][relay]["active"];
  }

  bool getRelayTimerState(String relay) // MÉTODO PARA OBTENER EL ESTADO DEL TEMPORIZADOR DEL RELÉ K1
  {
    return configDoc["Relay"][relay]["timerState"];
  }

  String getRelayName(String relay) // MÉTODO PARA OBTENER EL NOMBRE DEL RELÉ K1
  {
    return configDoc["Relay"][relay]["name"].as<String>();
  }

  unsigned long getRelayRemainingTime(String relay) // MÉTODO PARA OBTENER EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K1
  {
    return configDoc["Relay"][relay]["remainingTime"];
  }

  bool getRelayState(String relay) // MÉTODO PARA OBTENER EL ESTADO DEL RELÉ K1
  {
    return configDoc["Relay"][relay]["state"];
  }

  unsigned long getRelayTimerSelected(String relay) // MÉTODO PARA OBTENER EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K1
  {
    return configDoc["Relay"][relay]["timerSelected"];
  }



  //*****SETTERS *****//
  void setApSSID(String ssid) // MÉTODO PARA CAMBIAR EL NOMBRE DEL PUNTO DE ACCESO
  {
    configDoc["AP"]["ssid"] = ssid;
    saveConfig();
  }

  void setWifiActive(bool active) // MÉTODO PARA CAMBIAR EL ESTADO DEL WIFI
  {
    configDoc["Wifi"]["active"] = active;
    saveConfig();
  }

  void setWifiStatus(bool status) // MÉTODO PARA CAMBIAR EL ESTADO DEL WIFI
  {
    configDoc["Wifi"]["status"] = status;
    saveConfig();
  }

  void setWifiSSID(String ssid) // MÉTODO PARA CAMBIAR EL NOMBRE DE LA RED WIFI
  {
    configDoc["Wifi"]["ssid"] = ssid;
    saveConfig();
  }

  void setWifiPassword(String password) // MÉTODO PARA CAMBIAR LA CONTRASEÑA DE LA RED WIFI
  {
    configDoc["Wifi"]["password"] = password;
    saveConfig();
  }

  void setWifiStaticIp(bool staticIp) // MÉTODO PARA CAMBIAR EL ESTADO DE LA IP ESTÁTICA
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

  void setWifiIP(String ip) // MÉTODO PARA CAMBIAR LA DIRECCIÓN IP DEL DISPOSITIVO
  {
    configDoc["Wifi"]["ip"] = ip;
    saveConfig();
  }

  void setWifiSubnet(String subnet) // MÉTODO PARA CAMBIAR LA MÁSCARA DE SUBRED
  {
    configDoc["Wifi"]["subnet"] = subnet;
    saveConfig();
  }

  void setWifiGateway(String gateway) // MÉTODO PARA CAMBIAR LA PUERTA DE ENLACE
  {
    configDoc["Wifi"]["gateway"] = gateway;
    saveConfig();
  }

  void setApActive(bool active) // MÉTODO PARA CAMBIAR EL ESTADO DEL PUNTO DE ACCESO
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

  void setApStatus(bool status) // MÉTODO PARA CAMBIAR EL ESTADO DEL PUNTO DE ACCESO
  {
    configDoc["AP"]["status"] = status;
    saveConfig();
  }

  void setApPassword(String password) // MÉTODO PARA CAMBIAR LA CONTRASEÑA DEL PUNTO DE ACCESO
  {
    configDoc["AP"]["password"] = password;
    saveConfig();
  }

  void setMacAddress(String macAddress) // MÉTODO PARA CAMBIAR LA DIRECCIÓN MAC DEL DISPOSITIVO
  {
    configDoc["Wifi"]["macAddress"] = macAddress;
    saveConfig();
  }


// MÉTODOS DE LOS RELÉS  
  void setRelayActive(String relay, bool active) // MÉTODO PARA CAMBIAR EL ESTADO DEL RELÉ
  {

    configDoc["Relay"][relay]["active"] = active;
    saveConfig();
  }

  void setRelayTimerState(String relay, bool timerState) // MÉTODO PARA CAMBIAR EL ESTADO DEL TEMPORIZADOR DEL RELÉ
  {
    configDoc["Relay"][relay]["timerState"] = timerState;
    saveConfig();
  }

  void setRelayName(String relay, String name) // MÉTODO PARA CAMBIAR EL NOMBRE DEL RELÉ
  {
    configDoc["Relay"][relay]["name"] = name;
    saveConfig();
  }

  void setRelayRemainingTime(String relay, int remainingTime) // MÉTODO PARA CAMBIAR EL TIEMPO DEL TEMPORIZADOR DEL RELÉ
  {
    configDoc["Relay"][relay]["remainingTime"] = remainingTime;
  }

  void setRelayState(String relay, String state) // MÉTODO PARA CAMBIAR EL ESTADO DEL RELÉ
  {
      configDoc["Relay"][relay]["state"] = state;
  }

  void setAllRelayState(String state) // MÉTODO PARA CAMBIAR EL ESTADO DE TODOS LOS RELÉS
  {
    configDoc["Relay"]["K1"]["state"] = state;
    configDoc["Relay"]["K2"]["state"] = state;
    configDoc["Relay"]["K3"]["state"] = state;
    configDoc["Relay"]["K4"]["state"] = state;
    saveConfig();
  }

  void setRelayTimerSelected(String relay, unsigned long timerSelected) // MÉTODO PARA CAMBIAR EL TIEMPO DEL TEMPORIZADOR DEL RELÉ
  {
    configDoc["Relay"][relay]["timerSelected"] = timerSelected;
    saveConfig();
  }
  
};

#endif