#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
const int pot = A0;

#define ON_Board_LED 4
#define DHTPIN 5
#define DHTTYPE DHT11

const char* ssid = "-"; //--> Nama Wifi / SSID.
const char* password = "vikriramdani"; //-->  Password wifi .

const char* host = "script.google.com";
const int httpsPort = 443;

WiFiClientSecure client;
DHT dht(DHTPIN, DHTTYPE);

String GAS_ID = "AKfycbwpSfonhea1FQ2gWZh_8TMBoNiqI8CyBbiGCtoCKTMN_ZP4Iea93gCVvfNTe70d_i6P"; //--> spreadsheet script ID
void setup() {
  Serial.begin(115200);
  delay(500);

  dht.begin();
  WiFi.begin(ssid, password);
  Serial.println("");
    
  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, HIGH); //--> 

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
  }
  
  digitalWrite(ON_Board_LED, HIGH);
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  client.setInsecure();
}

void loop() {
//  nilai = analogRead(pot);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  sendData(t);
  delay(5000);
}

void sendData(float value) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String temp =  String(value, 2);
  String url = "/macros/s/" + GAS_ID + "/exec?nilai=" + temp;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
} 
