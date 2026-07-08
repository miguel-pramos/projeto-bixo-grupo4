#include <Arduino.h>
#include <pinos.h>

void setupEncoder()
{
    pinMode(CODERA1, INPUT_PULLUP);
    pinMode(CODERA2, INPUT_PULLUP);
}

void printEncoderSignals()
{
    Serial.printf("Leitor A: %d | Leitor B: %d\n", digitalRead(CODERA1), digitalRead(CODERA2));
}
