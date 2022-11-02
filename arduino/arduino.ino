//Serial================================
#include <SoftwareSerial.h>

// Serial INITIALIZATION
const byte rxPin = 3;
const byte txPin = 4;

SoftwareSerial esp32(rxPin, txPin);

// JSON================================
#include <ArduinoJson.h>

// LCD =================================
#include <LiquidCrystal_I2C.h>

// LCD INITIALIZATION
LiquidCrystal_I2C lcd(0x27, 16, 2);

//DHT================================
#include "DHT.h"

// DHT INITIALIZATION
#define DHTPIN 5
#define DHTTYPE DHT11

DHT dht ( DHTPIN , DHTTYPE );

// etc ================================
int btn_pin = 2;
int btn_pin_2 = 13;
int btn_pin_3 = 12;
int btn_pin_4 = 11;

int red_led = 10;

int relay = 7;

int t_alert = 0;
int h_alert = 0;

volatile int state = 0;

void setup() {
    pinMode(btn_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(btn_pin), switchState, FALLING);

    pinMode(btn_pin_2, INPUT_PULLUP);
    pinMode(btn_pin_3, INPUT_PULLUP);
    pinMode(btn_pin_4, INPUT_PULLUP);

    pinMode(red_led, OUTPUT);
    pinMode(relay, OUTPUT);

    digitalWrite(red_led, LOW);
    digitalWrite(relay, LOW);

    Serial.begin(115200);
    esp32.begin(9600);

    lcd.begin();
    dht.begin();
}

void loop() {
    //Online
    while (esp32.available()) {
        float t = dht.readTemperature();
        float h = dht.readHumidity();
        setText(t, h);
        comm(t, h);
        if (digitalRead(btn_pin_2) == HIGH && state != 0) {
            if (state == 1) {
                t_alert = setAlert();
            }
            else if (state == 2) {
                h_alert = setAlert();
            }
        }
        checkAlert(t, h);
    }
    //Offline
    if (esp32.available() == 0) {
        float t = dht.readTemperature();
        float h = dht.readHumidity();
        setText(t, h);
        if (digitalRead(btn_pin_2) == HIGH && state != 0) {
            if (state == 1) {
                t_alert = setAlert();
            }
            else if (state == 2) {
                h_alert = setAlert();
            }
        }
        checkAlert(t, h);
    }
    delay(100);
}

void switchState () {
    if (state == 0) {
    state = 1;
  }
    else if (state == 1) {
    state = 2;
  }
    else {
        state = 0;
    }
}

void setText(float t, float h) {
    if (state == 0) {
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Temp:");
        lcd.setCursor(10, 0);
        lcd.print(t);
        lcd.setCursor(0, 1);
        lcd.print("Humidity:");
        lcd.setCursor(10, 1);
        lcd.print(h);
    }
    else if (state == 1) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temp:");
        lcd.setCursor(10, 0);
        lcd.print(t);
        
        lcd.setCursor(0, 1);
        lcd.print("Alert:");
        lcd.setCursor(10, 1);
        lcd.print(t_alert);
    }
    else if (state == 2){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Humidity:");
        lcd.setCursor(10, 0);
        lcd.print(h);

        lcd.setCursor(0, 1);
        lcd.print("Alert:");
        lcd.setCursor(10, 1);
        lcd.print(h_alert);
    }
}

int setAlert() {
    int temp = 0;
    delay(100);
    while (digitalRead(btn_pin_2) != HIGH) {
        if (digitalRead(btn_pin_3) == HIGH){
            temp += 1;
            delay(100);
        }
        else if (digitalRead(btn_pin_4) == HIGH) {
            temp -= 1;
            delay(100);
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Alert = ");
        lcd.setCursor(8, 0);
        lcd.print(temp);
    }
    return temp;
}
    

void checkAlert(float t, float h) {
    if (t_alert == 0 && h_alert == 0) {
        digitalWrite(red_led, LOW);
        digitalWrite(relay, LOW);
    }
    else if (t_alert <= t && h_alert == 0) {
        digitalWrite(red_led, HIGH);
        digitalWrite(relay, HIGH);
    }
    else if (h_alert <= h && t_alert == 0) {
        digitalWrite(red_led, HIGH);
        digitalWrite(relay, HIGH);
    }
    else if (t_alert <= t || h_alert <= h) {
        digitalWrite(red_led, HIGH);
        digitalWrite(relay, HIGH);
    }
    else {
        digitalWrite(red_led, LOW);
        digitalWrite(relay, LOW);
    }
}

void comm(float t, float h) {
    StaticJsonDocument<1024> data;
    data["t"] = t;
    data["h"] = h;
    serializeJson(data, esp32);
    delay(1000);
}
