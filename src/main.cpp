#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define WATER_SENSOR_PIN 34

const char* ssid = "Kul";
const char* password = "244466666";

String Web_App_URL = "https://script.google.com/macros/s/AKfycbzY7Sh1tyPIUKlIbh0WNzVxI8wXewMoh8Re6hxALSW-kFgk6XxFhOFdKh_k5rMT08LZ/exec";

DHT dht(DHTPIN, DHTTYPE);

unsigned long currentTime = millis();
unsigned long currentTime2 = millis();

float temp, humid, water_value, water_level;

void writeSheet() {
	String Send_Data_URL = Web_App_URL + "?sts=write";
	Send_Data_URL += "&temp=" + String(temp);
	Send_Data_URL += "&humi=" + String(humid);
	Send_Data_URL += "&water=" + String(map(water_value, 0, 4095, 0, 100));

	Serial.println();
	Serial.println("-------------");
	Serial.println("Send data to Google Spreadsheet...");
	Serial.print("URL : ");
	Serial.println(Send_Data_URL);

	HTTPClient http;
	http.begin(Send_Data_URL.c_str());
	http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

	int httpCode = http.GET(); 
	Serial.print("HTTP Status Code : ");
	Serial.println(httpCode);

	String payload;
	if (httpCode > 0) {
		payload = http.getString();
		Serial.println("Payload : " + payload);    
	}

	http.end();
}

void setup(){
	Serial.begin(115200);
	dht.begin();

	Serial.println();
	Serial.println("-------------");
	Serial.println("WIFI mode : STA");
	WiFi.mode(WIFI_STA);
	Serial.println("-------------");

	Serial.println();
	Serial.println("------------");
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);

	int timeWait = 30; //--> 30 seconds.
  	while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
		if (timeWait > 0) timeWait--;
			if (timeWait == 0) {
			delay(1000);
			ESP.restart();
			}
		delay(1000);
	}
  	Serial.println("\nWiFi Connected!");
}

void loop(){
	temp = dht.readTemperature();
	humid = dht.readHumidity();
	water_value= analogRead(WATER_SENSOR_PIN);

	int water_level = map(water_value, 0, 4095, 0 ,100);

	Serial.println("---------------------");
	Serial.println("Temperature: ");
	Serial.println(temp);
	Serial.println("Humidity: ");
	Serial.println(humid);
	Serial.println("Water Level: ");
	Serial.println(water_level);

	if (millis() - currentTime2 > 10000) {
		writeSheet();
		currentTime2 = millis();
	}

	delay(1000);

}


