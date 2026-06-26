#include <Arduino.h>

//Analógicos
#define ENA 1
#define ENB 2
//Digitais
//Motor 1
#define IN1 3
#define IN2 4
//Motor 2
#define IN3 5
#define IN4 6

void setup()
{
    //Controlam a velocidade
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    //
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}

void loop()
{
    //Motor 1
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    //Motor 2
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    for (int i = 0; i <= 255; i++){
        analogWrite(ENA1, i);
        analogWrite(ENA2, i);
        delay(20);  
    }

    delay(2000);

    for (int i = 255; i >= 0; i--){
        analogWrite(ENA1, i);
        analogWrite(ENA2, i);
        delay(20);  
    }

    delay(2000);
}