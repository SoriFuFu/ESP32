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

  StaticJsonDocument<2048> &getConfigDoc() // MÉTODO PARA OBTENER EL DOCUMENTO JSON
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

  //MÉTODO DE K1
  bool getK1Active() // MÉTODO PARA OBTENER EL ESTADO DEL RELÉ K1
  {
    return configDoc["Relay"]["K1"]["active"];
  }

  String getK1Mode() // MÉTODO PARA OBTENER EL MODO DEL RELÉ K1
  {
    return configDoc["Relay"]["K1"]["mode"].as<String>();
  }
  
  String getK1Name() // MÉTODO PARA OBTENER EL NOMBRE DEL RELÉ K1
  {
    return configDoc["Relay"]["K1"]["name"].as<String>();
  }

  bool getK1State() // MÉTODO PARA OBTENER EL ESTADO DEL RELÉ K1
  {
    return configDoc["Relay"]["K1"]["state"];
  }
  
  unsigned long getK1ActivationTime() // MÉTODO PARA OBTENER EL TIEMPO DE ACTIVACIÓN DEL RELÉ K1
  {
    return configDoc["Relay"]["K1"]["activationTime"];
  }
  
  unsigned long getK1TimerSelected() // MÉTODO PARA OBTENER EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K1
  {
    return configDoc["Relay"]["K1"]["timerSelected"];
  }
  
  unsigned long getK1Timer() // MÉTODO PARA OBTENER EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K1
  {
    return configDoc["Relay"]["K1"]["timer"];
  }
  
  unsigned long getK1DeactivationTime() // MÉTODO PARA OBTENER EL TIEMPO DE DESACTIVACIÓN DEL RELÉ K1
  {
    return configDoc["Relay"]["K1"]["deactivationTime"];
  }

  //MÉTODO DE K2
  bool getK2Active() // MÉTODO PARA OBTENER EL ESTADO DEL RELÉ K2
  {
    return configDoc["Relay"]["K2"]["active"];
  }

  String getK2Mode() // MÉTODO PARA OBTENER EL MODO DEL RELÉ K2
  {
    return configDoc["Relay"]["K2"]["mode"].as<String>();
  }

  String getK2Name() // MÉTODO PARA OBTENER EL NOMBRE DEL RELÉ K2
  {
    return configDoc["Relay"]["K2"]["name"].as<String>();
  }

  bool getK2State() // MÉTODO PARA OBTENER EL ESTADO DEL RELÉ K2
  {
    return configDoc["Relay"]["K2"]["state"];
  }
  
  unsigned long getK2TimerSelected() // MÉTODO PARA OBTENER EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K2
  {
    return configDoc["Relay"]["K2"]["timerSelected"];
  }
  
  unsigned long getK2Timer() // MÉTODO PARA OBTENER EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K2
  {
    return configDoc["Relay"]["K2"]["timer"];
  }

  unsigned long getK2ActivationTime() // MÉTODO PARA OBTENER EL TIEMPO DE ACTIVACIÓN DEL RELÉ K2
  {
    return configDoc["Relay"]["K2"]["activationTime"];
  }

  unsigned long getK2DeactivationTime() // MÉTODO PARA OBTENER EL TIEMPO DE DESACTIVACIÓN DEL RELÉ K2
  {
    return configDoc["Relay"]["K2"]["deactivationTime"];
  }

  //MÉTODO DE K3
  bool getK3Active() // MÉTODO PARA OBTENER EL ESTADO DEL RELÉ K3
  {
    return configDoc["Relay"]["K3"]["active"];
  }

  String getK3Mode() // MÉTODO PARA OBTENER EL MODO DEL RELÉ K3
  {
    return configDoc["Relay"]["K3"]["mode"].as<String>();
  }

  String getK3Name() // MÉTODO PARA OBTENER EL NOMBRE DEL RELÉ K3
  {
    return configDoc["Relay"]["K3"]["name"].as<String>();
  }

  bool getK3State() // MÉTODO PARA OBTENER EL ESTADO DEL RELÉ K3
  {
    return configDoc["Relay"]["K3"]["state"];
  }

  unsigned long getK3TimerSelected() // MÉTODO PARA OBTENER EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K3
  {
    return configDoc["Relay"]["K3"]["timerSelected"];
  }
  
  unsigned long getK3Timer() // MÉTODO PARA OBTENER EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K3
  {
    return configDoc["Relay"]["K3"]["timer"];
  }

  unsigned long getK3ActivationTime() // MÉTODO PARA OBTENER EL TIEMPO DE ACTIVACIÓN DEL RELÉ K3
  {
    return configDoc["Relay"]["K3"]["activationTime"];
  }

  unsigned long getK3DeactivationTime() // MÉTODO PARA OBTENER EL TIEMPO DE DESACTIVACIÓN DEL RELÉ K3
  {
    return configDoc["Relay"]["K3"]["deactivationTime"];
  }


  //MÉTODO DE K4
  bool getK4Active() // MÉTODO PARA OBTENER EL ESTADO DEL RELÉ K4
  {
    return configDoc["Relay"]["K4"]["active"];
  }

  String getK4Mode() // MÉTODO PARA OBTENER EL MODO DEL RELÉ K4
  {
    return configDoc["Relay"]["K4"]["mode"].as<String>();
  }

  String getK4Name() // MÉTODO PARA OBTENER EL NOMBRE DEL RELÉ K4
  {
    return configDoc["Relay"]["K4"]["name"].as<String>();
  }

  bool getK4State() // MÉTODO PARA OBTENER EL ESTADO DEL RELÉ K4
  {
    return configDoc["Relay"]["K4"]["state"];
  }

  unsigned long getK4TimerSelected() // MÉTODO PARA OBTENER EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K4
  {
    return configDoc["Relay"]["K4"]["timerSelected"];
  }
  
  unsigned long getK4Timer() // MÉTODO PARA OBTENER EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K4
  {
    return configDoc["Relay"]["K4"]["timer"];
  }

  unsigned long getK4ActivationTime() // MÉTODO PARA OBTENER EL TIEMPO DE ACTIVACIÓN DEL RELÉ K4
  {
    return configDoc["Relay"]["K4"]["activationTime"];
  }

  unsigned long getK4DeactivationTime() // MÉTODO PARA OBTENER EL TIEMPO DE DESACTIVACIÓN DEL RELÉ K4
  {
    return configDoc["Relay"]["K4"]["deactivationTime"];
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

  //MÉTODOS DE K1
  void setK1Active(bool active) // MÉTODO PARA CAMBIAR EL ESTADO DEL RELÉ K1
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

  void setK1TimerStatus(bool timerStatus) // MÉTODO PARA CAMBIAR EL MODO DEL RELÉ K1
  {
    configDoc["Relay"]["K1"]["timerStatus"] = timerStatus;
    saveConfig();
  }

  void setK1Name(String name)  // MÉTODO PARA CAMBIAR EL NOMBRE DEL RELÉ K1
  {
    configDoc["Relay"]["K1"]["name"] = name;
    saveConfig();
    Serial.print("Nombre del relé K1 actualizado a ");
    Serial.println(name);
  }

  void setK1State(String state) // MÉTODO PARA CAMBIAR EL ESTADO DEL RELÉ K1
  {
    configDoc["Relay"]["K1"]["state"] = state;
    saveConfig();
  }

  void setK1TimerSelected(unsigned long timerSelected) // MÉTODO PARA CAMBIAR EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K1
  {
    configDoc["Relay"]["K1"]["timerSelected"] = timerSelected;
    saveConfig();
  }

  void setK1Timer(int timer) // MÉTODO PARA CAMBIAR EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K1
  {
    configDoc["Relay"]["K1"]["timer"] = timer;
    saveConfig();
  }

  void setK1ActivationTime(unsigned long activationTime) // MÉTODO PARA CAMBIAR EL TIEMPO DE ACTIVACIÓN DEL RELÉ K1
  {
    configDoc["Relay"]["K1"]["activationTime"] = activationTime;
    saveConfig();
  }

  void setK1DeactivationTime(unsigned long deactivationTime)  // MÉTODO PARA CAMBIAR EL TIEMPO DE DESACTIVACIÓN DEL RELÉ K1
  {
    configDoc["Relay"]["K1"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }

  //MÉTODOS DE K2
  void setK2Active(bool active) // MÉTODO PARA CAMBIAR EL ESTADO DEL RELÉ K2
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

  void setK2TimerStatus(bool timerStatus) // MÉTODO PARA CAMBIAR EL MODO DEL RELÉ K1
  {
    configDoc["Relay"]["K2"]["timerStatus"] = timerStatus;
    saveConfig();
  }

  void setK2Name(String name) // MÉTODO PARA CAMBIAR EL NOMBRE DEL RELÉ K2
  {
    configDoc["Relay"]["K2"]["name"] = name;
    saveConfig();
    Serial.print("Nombre del relé K2 actualizado a ");
    Serial.println(name);
  }

  void setK2State(String state) // MÉTODO PARA CAMBIAR EL ESTADO DEL RELÉ K2
  {
    configDoc["Relay"]["K2"]["state"] = state;
    saveConfig();
  }

  void setK2TimerSelected(unsigned long timerSelected) // MÉTODO PARA CAMBIAR EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K2
  {
    configDoc["Relay"]["K2"]["timerSelected"] = timerSelected;
    saveConfig();
  }
  
  void setK2Timer(int timer) // MÉTODO PARA CAMBIAR EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K2
  {
    configDoc["Relay"]["K2"]["timer"] = timer;
    saveConfig();
  }

  void setK2ActivationTime(unsigned long activationTime) // MÉTODO PARA CAMBIAR EL TIEMPO DE ACTIVACIÓN DEL RELÉ K2
  {
    configDoc["Relay"]["K2"]["activationTime"] = activationTime;
    saveConfig();
  }

  void setK2DeactivationTime(unsigned long deactivationTime) // MÉTODO PARA CAMBIAR EL TIEMPO DE DESACTIVACIÓN DEL RELÉ K2
  {
    configDoc["Relay"]["K2"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }

  //MÉTODOS DE K3
  void setK3Active(bool active) // MÉTODO PARA CAMBIAR EL ESTADO DEL RELÉ K3
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

  void setK3TimerStatus(bool timerStatus) // MÉTODO PARA CAMBIAR EL MODO DEL RELÉ K1
  {
    configDoc["Relay"]["K3"]["timerStatus"] = timerStatus;
    saveConfig();
  }

  void setK3Name(String name) // MÉTODO PARA CAMBIAR EL NOMBRE DEL RELÉ K3
  {
    configDoc["Relay"]["K3"]["name"] = name;
    saveConfig();
    Serial.print("Nombre del relé K3 actualizado a ");
    Serial.println(name);
  }

  void setK3State(String state) // MÉTODO PARA CAMBIAR EL ESTADO DEL RELÉ K3
  {
    configDoc["Relay"]["K3"]["state"] = state;
    saveConfig();
  }

  void setK3TimerSelected(unsigned long timerSelected) // MÉTODO PARA CAMBIAR EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K3
  {
    configDoc["Relay"]["K3"]["timerSelected"] = timerSelected;
    saveConfig();
  }
  
  void setK3Timer(int timer) // MÉTODO PARA CAMBIAR EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K3
  {
    configDoc["Relay"]["K3"]["timer"] = timer;
    saveConfig();
  }

  void setK3ActivationTime(unsigned long activationTime) // MÉTODO PARA CAMBIAR EL TIEMPO DE ACTIVACIÓN DEL RELÉ K3
  {
    configDoc["Relay"]["K3"]["activationTime"] = activationTime;
    saveConfig();
  }

  void setK3DeactivationTime(unsigned long deactivationTime) // MÉTODO PARA CAMBIAR EL TIEMPO DE DESACTIVACIÓN DEL RELÉ K3
  {
    configDoc["Relay"]["K3"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }

  //MÉTODOS DE K4
  void setK4Active(bool active) // MÉTODO PARA CAMBIAR EL ESTADO DEL RELÉ K4
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

  void setK4TimerStatus(bool timerStatus) // MÉTODO PARA CAMBIAR EL MODO DEL RELÉ K1
  {
    configDoc["Relay"]["K4"]["timerStatus"] = timerStatus;
    saveConfig();
  }

  void setK4Name(String name) // MÉTODO PARA CAMBIAR EL NOMBRE DEL RELÉ K4
  {
    configDoc["Relay"]["K4"]["name"] = name;
    saveConfig();
    Serial.print("Nombre del relé K4 actualizado a ");
    Serial.println(name);
  }

  void setK4State(String state) // MÉTODO PARA CAMBIAR EL ESTADO DEL RELÉ K4
  {
    configDoc["Relay"]["K4"]["state"] = state;
    saveConfig();
  }

  void setK4TimerSelected(unsigned long timerSelected) // MÉTODO PARA CAMBIAR EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K4
  {
    configDoc["Relay"]["K4"]["timerSelected"] = timerSelected;
    saveConfig();
  }

  void setK4Timer(int timer) // MÉTODO PARA CAMBIAR EL TIEMPO DEL TEMPORIZADOR DEL RELÉ K4
  {
    configDoc["Relay"]["K4"]["timer"] = timer;
    saveConfig();
  }
 
  void setK4ActivationTime(unsigned long activationTime) // MÉTODO PARA CAMBIAR EL TIEMPO DE ACTIVACIÓN DEL RELÉ K4
  {
    configDoc["Relay"]["K4"]["activationTime"] = activationTime;
    saveConfig();
  }

  void setK4DeactivationTime(unsigned long deactivationTime)  // MÉTODO PARA CAMBIAR EL TIEMPO DE DESACTIVACIÓN DEL RELÉ K4
  {
    configDoc["Relay"]["K4"]["deactivationTime"] = deactivationTime;
    saveConfig();
  }


};

#endif