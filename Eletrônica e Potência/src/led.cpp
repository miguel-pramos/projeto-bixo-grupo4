#include <Arduino.h>

void led_init()
{
  // código pra rodar só uma vez

  Serial.begin(115200);
  Serial.println("Oi, ESP32!");
}

void led_task()
{
  // código que repete

  delay(10); // espera 10 milissegundos
}
