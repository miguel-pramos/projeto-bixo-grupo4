#include <Arduino.h>
#include <pinos.h>

void motores_init()
{
    // Controlam a velocidade
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    //
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}

void motores_task()
{
    // Motor 1
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    // Motor 2
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    for (int i = 0; i <= 255; i++)
    {
        analogWrite(ENA, i);
        analogWrite(ENB, i);
        delay(20);
    }

    delay(2000);

    for (int i = 255; i >= 0; i--)
    {
        analogWrite(ENA, i);
        analogWrite(ENB, i);
        delay(20);
    }

    delay(2000);
}