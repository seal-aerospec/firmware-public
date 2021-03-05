#include "awsCredentials.h"
#include "wifiCredential.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
//#include <cstdlib.h> // for the random function

// for time and date
#define seconds(x) (x/1000)%(1000*60) // x in milliseconds; total num seconds mod minute
#define minutes(x) (x/(1000*60))%(1000*60*60) // x in milliseconds; total num minutes mod hour
#define hours(x) (x/(1000*60*60))%(1000*60*60*24) // x in milliseconds; total num hours mod day
#define day(x) (x/(1000*60*60*24))%(1000*60*60*24*28) // x in milliseconds; total days mod month; arbitrarily chosing feb as starting, max of millis() is 50 days
#define month(x) 2+(x/(1000*60*60*24*28)) //x in milliseconds

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);
  
void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // connect to Wifi
  Serial.println("Connecting to Wi-Fi");

  // wait until wifi connected
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  delay(1000);
  Serial.print("Connected, IP Address: ");  // print ip address
  Serial.println(WiFi.localIP());
  delay(100);
  
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler  
  client.onMessage(messageHandler);

  Serial.println("Connecting to AWS IOT");

  // while not connected to aws
  while (!client.connect(THINGNAME)) {
    if(WiFi.status()== WL_CONNECTED) Serial.println("Connected to WiFi, Connecting to AWS");
    delay(500);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

char* time(unsigned long ms)
{
  char time[8];  
  sprintf(time, "%2d:%2d:%2d", hours(ms), minutes(ms), seconds(ms));
  return time;
}

char* date(unsigned long ms)
{
  char date[10];
  sprintf(date, "2021/%2d/%2d", month(ms), day(ms));
}

void publishMessage()
{
  int one, twopt;
  StaticJsonDocument<200> doc;
  doc["Date"] = date(millis());
  doc["Time"] = time(millis());
  doc["Serial Number"] = 1;
  doc["Battery"] = rand()%10000/100.0;
  doc["WiFi Strength"] = rand()%10000/100.0;
  doc["Fix"] = rand()%2;
  doc["Latitude"] = rand()%18000/100.0;
  doc["Longitude"] = rand()%36000/100.0;
  doc["0.3um PC"] = rand()%10000;
  doc["0.5um PC"] = rand()%1000;
  doc["1.0um PC"] = rand()%500;
  doc["2.5um PC"] = rand()%300;
  doc["5um PC"] = rand()%100;
  doc["10um PC"] = rand()%50;
  one = rand()%100;
  doc["Env PM<1.0"] = one;
  twopt = rand()%50 + one;
  doc["Env PM<2.5"] = twopt;
  doc["Env PM<10"] = rand()%30 + twopt;
  doc["Temperature"] = rand()%15 + 20;
  doc["Relative Humidity"] = rand()%10000/100.0;
  doc["Total VoC"] = rand()%250 + 50;
  doc["equiv CO2"] = rand()%300 + 200;
  
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

// print incoming messages to serial monitor
void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

//  StaticJsonDocument<200> doc;
//  deserializeJson(doc, payload);
//  const char* message = doc["message"];
}

void setup() {
  Serial.begin(9600);
  connectAWS();
}

void loop() {
  if(client.connected()) {
      publishMessage(); // send a test message
      Serial.println("Test message Sent");
  }
  else { // if disconnected from aws, see if wifi is still connected
    Serial.println("Client Disconnected");
    if(WiFi.status()== WL_CONNECTED) Serial.println("Still connected to Wifi");
    else Serial.println("Also Disconnected From Wifi");
  }
  client.loop();
  delay(5000);
}
