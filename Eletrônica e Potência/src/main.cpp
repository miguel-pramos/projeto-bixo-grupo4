#include <Arduino.h>
#include <encoder.h>

// Declarações das funções dos módulos
void motores_init();
void motores_task();
void led_init();
void led_task();


void setSerial() {
    Serial.begin(115200);
    Serial.println("Hello, World!");
}

void setup()
{
    setSerial();
    setupEncoder();
    // motores_init();
    // led_init();
}

void loop()
{
    //motores_task();
    //led_task();
    printEncoderSignals();
    delay(200);
}

