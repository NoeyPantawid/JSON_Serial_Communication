//Serial================================
#include <SoftwareSerial.h>

// Serial INITIALIZATION
const byte rxPin = 16;
const byte txPin = 17;

SoftwareSerial arduino(rxPin, txPin);

// JSON================================
#include <ArduinoJson.h>

// WiFi================================
#include <WiFi.h>

WiFiClient client;

String thingSpeakAddress= "YOUR ADDRESS";
String tsfield1Name;
String request_string;
String apiKey="YOUR ADDRESS";

void setup() {
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    Serial.begin(115200);
    arduino.begin(9600);

    WiFi.disconnect();
    WiFi.begin("SSID","PASSWORD");

    while ((!(WiFi.status() == WL_CONNECTED))){
        delay(500);
        Serial.println("Connecting");
    }
    Serial.println("Connected!");
    Serial.println((WiFi.localIP()));
}

void loop() {
    while (arduino.available()) {
        comm();
    }
    delay(100);
}

void comm() {
    StaticJsonDocument<1024> data;
    DeserializationError error = deserializeJson(data, arduino);
    if (error) {
        Serial.println(error.f_str());
        return;
    }
    else {
        float t = data["t"];
        float h = data["h"];

        Serial.println(t);
        Serial.println(h);

        thingSpeak(t, h);
    }
    delay(1000);
}

void thingSpeak(float t, float h) {
    if (client.connect("api.thingspeak.com",80)) {
        request_string = thingSpeakAddress;
        request_string += "&field1=";
        request_string += t;
        request_string += "&field2=";
        request_string += h;
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(request_string.length());
        client.print("\n\n");
        client.print(request_string);
    }
}