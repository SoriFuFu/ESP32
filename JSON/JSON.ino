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
    IPAddress ip = config.getWifiIP();
    IPAddress subnet = config.getWifiSubnet();
    IPAddress gateway = config.getWifiGateway();
    wifiConfig.initWifiPlus(wifiSSID.c_str(), wifiPassword.c_str(), ip, subnet, gateway, apSSID.c_str(), apPassword.c_str());

    IPAddress ipAddress = wifiConfig.getIPAddress();
    IPAddress subnetAddress = wifiConfig.getSubnetMask();
    IPAddress gatewayAddress = wifiConfig.getGatewayIP();
    config.setWifiIP(ipAddress.toString());
    config.setWifiSubnet(subnetAddress.toString());
    config.setWifiGateway(gatewayAddress.toString());
  }
  else if (wifiActive && !apActive) // Inicializar WiFi
  {
    String wifiSSID = config.getWifiSSID();
    String wifiPassword = config.getWifiPassword();
    IPAddress ip = config.getWifiIP();
    IPAddress subnet = config.getWifiSubnet();
    IPAddress gateway = config.getWifiGateway();
    wifiConfig.initWifi(wifiSSID.c_str(), wifiPassword.c_str(), ip, gateway, subnet);
  }
  else if (!wifiActive && apActive) // Inicializar AP
  {
    String apSSID = config.getApSSID();
    String apPassword = config.getApPassword();
    wifiConfig.initAP(apSSID.c_str(), apPassword.c_str());
    IPAddress ipAddress = wifiConfig.getAPIP();
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

    else if (action == "setWifiConfig") // ACTIVAR O DESACTIVAR WIFI
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
        String ipConfig = data["ip"];
        String subnetConfig = data["subnet"];
        String gatewayConfig = data["gateway"];

        IPAddress ip;
        IPAddress subnet;
        IPAddress gateway;

        wifiConfig.initWifi(ssid.c_str(), password.c_str(), ip, gateway, subnet);

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
          config.setWifiIP(ip.toString());
          config.setWifiSubnet(subnet.toString());
          config.setWifiGateway(gateway.toString());
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
      }
      else if (active == "false")
      {
        config.setApActive(false);
        config.setApStatus(false);
        wifiConfig.stopAP();
      }
    }

    //****CONFIGURACIÓN RELÉS****//
    if (action == "setRelayActive") // Configurar relés
    {
      String relay = data["relay"];

      if (relay == "K1")
      {
        bool K1Active = data["K1Active"];
        config.setK1Active(K1Active);
      }
      else if (relay == "K2")
      {
        bool K2Active = data["K2Active"];
        config.setK2Active(K2Active);
      }
      else if (relay == "K3")
      {
        bool K3Active = data["K3Active"];
        config.setK3Active(K3Active);
      }
      else if (relay == "K4")
      {
        bool K4Active = data["K4Active"];
        config.setK4Active(K4Active);
      }
    }
    else if (action == "setRelayName")
    {
      String relay = data["relay"];
      String relayName = data["relayName"];
      if (relay == "K1")
      {
          config.setK1Name(relayName);
      }
      else if (relay == "K2")
      {
          config.setK2Name(relayName);
      }
      else if (relay == "K3")
      {
          config.setK3Name(relayName);
      }
      else if (relay == "K4")
      {
          config.setK4Name(relayName);
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
