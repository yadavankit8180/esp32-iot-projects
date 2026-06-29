#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

#define DHTPIN 32
#define DHTTYPE DHT11

float tempSum = 0;
float humSum = 0;

int sampleCount = 0;

unsigned long previousMillis = 0;
const unsigned long uploadInterval = 5000;   // 5 seconds

DHT dht(DHTPIN,DHTTYPE);

const char* ssid="8209";
const char* password="12121212";

String GOOGLE_SCRIPT_ID=" ";

WebServer server(80);

float temperature=0;
float humidity=0;

void handleRoot()
{
String html="<!DOCTYPE html><html>";

html+="<head>";
html+="<meta http-equiv='refresh' content='1'>";
html+="<style>";
html+="body{font-family:Arial;text-align:center;background:#f2f2f2;}";
html+=".card{width:320px;margin:auto;padding:20px;background:white;";
html+="border-radius:12px;box-shadow:0 0 10px gray;}";
html+="h1{color:#0b7dda;}";
html+=".value{font-size:35px;font-weight:bold;color:#ff6600;}";
html+="</style>";
html+="</head>";

html+="<body>";
html+="<div class='card'>";
html+="<h1>ESP32 Weather Station</h1>";

html+="<h2>Temperature</h2>";
html+="<div class='value'>";
html+=String(temperature,1);
html+=" °C</div>";

html+="<hr>";

html+="<h2>Humidity</h2>";
html+="<div class='value'>";
html+=String(humidity,1);
html+=" %</div>";

html+="</div>";
html+="</body>";
html+="</html>";

server.send(200,"text/html",html);
}

void setup()
{
Serial.begin(115200);

dht.begin();

WiFi.begin(ssid,password);

while(WiFi.status()!=WL_CONNECTED)
{
delay(500);
Serial.print(".");
}

Serial.println();
Serial.println(WiFi.localIP());

server.on("/",handleRoot);
server.begin();
}

void loop()

{
  server.handleClient();

  // Read sensor every second
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (!isnan(temperature) && !isnan(humidity))
  {
    // Display latest values
    Serial.print("Temperature : ");
    Serial.print(temperature);
    Serial.print(" °C  ");

    Serial.print("Humidity : ");
    Serial.print(humidity);
    Serial.println(" %");

    // Store values for averaging
    tempSum += temperature;
    humSum += humidity;
    sampleCount++;
  }

  // Every 5 seconds upload average
  if (millis() - previousMillis >= uploadInterval)
  {
    previousMillis = millis();

    if (sampleCount > 0)
    {
      float avgTemp = tempSum / sampleCount;
      float avgHum = humSum / sampleCount;

      Serial.println("----------------------------");
      Serial.print("Average Temperature = ");
      Serial.println(avgTemp);

      Serial.print("Average Humidity = ");
      Serial.println(avgHum);
      Serial.println("----------------------------");

      WiFiClientSecure client;
      client.setInsecure();

      HTTPClient https;

      String url = GOOGLE_SCRIPT_ID +
                   "?temp=" + String(avgTemp,2) +
                   "&hum=" + String(avgHum,2);

      https.begin(client, url);

      int httpCode = https.GET();

      Serial.print("Google Response : ");
      Serial.println(httpCode);

      https.end();

      // Reset for next 5-second interval
      tempSum = 0;
      humSum = 0;
      sampleCount = 0;
    }
  }

  delay(1000);
}