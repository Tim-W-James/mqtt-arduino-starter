#include <ArduinoMqttClient.h>
#include "WiFi.h"
#include "arduino_secrets.h"

// ! Define env vars in arduino_secrets.h
const char wifi_ssid[] = WIFI_SSID; //  your network SSID (name)
const char wifi_pass[] = WIFI_PASS; // your network password

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char mqtt_broker[] = MQTT_BROKER;
const int mqtt_port = MQTT_PORT;
const char mqtt_username[] = MQTT_USERNAME;
const char mqtt_password[] = MQTT_PASSWORD;

const char sensor_topic[] = SENSOR_TOPIC;

const long interval = 8000;
unsigned long previousMillis = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  initWiFi(wifi_ssid, wifi_pass);

  initMqtt(mqtt_broker, mqtt_port, mqtt_username, mqtt_password);
}

void loop()
{
  // Call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    // Save the last time a message was sent
    previousMillis = currentMillis;

    sendMqttMessage(sensor_topic, "Hello World!");
  }
}

void initWiFi(const char ssid[], const char password[])
{
  WiFi.mode(WIFI_STA);
  WiFi.enableIpV6();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi with SSID [");
  Serial.print(ssid);
  Serial.print("]...");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println();
  Serial.print("Connected to the network with IP: [");
  Serial.print(WiFi.localIP());
  Serial.println("].");
  Serial.println();
}

void initMqtt(const char broker[], const int port, const char username[], const char password[])
{
  Serial.print("Attempting to connect to the MQTT broker: [");
  Serial.print(broker);
  Serial.print(":");
  Serial.print(port);
  Serial.println("]...");

  mqttClient.setUsernamePassword(username, password);
  if (!mqttClient.connect(broker, port))
  {
    Serial.print("MQTT connection failed. Error code = [");
    Serial.print(mqttClient.connectError());
    Serial.println("].");

    while (1)
      ;
  }

  Serial.println("Connected to the MQTT broker.");
  Serial.println();
}

void sendMqttMessage(const char topic[], const char message[])
{
  Serial.print("Sending message to topic: [");
  Serial.print(topic);
  Serial.println("]...");

  mqttClient.beginMessage(topic);
  mqttClient.print(message);
  mqttClient.endMessage();

  Serial.print("Message sent.");
  Serial.println();
}