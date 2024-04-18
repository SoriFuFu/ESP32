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
    SPIFFS.begin();
    // Cargar la configuración al inicio
    config.loadConfig();

    bool wifiActive = config.getWifiActive();
    bool staticIp = config.getWifiStaticIp();
    bool apActive = config.getApActive();
    Serial.print("Static IP: ");
    Serial.println(staticIp);

    if (wifiActive && apActive)
    {
        String wifiSSID = config.getWifiSSID();
        String wifiPassword = config.getWifiPassword();
        String apSSID = config.getApSSID();
        String apPassword = config.getApPassword();
        // Inicializar la conexión WiFi utilizando los valores obtenidos del objeto WiFi
        wifiConfig.initWifiPlus(wifiSSID.c_str(), wifiPassword.c_str(), apSSID.c_str(), apPassword.c_str());
        if (staticIp)
        {
            IPAddress ip = config.getWifiIP();
            IPAddress subnet = config.getWifiSubnet();
            IPAddress gateway = config.getWifiGateway();
            wifiConfig.configStaticIp(ip, gateway, subnet);
            Serial.print("IP: ");
            Serial.println(ip);
        }
        IPAddress ipAddress = wifiConfig.getIPAddress();
        Serial.print("IP: ");
        Serial.println(ipAddress);
    }
    else if (wifiActive && !apActive)
    {
        String wifiSSID = config.getWifiSSID();
        String wifiPassword = config.getWifiPassword();
        // Inicializar la conexión WiFi utilizando los valores obtenidos del objeto WiFi
        wifiConfig.initWifi(wifiSSID.c_str(), wifiPassword.c_str());
        if (staticIp)
        {
            IPAddress ip = config.getWifiIP();
            IPAddress subnet = config.getWifiSubnet();
            IPAddress gateway = config.getWifiGateway();
            wifiConfig.configStaticIp(ip, gateway, subnet);
            Serial.print("IP: ");
            Serial.println(ip);
        }else{
            IPAddress ipAddress = wifiConfig.getIPAddress();
            Serial.print("IP: ");
            Serial.println(ipAddress);}
    }
    else if (!wifiActive && apActive)
    {
        String apSSID = config.getApSSID();
        String apPassword = config.getApPassword();
        // Inicializar el punto de acceso utilizando los valores obtenidos del objeto WiFi
        wifiConfig.initAP(apSSID.c_str(), apPassword.c_str());
        IPAddress ipAddress = wifiConfig.getIPAddress();
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

        if (action  == "getConfig")
        {
            Serial.println("Enviando configuración al cliente");
            // Enviar la configuración actual al cliente
          updateClientConfig();
        }
        else if (action  == "getNetworks")
        {
            Serial.println("Buscando redes WiFi");
            // Obtener el objeto JSON con la lista de redes WiFi
            String networksJson = wifiConfig.searchNetworks();

            // Enviar el objeto JSON al cliente a través del WebSocket
            webSocketConfig.sendTXT(num, networksJson);
        }else if (action  == "setStaticIp")
        {
            // Se recibió la orden para configurar la IP estática
            DynamicJsonDocument data(1024);
            deserializeJson(data, (char *)payload);
            bool staticIpEnabled = data["staticIp"];

            if (staticIpEnabled)
            {
            
                Serial.println("Configurando IP estática");
                config.setWifiStaticIp(true);
                Serial.print("IP: ");
                Serial.println(data["ip"].as<String>());
                config.setWifiIP(data["ip"].as<String>());
                Serial.print("Subnet: ");
                Serial.println(data["subnet"].as<String>());
                config.setWifiSubnet(data["subnet"].as<String>());
                Serial.print("Gateway: ");
                Serial.println(data["gateway"].as<String>());
                config.setWifiGateway(data["gateway"].as<String>());
                IPAddress ip = config.getWifiIP();
                IPAddress subnet = config.getWifiSubnet();
                IPAddress gateway = config.getWifiGateway();

                wifiConfig.configStaticIp(ip, gateway, subnet);
                

            }
            else
            {
            config.setWifiStaticIp(false);
            config.setWifiIP("");
            config.setWifiSubnet("");
            config.setWifiGateway("");
            wifiConfig.configWifiDHCP();

            updateClientConfig();
            }

            // Aquí puedes enviar una confirmación al cliente si es necesario
            // webSocketConfig.sendTXT(num, "Static IP configured");
        }
    }
}

void updateClientConfig()
{
    // Enviar la configuración actual al cliente
    String configJson;
    serializeJson(config.getConfigDoc(), configJson);
    webSocketConfig.broadcastTXT(configJson);
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
