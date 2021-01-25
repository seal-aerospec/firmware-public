
#include "awsCredentials.h"
#include "wifiCredential.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

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

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["sensor_a0"] = analogRead(0);
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
