#include "config.h"
#include "wifi_config.h"
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

ConfigManager config;
WifiConfig wifiConfig;

#define INTERVALODELECTURA 1000

WebSocketsServer webSocketConfig = WebSocketsServer(81);
AsyncWebServer server(80); // Objeto del servidor HTTP

void setup()
{
  Serial.begin(115200);
  delay(1000);
  SPIFFS.begin();      // Inicializar el sistema de archivos SPIFFS
  config.loadConfig(); // Cargar la configuración al inicio

  bool wifiActive = config.getWifiActive(); // Verificar si el WiFi está activado
  bool staticIp = config.getWifiStaticIp(); // Verificar si la IP estática está activada
  bool apActive = config.getApActive();     // Verificar si el AP está activado

  if (wifiActive && apActive) // Inicializar WiFi y AP
  {
    String wifiSSID = config.getWifiSSID();
    String wifiPassword = config.getWifiPassword();
    String apSSID = config.getApSSID();
    String apPassword = config.getApPassword();
    wifiConfig.initWifiPlus(wifiSSID.c_str(), wifiPassword.c_str(), apSSID.c_str(), apPassword.c_str());
    if (staticIp) // Configurar IP estática
    {
      IPAddress ip = config.getWifiIP();
      IPAddress subnet = config.getWifiSubnet();
      IPAddress gateway = config.getWifiGateway();
      wifiConfig.configStaticIp(ip, gateway, subnet);
    }
    IPAddress ipAddress = wifiConfig.getIPAddress();
    IPAddress subnetAddress = wifiConfig.getSubnetMask();
    IPAddress gatewayAddress = wifiConfig.getGatewayIP();
    config.setWifiIP(ipAddress.toString());
    config.setWifiSubnet(subnetAddress.toString());
    config.setWifiGateway(gatewayAddress.toString());
    Serial.print("IP: ");
    Serial.println(ipAddress);
  }
  else if (wifiActive && !apActive) // Inicializar WiFi
  {
    String wifiSSID = config.getWifiSSID();
    String wifiPassword = config.getWifiPassword();
    wifiConfig.initWifi(wifiSSID.c_str(), wifiPassword.c_str());
    if (staticIp)
    {
      IPAddress ip = config.getWifiIP();
      IPAddress subnet = config.getWifiSubnet();
      IPAddress gateway = config.getWifiGateway();
      wifiConfig.configStaticIp(ip, gateway, subnet);
      Serial.print("IP: ");
      Serial.println(ip);
    }
    else
    {
      IPAddress ipAddress = wifiConfig.getIPAddress();
      Serial.print("IP: ");
      Serial.println(ipAddress);
    }
  }
  else if (!wifiActive && apActive) // Inicializar AP
  {
    String apSSID = config.getApSSID();
    String apPassword = config.getApPassword();
    wifiConfig.initAP(apSSID.c_str(), apPassword.c_str());
    IPAddress ipAddress = wifiConfig.getAPIP();
    Serial.print("IP: ");
    Serial.println(ipAddress);
  }

  // CONFIGURAR EL CANAL WEBSOCKET PARA LA CONFIGURACIÓN
  webSocketConfig.begin();
  webSocketConfig.onEvent(webSocketEventConfig);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });
  server.serveStatic("/", SPIFFS, "/");
  // INICIAR EL SERVIDOR HTTP

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

// Función para manejar los eventos del WebSocket de configuración
void webSocketEventConfig(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if (type == WStype_TEXT)
  {
    String message = String((char *)payload);
    DynamicJsonDocument data(1024);
    deserializeJson(data, message);

    String action = data["action"];
    //****CONFIGURACIÓN WIFI Y AP****//
    if (action == "getConfig") // Enviar la configuracion al cliente
    {                          //
      updateClientConfig();
    }

    else if (action == "getNetworks") // Buscar redes WiFi
    {
      Serial.println("Buscando redes WiFi");
      String networksJson = wifiConfig.searchNetworks();
      webSocketConfig.sendTXT(num, networksJson);
    }

    else if (action == "setWifiActive") // Activar/desactivar WiFi
    {
      String active = data["active"];
      if (active == "false")
      {
        config.setWifiActive(false);
        config.setWifiIP("");
        config.setWifiSubnet("");
        config.setWifiGateway("");
      }
      updateClientConfig();
    }

    else if (action == "setWifiConfig") // Conectar WiFi
    {
      bool wifiActive = data["wifiActive"];
      if (!wifiActive)
      {
        config.setWifiActive(false);
        config.setWifiStatus(false);
        config.setWifiSSID("");
        config.setWifiPassword("");
        config.setWifiIP("");
        config.setWifiSubnet("");
        config.setWifiGateway("");
        wifiConfig.disconnectWifi();
      }
      else
      {
        String ssid = data["ssid"];
        String password = data["password"];

        wifiConfig.initWifi(ssid.c_str(), password.c_str()); // Inicializar WiFi
        bool WifiConnection = wifiConfig.verifyConnection(); // Verificar la conexión
        if (WifiConnection)
        {
          config.setWifiActive(true);
          config.setWifiStatus(true);
          config.setWifiSSID(ssid);
          config.setWifiPassword(password);
          IPAddress ip = wifiConfig.getIPAddress();
          IPAddress subnet = wifiConfig.getSubnetMask();
          IPAddress gateway = wifiConfig.getGatewayIP();
          String message = "{ \"status\": \"true\", \"ip\": \"" + ip.toString() + "\", \"subnet\": \"" + subnet.toString() + "\", \"gateway\": \"" + gateway.toString() + "\" }";
          webSocketConfig.sendTXT(num, message);
          Serial.print("IP Wifi: ");
          Serial.println(ip);
        }

        else
        {
          webSocketConfig.sendTXT(num, "false");
        }
      }
    }

    else if (action == "setStaticIp") // Configurar IP estática
    {
      DynamicJsonDocument data(1024);
      deserializeJson(data, (char *)payload);
      bool staticIpEnabled = data["staticIp"];
      bool staticIp = config.getWifiStaticIp();
      if (staticIpEnabled)
      {
        Serial.println("Configurando IP estática");
        String ipConfig = data["ip"];
        String subnetConfig = data["subnet"];
        String gatewayConfig = data["gateway"];
        config.setWifiStaticIp(true);
        config.setWifiIP(ipConfig);
        config.setWifiSubnet(subnetConfig);
        config.setWifiGateway(gatewayConfig);
        IPAddress ip = config.getWifiIP();
        IPAddress subnet = config.getWifiSubnet();
        IPAddress gateway = config.getWifiGateway();
        wifiConfig.configStaticIp(ip, gateway, subnet);
        IPAddress ipWifi = wifiConfig.getIPAddress();
        IPAddress subnetWifi = wifiConfig.getSubnetMask();
        IPAddress gatewayWifi = wifiConfig.getGatewayIP();
        if (ipWifi == ip && subnetWifi == subnet && gatewayWifi == gateway)
        {
          Serial.println("IP estática configurada");
          Serial.print("IP: ");
          Serial.println(ip);
          sendMessage("true");
        }
        else
        {
          sendMessage("false");
        }
      }
      else if (!staticIpEnabled)
      {
        if (staticIp)
        {
          config.setWifiStaticIp(false);
          config.setWifiIP("");
          config.setWifiSubnet("");
          config.setWifiGateway("");
          wifiConfig.configWifiDHCP();
        }
      }
      updateClientConfig();
    }

    else if (action == "setApConfig") // Configurar AP
    {
      String active = data["active"];
      if (active == "true")
      {
        String ssid = config.getApSSID();
        String password = config.getApPassword();
        wifiConfig.initAP(ssid.c_str(), password.c_str());
        config.setApActive(true);
        config.setApStatus(true);
        IPAddress ip = wifiConfig.getAPIP();
        Serial.print("IP: ");
        Serial.println(ip);
      }
      else if (active == "false")
      {
        config.setApActive(false);
        config.setApStatus(false);
        wifiConfig.stopAP();
      }
    }

    //****CONFIGURACIÓN RELÉS****//
    if (action == "setRelayConfig") // Configurar relés
    {
      String relay = data["relay"];

      if (relay == "1")
      {
        bool K1Active = data["K1Active"];
        bool getK1Active = config.getK1Active();
        if (K1Active && !getK1Active)
        {
          String relayName = data["relayName"];
          String relayMode = data["relayMode"];
          config.setK1Active(true);
          config.setK1Name(relayName);
          config.setK1Mode(relayMode);
        }
        else if (!K1Active && getK1Active)
        {
          config.setK1Active(false);
          config.setK1Name("");
          config.setK1Mode("");
        }
      }
      else if (relay == "2"){
        bool K2Active = data["K2Active"];
        bool getK2Active = config.getK2Active();
        if (K2Active && !getK2Active)
        {
          String relayName = data["relayName"];
          String relayMode = data["relayMode"];
          config.setK2Active(true);
          config.setK2Name(relayName);
          config.setK2Mode(relayMode);
        }
        else if (!K2Active && getK2Active)
        {
          config.setK2Active(false);
          config.setK2Name("");
          config.setK2Mode("");
        }
      }
      else if (relay == "3"){
        bool K3Active = data["K3Active"];
        bool getK3Active = config.getK3Active();
        if (K3Active && !getK3Active)
        {
          String relayName = data["relayName"];
          String relayMode = data["relayMode"];
          config.setK3Active(true);
          config.setK3Name(relayName);
          config.setK3Mode(relayMode);
        }
        else if (!K3Active && getK3Active)
        {
          config.setK3Active(false);
          config.setK3Name("");
          config.setK3Mode("");
        }
      }
      else if (relay == "4"){
        bool K4Active = data["K4Active"];
        bool getK4Active = config.getK4Active();
        if (K4Active && !getK4Active)
        {
          String relayName = data["relayName"];
          String relayMode = data["relayMode"];
          config.setK4Active(true);
          config.setK4Name(relayName);
          config.setK4Mode(relayMode);
        }
        else if (!K4Active && getK4Active)
        {
          config.setK4Active(false);
          config.setK4Name("");
          config.setK4Mode("");
        }
      }
     
    }
  }
  else if (type == WStype_CONNECTED)
  {
    Serial.println("Cliente conectado");
  }
  else if (type == WStype_DISCONNECTED)
  {
    Serial.println("Cliente desconectado");
  }
}

void updateClientConfig()
{
  // Enviar la configuración actual al cliente
  String configJson;
  serializeJson(config.getConfigDoc(), configJson);
  webSocketConfig.broadcastTXT(configJson);
  Serial.println("Enviando configuración al cliente");
}
void sendMessage(String message)
{
  webSocketConfig.broadcastTXT(message);
}
void loop()
{
  webSocketConfig.loop();
  // RETARDO DE 1 SEGUNDO
  unsigned long tiempoActual = millis();
  static unsigned long ultimoTiempo = 0;
  if (tiempoActual - ultimoTiempo >= INTERVALODELECTURA)
  {
    ultimoTiempo = tiempoActual; // Actualizar el tiempo de la última lectura
                                 //*** ***//
  }
}
