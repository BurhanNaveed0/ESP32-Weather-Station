#include "WiFi.h"
#include "HTTPClient.h"
#include "DHT.h"

#define DHTPIN 5
#define DHTTYPE DHT11

const char* ssid = "BAOO";
const char* password = "fcdde0eed24800";

DHT dht(DHTPIN, DHTTYPE);

void scan_networks() {
  Serial.println("Scan Start");

  int n = WiFi.scanNetworks();
  Serial.println("Scan done!");

  if (n == 0) {
      Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");
}

void init_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(9600);

  scan_networks();
  init_wifi();

  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
  Serial.print("Setup Done");

  dht.begin();
  Serial.print("Sensor Reading Has Begun");
}

void loop() {
  delay(5000);
  
  if(WiFi.status()== WL_CONNECTED){
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" C");
  
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.println(" %");
    
    HTTPClient http;
    http.begin("http://bore.pub:7654/api/weather?temperature=" + String(t) + "&humidity=" + String(h));     
    
    int httpResponseCode = http.PUT("");

    if(httpResponseCode > 0){
      String response = http.getString();   
      Serial.println(httpResponseCode);
      Serial.println(response);          
    } else {
      Serial.print("Error on sending PUT Request: ");
      Serial.println(httpResponseCode);
    }
 
    http.end();
   
  } else {
    Serial.println("Error in wifi connection...");
  }

}
