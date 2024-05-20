#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <WiFi.h>
class WifiConfig
{
public:
   WifiConfig()
    {
    }

    // Método para conectar al WiFi utilizando un SSID y contraseña
    bool initWifi(const char *ssid, const char *password, IPAddress ip, IPAddress subnet, IPAddress gateway) // MÉTODO PARA INICIAR UNA CONEXIÓN WIFI
    {

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
                Serial.println(" No se pudo conectar al WiFi");
                return false;
            }
        }
        WiFi.config(ip, gateway, subnet);
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("");
            Serial.println("WiFi conectado");
            Serial.println("IP estática configurada correctamente");
            Serial.print("IP: ");
            Serial.println(WiFi.localIP());
            Serial.print("Subnet: ");
            Serial.println(WiFi.subnetMask());
            Serial.print("Puerta de enlace: ");
            Serial.println(WiFi.gatewayIP());  
        }
        return true; 
    }

    bool initAP(const char *ssid, const char *password) // MÉTODO PARA INICIAR UN PUNTO DE ACCESO
    {
        WiFi.softAP(ssid, password);
        Serial.println("Punto de acceso iniciado");
        Serial.print("IP: ");
        Serial.println(WiFi.softAPIP());
        return true;
    }

    void setApSSID(const char *ssid) // MÉTODO PARA CAMBIAR EL NOMBRE DEL PUNTO DE ACCESO
    {
        WiFi.softAP(ssid);
        Serial.println("Punto de acceso configurado");
    }

    bool initWifiPlus(const char *ssid, const char *password, IPAddress ip, IPAddress subnet, IPAddress gateway, const char *apSsid, const char *apPassword) // MÉTODO PARA INICIAR UNA CONEXIÓN WIFI Y UN PUNTO DE ACCESO
    {
        bool wifi = initWifi(ssid, password, ip, subnet, gateway);
        bool ap = initAP(apSsid, apPassword);
        return wifi && ap;
    }

    void changeStaticIP(IPAddress ip, IPAddress subnet, IPAddress gateway) // MÉTODO PARA CAMBIAR LA CONFIGURACIÓN DE LA DIRECCIÓN IP
    {
        WiFi.config(ip, gateway, subnet);
        Serial.println("Configuración de IP estática cambiada");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("Subnet: ");
        Serial.println(WiFi.subnetMask());
        Serial.print("Puerta de enlace: ");
        Serial.println(WiFi.gatewayIP());
    }
   
    String searchNetworks() // MÉTODO PARA BUSCAR REDES DISPONIBLES
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

    String getConnectedSSID() // MÉTODO PARA OBTENER EL NOMBRE DE LA RED A LA QUE ESTÁ CONECTADO EL DISPOSITIVO
    {
        return WiFi.SSID();
    }

    IPAddress getAPIP() // MÉTODO PARA OBTENER LA DIRECCIÓN IP DEL PUNTO DE ACCESO
    {
        return WiFi.softAPIP();
    }

    IPAddress getIPAddress() // MÉTODO PARA OBTENER LA DIRECCIÓN IP DEL DISPOSITIVO
    {
        return WiFi.localIP();
    }

    IPAddress getSubnetMask() // MÉTODO PARA OBTENER LA MÁSCARA DE SUBRED
    {
        return WiFi.subnetMask();
    }

    IPAddress getGatewayIP() // MÉTODO PARA OBTENER LA PUERTA DE ENLACE
    {
        return WiFi.gatewayIP();
    }

    String getMACAddress() // MÉTODO PARA OBTENER LA DIRECCIÓN MAC DEL DISPOSITIVO
    {
        return WiFi.macAddress();
    }

    int getWifiSignal() // MÉTODO PARA OBTENER LA POTENCIA DE LA SEÑAL
    {
        return WiFi.RSSI();
    }

    void disconnectWifi() // MÉTODO PARA DESCONECTAR EL DISPOSITIVO DE LA RED WIFI
    {
        WiFi.disconnect();
        Serial.println("WiFi desconectado");
    }

    void stopAP() // MÉTODO PARA DETENER EL PUNTO DE ACCESO
    {
        WiFi.softAPdisconnect(true);
        Serial.println("AP Desconectada");
    }

    void reconnect() // MÉTODO PARA RECONECTAR EL DISPOSITIVO A LA RED WIFI
    {
        WiFi.reconnect();
        Serial.println("WiFi reconectado");
    }

    bool verifyWifiConnection() // MÉTODO PARA VERIFICAR SI EL DISPOSITIVO ESTÁ CONECTADO A UNA RED WIFI
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
