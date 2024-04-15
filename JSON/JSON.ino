#include "config.h"
#include "wifi_config.h"
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

ConfigManager config;
WifiConfig wifiConfig;

WebSocketsServer webSocketConfig = WebSocketsServer(81);
WebSocketsServer webSocketSensors = WebSocketsServer(82);
WebSocketsServer webSocketK1 = WebSocketsServer(83);
WebSocketsServer webSocketK2 = WebSocketsServer(84);
WebSocketsServer webSocketK3 = WebSocketsServer(85);
WebSocketsServer webSocketK4 = WebSocketsServer(86);

void setup()
{
    Serial.begin(115200);
    delay(1000);

    SPIFFS.begin();

    // Cargar la configuración al inicio
    config.loadConfig();

    // Obtener el SSID y la contraseña del objeto WiFi
    String wifiSSID = config.getWifiSSID();
    String wifiPassword = config.getWifiPassword();
    // Obtener el SSID y la contraseña del objeto AP
    String apSSID = config.getApSSID();
    String apPassword = config.getApPassword();

    // Inicializar la conexión WiFi y el punto de acceso (AP) utilizando los valores obtenidos del objeto AP
    wifiConfig.initWifiPlus(wifiSSID.c_str(), wifiPassword.c_str(), apSSID.c_str(), apPassword.c_str());

    // Ejemplo de cómo obtener y actualizar datos de configuración de WiFi (opcional)
    bool wifiActive = config.getWifiActive();
    Serial.print("Wifi active: ");
    Serial.println(wifiActive);

    // Obtener el SSID de la red WiFi a la que está conectado el dispositivo
    String connectedSSID = wifiConfig.getConnectedSSID();
    Serial.print("Connected to WiFi SSID: ");
    Serial.println(connectedSSID);

    // Obtener la dirección IP asignada al dispositivo
    IPAddress ipAddress = wifiConfig.getIPAddress();
    Serial.print("IP Address: ");
    Serial.println(ipAddress);

    // Buscar y listar redes WiFi disponibles
    wifiConfig.searchNetworks();
    //CONFIGURAR EL CANAL WEBSOCKET PARA LA CONFIGURACIÓN
    webSocketConfig.begin();
    webSocketConfig.onEvent(webSocketEventConfig);
    // CONFIGURAR EL CANAL WEBSOCKET PARA LOS SENSORES
    webSocketSensors.begin();
    webSocketSensors.onEvent(webSocketEventSensors);
    // CONFIGURAR EL CANAL WEBSOCKET PARA K1
    webSocketK1.begin();
    webSocketK1.onEvent(webSocketEventK1);
    // CONFIGURAR EL CANAL WEBSOCKET PARA K2
    webSocketK2.begin();
    webSocketK2.onEvent(webSocketEventK2);
    // CONFIGURAR EL CANAL WEBSOCKET PARA K3
    webSocketK3.begin();
    webSocketK3.onEvent(webSocketEventK3);
    // CONFIGURAR EL CANAL WEBSOCKET PARA K4
    webSocketK4.begin();
    webSocketK4.onEvent(webSocketEventK4);
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
        // Tu código aquí
    }
}
// Función para manejar los eventos del WebSocket de sensores
void webSocketEventSensors(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    // Tu código aquí
}
// Función para manejar los eventos del WebSocket de K1
void webSocketEventK1(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    // Tu código aquí
}
// Función para manejar los eventos del WebSocket de K2
void webSocketEventK2(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    // Tu código aquí
}
// Función para manejar los eventos del WebSocket de K3
void webSocketEventK3(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    // Tu código aquí
}
// Función para manejar los eventos del WebSocket de K4
void webSocketEventK4(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    // Tu código aquí
}


void loop()
{
    webSocketConfig.loop();
    webSocketSensors.loop();
    webSocketK1.loop();
    webSocketK2.loop();
    webSocketK3.loop();
    webSocketK4.loop();

     // RETARDO DE 1 SEGUNDO
    unsigned long tiempoActual = millis();
    static unsigned long ultimoTiempo = 0;
    if (tiempoActual - ultimoTiempo >= INTERVALODELECTURA)
    {
        ultimoTiempo = tiempoActual; // Actualizar el tiempo de la última lectura
        
        // ENVÍO DE DATOS DE WIFI
// Obtener el SSID y la contraseña del objeto WiFi
    String wifiSSID = config.getWifiSSID();
    String wifiPassword = config.getWifiPassword();
    // Obtener el SSID y la contraseña del objeto AP
    String apSSID = config.getApSSID();
    String apPassword = config.getApPassword();



    }
}
