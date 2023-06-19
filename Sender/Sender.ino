#include <esp_now.h>
#include <WiFi.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>             //SPI library for TFT screen

#define TFT_CS 17
#define TFT_RST 4
#define TFT_DC 16

uint8_t receiver_mac[6] = {0x40, 0x22, 0xD8, 0x05, 0x43, 0x88};
// 40:22:D8:05:43:88

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

float vBat = 0.0f;

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_MODE_STA);
    Serial.println(WiFi.macAddress());
    esp_now_init();

    esp_now_peer_info_t peer_info = {};
    peer_info.channel = 1;
    peer_info.encrypt = false;
    memcpy(peer_info.peer_addr, receiver_mac, 6);
    esp_now_add_peer(&peer_info);

    Serial.println("Init Sender");
    tft.initR(INITR_BLACKTAB); // Init ST7735S chip, black tab

    tft.setSPISpeed(50000000);
    tft.fillScreen(ST77XX_BLACK);
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.print("init");
    Serial.println(F("Initialized"));
    delay(800);
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.print("Battery   Voltage: ");
}

char charVal[6];

struct data_frame
{
    int pot1;
    int pot2;
    int pot3;
    int pot4;
    bool sw1;
    bool sw2;
    bool sw3;
};

void loop()
{
    bool sw1 = digitalRead(12);
    bool sw2 = digitalRead(14);
    bool sw3 = digitalRead(26);

    data_frame data = {};
    data.pot1 = analogRead(32);
    data.pot2 = analogRead(33);
    data.pot3 = analogRead(34);
    data.pot4 = analogRead(35);
    data.sw1 = sw1;
    data.sw2 = sw2;
    data.sw3 = sw3;

    vBat = (mapfloat(analogRead(36), 0, 4096, 0, 3.3) + 0.1) / 0.6875;

    Serial.println(analogRead(36));
    Serial.println("vBat: ");
    Serial.println(vBat);
    tft.setCursor(0, 20);
    tft.print(dtostrf(vBat, 3, 1, charVal));

    tft.setCursor(0, 30);
    tft.print("Sw1: ");
    if (sw1 == 1)
    {
        tft.print("Open");
    }
    else
    {
        tft.print("Closed");
    }

    tft.setCursor(0, 36);
    tft.print("Sw2: ");
    if (sw2 == 1)
    {
        tft.print("Open");
    }
    else
    {
        tft.print("Closed");
    }

    tft.setCursor(0, 42);
    tft.print("Sw3: ");
    if (sw3 == 1)
    {
        tft.print("Open");
    }
    else
    {
        tft.print("Closed");
    }

    esp_now_send(receiver_mac, (uint8_t *)&data, sizeof(data));
    delay(60);
    tft.fillScreen(ST77XX_BLACK);
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}