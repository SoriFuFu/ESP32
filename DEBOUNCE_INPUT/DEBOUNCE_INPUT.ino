#include "ADebouncer.h"

#define setPin 13          // Define the set input pin.
#define resetPin 33        // Define the reset input pin.
#define debouncePeroid 500 // Define the debounce period in milliseconds
#define debounceReset 3000 // Define the debounce period in milliseconds
#define LED_BUILTIN 21

ADebouncer setButton;   // Declare set debouncer variable.
ADebouncer resetButton; // Declare reset debouncer variable.
bool STATE;             // Declare STATE variable for ResetSet.

void setup()
{
    Serial.begin(115200);                           // Initialize the serial monitor.
    pinMode(setPin, INPUT_PULLDOWN);                // Set the button mode as input pullup.
    pinMode(resetPin, INPUT_PULLUP);                // Set the button mode as input pullup.
    pinMode(LED_BUILTIN, OUTPUT);                   // Set the LED_BUILTIN mode as output.
    setButton.mode(INSTANT, debouncePeroid, LOW);   // Set the debounce mode as delayed mode and debounce period, with the initial output in a HIGH STATE.
    resetButton.mode(DELAYED, debounceReset, HIGH); // Set the debounce mode as instant mode and debounce period, with the initial output in a HIGH STATE.
    STATE = LOW;                                    // Initial STATE in a LOW STATE.
}

void loop()
{
    bool RIEGO_INTERIOR = setButton.debounce(digitalRead(setPin)); // Debounce input of the set button STATE.
    if (RIEGO_INTERIOR && !STATE)
    {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("RIEGO INTERIOR PULSADO");
    }
    else 
    {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("RIEGO INTERIOR PULSADO");
    }
    bool RESET = resetButton.debounce(digitalRead(resetPin)); // Debounce input of the reset button STATE.

    if (!RESET)
    {
        Serial.println("RESET PULSADO");
    }
}