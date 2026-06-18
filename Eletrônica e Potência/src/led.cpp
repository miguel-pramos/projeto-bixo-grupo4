#include <Arduino.h>

void setup() {
  // código pra rodar só uma vez

  Serial.begin(115200);
  Serial.println("Oi, ESP32!");
}

void loop() {
  // código que repete

  delay(10); // espera 10 milissegundos
}
