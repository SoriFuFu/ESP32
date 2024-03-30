#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>
#include "time.h"

#define LCD_ADDR 0x27    // Dirección de la pantalla LCD
#define BMP280_ADDR 0x76 // Dirección del sensor BMP280 (temperatura y presión)
#define SDA 2            // Pin SDA para la comunicación I2C
#define SCL 4            // Pin SCL para la comunicación I2C

// Pines de entrada
#define RESET_BUTTON_PIN 33 // Pin para el botón de reset
#define ARRIBA 14           // Pin para el botón de arriba
#define ABAJO 15            // Pin para el botón de abajo
#define IZQUIERDA 12        // Pin para el botón de izquierda
#define DERECHA 32          // Pin para el botón de derecha
#define BOTON_EXTERIOR 22   // Pin para el botón de riego exterior
#define BOTON_INTERIOR 13   // Pin para el botón de riego interior

// Pines de salida
#define RIEGO_EXTERIOR 21 // Pin para el riego exterior
#define RIEGO_INTERIOR 19 // Pin para el riego interior
#define LIBRE_1 18        // Pin para una salida libre
#define LIBRE_2 5         // Pin para otra salida libre

// Tamaño de la pantalla LCD (20 columnas x 4 filas)
#define LCD_COLS 20
#define LCD_ROWS 4

// Inicialización de la librería de la pantalla LCD
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Adafruit_BMP280 bmp;

// VARIABLES
// WIFI
const char *ssid = "Tarta-D";
const char *password = "OliviaPampo2024...";

#define LONG_PRESS_DURATION 3000 // Duración para considerar una pulsación larga en milisegundos
#define SHORT_PRESS_DURATION 500 // Duración para considerar una pulsación corta en milisegundos
#define INTERVALODELECTURA 2000  // Duración para considerar una pulsación larga en milisegundos

unsigned long tiempoRestante = 0;
int estadoBotonExterior = 0;
int estadoAnteriorBotonExterior = 0;
int contadorPulsacionesExterior = 0;
int estadoBotonInterior = 0;
int estadoAnteriorBotonInterior = 0;
int contadorPulsacionesInterior = 0;
unsigned long tiempoActual = 0;



enum RiegoState
{
    INACTIVO,
    ESPERA,
    ACTIVO,
    PAUSA
};

RiegoState estadoRiego1 = INACTIVO;
int tiempoSeleccionadoRiego1 = 0;
int tiempoRestanteRiego1 = 0;
RiegoState estadoRiego2 = INACTIVO;
int tiempoSeleccionadoRiego2 = 0;
int tiempoRestanteRiego2 = 0;

void setup()
{
    Serial.begin(115200);
    // Inicializar la comunicación I2C con los pines SDA y SCL especificados
    Wire.begin(SDA, SCL);
    configTime(3600, 0, "pool.ntp.org"); // Configura el desplazamiento horario a +1 hora (3600 segundos)

    // DISPLAY
    // SE INICIALIZA EL DISPLAY
    lcd.init();
    // ENCIENDE LA RETROILUMINACIÓN DE LA PANTALLA
    lcd.backlight();
    // LIMPIA LA PANTALLA
    lcd.clear();
    // SENSOR BMP280
    if (!bmp.begin(BMP280_ADDR))
    {
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
        while (1)
            ;
    }

    // Especificación del PIN como entrada o salida
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
    pinMode(ARRIBA, INPUT_PULLDOWN);
    pinMode(ABAJO, INPUT_PULLDOWN);
    pinMode(IZQUIERDA, INPUT_PULLDOWN);
    pinMode(DERECHA, INPUT_PULLDOWN);
    pinMode(BOTON_EXTERIOR, INPUT_PULLDOWN);
    pinMode(BOTON_INTERIOR, INPUT_PULLDOWN);

    pinMode(RIEGO_EXTERIOR, OUTPUT);
    pinMode(RIEGO_INTERIOR, OUTPUT);
    pinMode(LIBRE_1, OUTPUT);
    pinMode(LIBRE_2, OUTPUT);

    digitalWrite(RIEGO_EXTERIOR, LOW);
    digitalWrite(RIEGO_INTERIOR, LOW);
    digitalWrite(LIBRE_1, LOW);
    digitalWrite(LIBRE_2, LOW);

    // SE MUESTRA UN MENSAJE DE BIENVENIDA
    imprimirMensaje(0, 1, "Iniciando sistema...");
    delay(2000);
    lcd.clear();

    // SE INICIALIZA LA CONEXIÓN WIFI
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        lcd.setCursor(1, 1);
        lcd.print("Conectando WiFi...");
    }
    lcd.clear();
    lcd.setCursor(5, 1);
    lcd.print("Conectado");
    lcd.setCursor(2, 2);
    lcd.print("IP: ");
    lcd.print(WiFi.localIP().toString());
    delay(5000);
    lcd.clear();
}
void loop()
{
    tiempoActual = millis();
    static unsigned long ultimoTiempo = 0;
    // Realizar la lectura solo si ha pasado el intervalo de tiempo
    if (tiempoActual - ultimoTiempo >= INTERVALODELECTURA)
    {
        ultimoTiempo = tiempoActual; // Actualizar el tiempo de la última lectura
        int estadoBoton = digitalRead(RESET_BUTTON_PIN); // Lee el estado del botón de reset
        if (estadoBoton == LOW)
        { // Si el botón de reset está presionado (estado bajo)
            Serial.println("Boton de reset presionado. Reiniciando...");
            ESP.restart(); // Reinicia la placa ESP32
        }
    }

    const unsigned long DEBOUNCE_DELAY = 500; // Define el retraso de debounce en milisegundos
    static unsigned long lastDebounceTimeExterior = 0; // La última vez que el pin de entrada fue activado
    static unsigned long lastDebounceTimeInterior = 0;

    estadoBotonExterior = digitalRead(BOTON_EXTERIOR);
    estadoBotonInterior = digitalRead(BOTON_INTERIOR);
    if (tiempoActual - lastDebounceTimeExterior > DEBOUNCE_DELAY)
    {
        if (estadoBotonExterior == LOW && estadoAnteriorBotonExterior == HIGH)
        {
            lastDebounceTimeExterior = tiempoActual; // Actualiza la última vez que el botón fue activado
            Serial.println("Boton exterior presionado");
            estadoAnteriorBotonExterior = LOW;
        }
        else if (estadoBotonExterior == HIGH && estadoAnteriorBotonExterior == LOW)
        {
            lastDebounceTimeExterior = tiempoActual; // Actualiza la última vez que el botón fue activado
            Serial.println("Boton exterior presionado largo");
            estadoAnteriorBotonExterior = LOW;
        }
    }

    if (estadoBotonExterior == LOW && estadoAnteriorBotonExterior == HIGH && (tiempoActual - lastDebounceTimeExterior) > DEBOUNCE_DELAY){
        lastDebounceTimeExterior = millis(); // Actualiza la última vez que el botón fue activado
        Serial.println("Boton exterior presionado");
        estadoAnteriorBotonExterior = LOW;
    } else if (estadoBotonExterior == HIGH && estadoAnteriorBotonExterior == LOW && (tiempoActual - lastDebounceTimeInterior) > LONG_PRESS_DURATION){
        lastDebounceTimeInterior = millis(); // Actualiza la última vez que el botón fue activado
        Serial.println("Boton interior presionado largo");
        estadoAnteriorBotonExterior = LOW;
    } 

    if (estadoBotonInterior == LOW && estadoAnteriorBotonInterior == HIGH && (tiempoActual - lastDebounceTimeInterior) > DEBOUNCE_DELAY){
        lastDebounceTimeInterior = millis(); // Actualiza la última vez que el botón fue activado
        Serial.println("Boton interior presionado");
        estadoAnteriorBotonInterior = LOW;
    }   else if (estadoBotonInterior == HIGH && estadoAnteriorBotonInterior == LOW && (tiempoActual - lastDebounceTimeInterior) > LONG_PRESS_DURATION){
        lastDebounceTimeInterior = millis(); // Actualiza la última vez que el botón fue activado
        Serial.println("Boton interior presionado largo");
        estadoAnteriorBotonInterior = LOW;
    }

    estadoAnteriorBotonExterior = estadoBotonExterior;
    estadoAnteriorBotonInterior = estadoBotonInterior;
       
}

// void loop()
// {
//     unsigned long tiempoActual = millis();
//     static unsigned long ultimoTiempo = 0;
//     static unsigned long tiempoInicioPulsacionExterior = 0;
//     static unsigned long tiempoInicioPulsacionInterior = 0;

//     // Realizar la lectura solo si ha pasado el intervalo de tiempo
//     if (tiempoActual - ultimoTiempo >= INTERVALODELECTURA) {
//         ultimoTiempo = tiempoActual; // Actualizar el tiempo de la última lectura

//         int estadoBoton = digitalRead(RESET_BUTTON_PIN); // Lee el estado del botón de reset

//         if (estadoBoton == LOW) { // Si el botón de reset está presionado (estado bajo)
//             Serial.println("Boton de reset presionado. Reiniciando...");
//             ESP.restart(); // Reinicia la placa ESP32
//         }
//     }

//     estadoBotonExterior = digitalRead(BOTON_EXTERIOR);
//     estadoBotonInterior = digitalRead(BOTON_INTERIOR);

//    if (estadoBotonExterior == LOW && estadoAnteriorBotonExterior == HIGH && (millis() - tiempoPressBotonExterior >= SHORT_PRESS_DURATION)) {
//         Serial.println("Boton exterior presionado");
//         tiempoPressBotonExterior = millis(); // Almacenar el tiempo de inicio de la pulsación exterior
//         estadoAnteriorBotonExterior = LOW;
//     } else if (estadoBotonExterior == HIGH && estadoAnteriorBotonExterior == LOW) {
//         estadoAnteriorBotonExterior = HIGH;
//     } else if (estadoBotonExterior == HIGH && (millis() - tiempoPressBotonExterior == LONG_PRESS_DURATION)) {
//         Serial.println("Boton exterior presionado largo");
//     }

//     if (estadoBotonInterior == LOW && estadoAnteriorBotonInterior == HIGH && (millis() - tiempoPressBotonInterior >= SHORT_PRESS_DURATION)){
//         Serial.println("Boton interior presionado");
//         tiempoPressBotonInterior = millis(); // Almacenar el tiempo de inicio de la pulsación interior
//         estadoAnteriorBotonInterior = LOW;
//     } else if (estadoBotonInterior == HIGH && estadoAnteriorBotonInterior == LOW) {
//         estadoAnteriorBotonInterior = HIGH;
//     } else if (estadoBotonInterior == HIGH && (millis() - tiempoPressBotonInterior == LONG_PRESS_DURATION)) {
//         Serial.println("Boton interior presionado largo");
//     }

//     // // Verificar si se ha mantenido presionado el botón exterior durante más de 3 segundos
//     // if (estadoBotonExterior == HIGH && tiempoInicioPulsacionExterior == 0) {
//     //     tiempoInicioPulsacionExterior = millis(); // Iniciar temporizador al detectar una pulsación
//     //      if (estadoRiego1 == INACTIVO)
//     //     {
//     //         configTemp("RIEGO_EXTERIOR"); // Configurar el temporizador si el riego está inactivo
//     //     }
//     //     else if (estadoRiego1 == ESPERA)
//     //     {
//     //         iniciarRiego("RIEGO_EXTERIOR"); // Iniciar riego si el estado es activo
//     //     }
//     //     else if (estadoRiego1 == ACTIVO)
//     //     {
//     //         pausarRiego("RIEGO_EXTERIOR"); // Pausar riego si el estado es activo
//     //     }
//     //     else if (estadoRiego1 == PAUSA)
//     //     {
//     //         continuarRiego("RIEGO_EXTERIOR"); // Continuar riego si el estado es pausa
//     //     }
//     // }

//     // if (estadoBotonExterior == LOW && tiempoInicioPulsacionExterior != 0) {
//     //     if (tiempoActual - tiempoInicioPulsacionExterior >= LONG_PRESS_DURATION) {
//     //         Serial.println("Pulsación larga exterior detectada");
//     //         apagarRiego("RIEGO_EXTERIOR"); // Apagar riego en caso de pulsación larga
//     //     }
//     //     tiempoInicioPulsacionExterior = 0; // Reiniciar el tiempo de inicio de pulsación
//     // }

//     // // Verificar si se ha mantenido presionado el botón interior durante más de 3 segundos
//     // if (estadoBotonInterior == HIGH && tiempoInicioPulsacionInterior == 0) {
//     //     tiempoInicioPulsacionInterior = millis(); // Iniciar temporizador al detectar una pulsación
//     //      if (estadoRiego1 == INACTIVO)
//     //     {
//     //         configTemp("RIEGO_EXTERIOR"); // Configurar el temporizador si el riego está inactivo
//     //     }
//     //     else if (estadoRiego1 == ESPERA)
//     //     {
//     //         iniciarRiego("RIEGO_EXTERIOR"); // Iniciar riego si el estado es activo
//     //     }
//     //     else if (estadoRiego1 == ACTIVO)
//     //     {
//     //         pausarRiego("RIEGO_EXTERIOR"); // Pausar riego si el estado es activo
//     //     }
//     //     else if (estadoRiego1 == PAUSA)
//     //     {
//     //         continuarRiego("RIEGO_EXTERIOR"); // Continuar riego si el estado es pausa
//     //     }
//     // }

//     // if (estadoBotonInterior == LOW && tiempoInicioPulsacionInterior != 0) {
//     //     if (tiempoActual - tiempoInicioPulsacionInterior >= LONG_PRESS_DURATION) {
//     //         Serial.println("Pulsación larga interior detectada");
//     //         apagarRiego("RIEGO_INTERIOR"); // Apagar riego en caso de pulsación larga
//     //     }
//     //     tiempoInicioPulsacionInterior = 0; // Reiniciar el tiempo de inicio de pulsación
//     // }

// }

void configTemp(String riego)
{
    if (riego == "RIEGO_EXTERIOR")
    {
        tiempoSeleccionadoRiego1 = solicitarTiempo(riego);
        estadoRiego1 = ESPERA;
        tiempoRestanteRiego1 = tiempoSeleccionadoRiego1;
    }
    else if (riego == "RIEGO_INTERIOR")
    {
        tiempoSeleccionadoRiego2 = solicitarTiempo(riego);
        estadoRiego2 = ESPERA;
        tiempoRestanteRiego2 = tiempoSeleccionadoRiego2;
    }
}



void iniciarRiego(String riego)
{
    if (riego == "RIEGO_EXTERIOR")
    {
        digitalWrite(RIEGO_EXTERIOR, HIGH);
        estadoRiego1 = ACTIVO;
        Serial.println("Riego exterior iniciado");       
    }
    else if (riego == "RIEGO_INTERIOR")
    {
        digitalWrite(RIEGO_INTERIOR, HIGH);
        estadoRiego2 = ACTIVO;
        Serial.println("Riego interior iniciado");
    }
}

void pausarRiego(String riego)
{
    if (riego == "RIEGO_EXTERIOR")
    {
        digitalWrite(RIEGO_EXTERIOR, LOW);
        estadoRiego1 = PAUSA;
        Serial.println("Riego exterior pausado");
    }
    else if (riego == "RIEGO_INTERIOR")
    {
        digitalWrite(RIEGO_INTERIOR, LOW);
        estadoRiego2 = PAUSA;
        Serial.println("Riego interior pausado");
    }
}

void continuarRiego(String riego)
{
    if (riego == "RIEGO_EXTERIOR")
    {
        digitalWrite(RIEGO_EXTERIOR, HIGH);
        estadoRiego1 = ACTIVO;
        Serial.println("Riego exterior continuado");
    }
    else if (riego == "RIEGO_INTERIOR")
    {
        digitalWrite(RIEGO_INTERIOR, HIGH);
        estadoRiego2 = ACTIVO;
        Serial.println("Riego interior continuado");
    }
}

void apagarRiego(String riego)
{
    if (riego == "RIEGO_EXTERIOR")
    {
        digitalWrite(RIEGO_EXTERIOR, LOW);
        estadoRiego1 = INACTIVO;
        Serial.println("Riego exterior apagado");
    }
    else if (riego == "RIEGO_INTERIOR")
    {
        digitalWrite(RIEGO_INTERIOR, LOW);
        estadoRiego2 = INACTIVO;
        Serial.println("Riego interior apagado");
    }
}

void imprimirMensaje(int COL, int ROW, String mensaje)
{
    static unsigned long tiempoInicioLimpiarLinea = 0;

    unsigned long tiempoActual = millis();

    lcd.setCursor(COL, ROW);
    lcd.print(mensaje);

    // Si han pasado 3 segundos desde la última impresión, limpia la línea
    if (tiempoActual - tiempoInicioLimpiarLinea >= 3000)
    {
        clearLCDLine(ROW); // Limpia la línea especificada
    }

    // Actualiza el tiempo de inicio para el próximo temporizador
    tiempoInicioLimpiarLinea = tiempoActual;
}
void clearLCDLine(int ROW)
{
    lcd.setCursor(0, ROW); // Mueve el cursor al inicio de la línea 'ROW'
    for (int n = 0; n < 20; n++)
    {                   // Suponiendo que es un LCD de 20 caracteres de ancho
        lcd.print(" "); // Imprime un espacio en blanco en cada posición
    }
}
