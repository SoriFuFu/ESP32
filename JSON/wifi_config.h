#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <WiFi.h>

class WifiConfig
{
public:
    WifiConfig() {}

    // Método para conectar al WiFi utilizando un SSID y contraseña
    void initWifi(const char *ssid, const char *password)
    {
        WiFi.begin(ssid, password);
        Serial.print("Connecting to WiFi");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.println("WiFi connected");
    }

    // Método para inicializar un punto de acceso (AP) con un SSID y contraseña
    void initAP(const char *ssid, const char *password)
    {
        WiFi.softAP(ssid, password);
        Serial.println("Access Point started");
    }

    // Método para conectar al WiFi utilizando un SSID y contraseña y configurar un punto de acceso simultáneo
    void initWifiPlus(const char *ssid, const char *password, const char *apSsid, const char *apPassword)
    {
        initWifi(ssid, password); // Conexión al WiFi
        initAP(apSsid, apPassword); // Inicialización del punto de acceso
    }

    // Método para buscar y listar redes WiFi disponibles
    void searchNetworks()
    {
        int numNetworks = WiFi.scanNetworks();
        Serial.println("WiFi Networks:");
        for (int i = 0; i < numNetworks; i++)
        {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(WiFi.SSID(i));
        }
    }

    // Método para configurar una IP estática
    void configStaticIp(IPAddress ip, IPAddress gateway, IPAddress subnet)
    {
        WiFi.config(ip, gateway, subnet);
        Serial.println("Static IP configured");
    }

        // Método para obtener el SSID de la red WiFi a la que está conectado el dispositivo
    String getConnectedSSID()
    {
        return WiFi.SSID();
    }

    // Método para obtener la dirección IP asignada al dispositivo
    IPAddress getIPAddress()
    {
        return WiFi.localIP();
    }

    // Método para obtener la dirección Subnet del dispositivo
    IPAddress getSubnetIP()
    {
        return WiFi.subnetMask();
    }

    // Método para obtener la dirección IP del gateway
    IPAddress getGatewayIP()
    {
        return WiFi.gatewayIP();
    }

    // Método para obtener la dirección MAC del dispositivo
    String getMACAddress()
    {
        return WiFi.macAddress();
    }

    // Método para obtener la intensidad de la señal WiFi
    int getSignalStrength()
    {
        return WiFi.RSSI();
    }

    // Método para desconectar el dispositivo del WiFi
    void disconnect()
    {
        WiFi.disconnect();
        Serial.println("WiFi disconnected");
    }

    // Método para desconectar el dispositivo del punto de acceso (AP)
    void stopAP()
    {
        WiFi.softAPdisconnect(true);
        Serial.println("Access Point stopped");
    }

    // Método para reiniciar la conexión WiFi
    void reconnect()
    {
        WiFi.reconnect();
        Serial.println("WiFi reconnected");
    }

    // Método para reiniciar el punto de acceso (AP)
    void restartAP()
    {
        WiFi.softAPdisconnect(true);
        Serial.println("Access Point restarted");
    }



};

#endif
