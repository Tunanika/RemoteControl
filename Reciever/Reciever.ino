#include "WiFi.h"
#include <esp_now.h>
#include <ESP32Servo.h>

Servo motor1;
Servo motor2;
Servo motor3;

int Motor1PWM = 0;
int Motor2PWM = 0;
int Motor3PWM = 0;

float tempX = 0.0f;
float tempY = 0.0f;

float tempMotorR = 0.0f;
float tempMotorL = 0.0f;

int lastMsg = 0;

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_MODE_STA);
    Serial.println(WiFi.macAddress());
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    motor1.attach(18);
    motor2.attach(19);
    motor3.attach(21);

    Serial.println("Init recieve");
    esp_now_register_recv_cb(OnDataRecv);
}
struct data_frame
{
    int pot1;
    int pot2;
    int pot3;
    int pot4;
};
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    data_frame ControllerData = {};
    memcpy(&ControllerData, incomingData, len);

    Motor3PWM = map(ControllerData.pot3, 0, 4096, 1000, 2000);

    tempX = mapfloat(ControllerData.pot1, 0, 4096, -1, 1);
    tempY = mapfloat(ControllerData.pot2, 0, 4096, -1, 1);

    tempMotorR = tempY - tempX;
    tempMotorL = tempY + tempX;

    Motor1PWM = mapfloat(tempMotorR, -1, 1, 1000, 2000);
    Motor2PWM = mapfloat(tempMotorL, -1, 1, 1000, 2000);

    Serial.println(millis());
    Serial.println("Pot1: " + String(ControllerData.pot1));
    Serial.println("Pot2: " + String(ControllerData.pot2));
    Serial.println("Motor1 PWM val: " + String(Motor1PWM));
    Serial.println("Motor2 PWM val: " + String(Motor2PWM));
    Serial.println("Motor3 PWM val: " + String(Motor3PWM));

    motor1.writeMicroseconds(Motor1PWM);
    motor2.writeMicroseconds(Motor2PWM);
    motor3.writeMicroseconds(Motor3PWM);
    lastMsg = millis();
}
void loop()
{
    delay(1000);
    if (millis() - 100 > lastMsg)
    {
        Serial.println("Waited too long to recieve message");
        killAll();
    }
}

void killAll()
{
    motor1.writeMicroseconds(1500);
    motor2.writeMicroseconds(1500);
    motor3.writeMicroseconds(1500);
    delay(500);
    Serial.println("Restarting...");
    ESP.restart();
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}