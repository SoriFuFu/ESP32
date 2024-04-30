#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <WiFi.h>

class WifiConfig
{
public:
    WifiConfig() {}

    // Método para conectar al WiFi utilizando un SSID y contraseña
    void initWifi(const char *ssid, const char *password, IPAddress ip, IPAddress gateway, IPAddress subnet)
    {
  
        WiFi.config(ip, gateway, subnet);
        WiFi.begin(ssid, password);

        Serial.print("Conectando a WiFi ");
        Serial.print(ssid);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
            attempts++;

            if (attempts > 40)
            {
                Serial.println("No se pudo conectar al WiFi");
                // Intentar reconectar después de un pequeño retraso
                delay(5000);
                WiFi.begin(ssid, password);
                attempts = 0; // Reiniciar el contador de intentos
            }
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("");
            Serial.println("WiFi conectado");
            Serial.println("IP estática configurada correctamente");
            Serial.print("IP: ");
            Serial.println(WiFi.localIP());
            // Serial.print("Subnet: ");
            // Serial.println(WiFi.subnetMask());
            // Serial.print("Puerta de enlace: ");
            // Serial.println(WiFi.gatewayIP());
        }
    }

    // Método para inicializar un punto de acceso (AP) con un SSID y contraseña
    void initAP(const char *ssid, const char *password)
    {
        WiFi.softAP(ssid, password);
        Serial.println("Punto de acceso iniciado");
        Serial.print("IP: ");
        Serial.println(WiFi.softAPIP());
    }
    void setApSSID(const char *ssid)
    {
        WiFi.softAP(ssid);
        Serial.println("Punto de acceso configurado");
    }

    // Método para conectar al WiFi utilizando un SSID y contraseña y configurar un punto de acceso simultáneo
    void initWifiPlus(const char *ssid, const char *password,IPAddress ip, IPAddress gateway, IPAddress subnet, const char *apSsid, const char *apPassword)
    {
        initWifi(ssid, password, ip, subnet, gateway);   // Conexión al WiFi
        initAP(apSsid, apPassword); // Inicialización del punto de acceso
    }

    // Método para buscar y listar redes WiFi disponibles
    String searchNetworks()
    {
        DynamicJsonDocument doc(1024);
        JsonArray networksArray = doc.createNestedArray("networks");
        int numNetworks = WiFi.scanNetworks();
        for (int i = 0; i < numNetworks; i++)
        {
            networksArray.add(WiFi.SSID(i));
        }

        String networksJson;
        serializeJson(doc, networksJson);

        return networksJson;
    }

    // // Método para configurar una IP estática
    // void configStaticIp(IPAddress ip, IPAddress gateway, IPAddress subnet)
    // {
    //     WiFi.config(ip, gateway, subnet);
    //     Serial.println("IP estática configurada");
    // }

    // Método para obtener el SSID de la red WiFi a la que está conectado el dispositivo
    String getConnectedSSID()
    {
        return WiFi.SSID();
    }

    // Método para obtener la dirección IP asignada del AP

    IPAddress getAPIP()
    {
        return WiFi.softAPIP();
    }

    // Método para obtener la dirección IP asignada al dispositivo
    IPAddress getIPAddress()
    {
        return WiFi.localIP();
    }

    // Método para obtener la dirección Subnet del dispositivo
    IPAddress getSubnetMask()
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
    void disconnectWifi()
    {
        WiFi.disconnect();
        Serial.println("WiFi desconectado");
    }

    // Método para desconectar el dispositivo del punto de acceso (AP)
    void stopAP()
    {
        WiFi.softAPdisconnect(true);
        Serial.println("AP Desconectada");
    }

    // Método para reiniciar la conexión WiFi
    void reconnect()
    {
        WiFi.reconnect();
        Serial.println("WiFi reconectado");
    }

    // Método para verificar si el dispositivo está conectado a una red WiFi
    bool verifyConnection()
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

#endif
