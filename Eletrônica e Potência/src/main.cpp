#include <Arduino.h>

// Declarações das funções dos módulos
void motores_init();
void motores_task();
void led_init();
void led_task();

void setup()
{
    motores_init();
    led_init();
}

void loop()
{
    motores_task();
    led_task();
}
