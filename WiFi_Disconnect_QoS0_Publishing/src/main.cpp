#include <Arduino.h>

#include "secrets/wifi.h"
#include "wifi_connect.h"
#include <WiFiClientSecure.h>
#include "ca_cert.h"

#include "secrets/mqtt.h"
#include <PubSubClient.h>

#include <Ticker.h>

namespace
{
    const char *ssid = "Thai Nguyen Viettel"; 
    const char *password = "votuyen21"; 
    const char *echo_topic = "esp32/echo_test";
    unsigned int publish_count = 0;
    uint16_t keepAlive = 15;    
    uint16_t socketTimeout = 5;
}

WiFiClientSecure tlsClient;
PubSubClient mqttClient(tlsClient);
Ticker mqttPublishTicker;

const char *mqttBroker = "a13dcfcaabef4c32acf611db7d43e897.s1.eu.hivemq.cloud";
const int mqttPort = 8883;

void mqttPublish()
{
    Serial.print("Publishing: ");
    Serial.println(publish_count);
    mqttClient.publish(echo_topic, String(publish_count).c_str(), false);
    publish_count++;
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.printf("From %s:  ", topic);
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void mqttReconnect()
{
    while (!mqttClient.connected())
    {
        Serial.println("Attempting MQTT connection...");
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        
        if (mqttClient.connect(client_id.c_str(), MQTT::username, MQTT::password)) 
        {
            Serial.print(client_id);
            Serial.println(" connected");
            mqttClient.subscribe(echo_topic);
        }
        else
        {
            Serial.print("MQTT connect failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 1 seconds");
            delay(1000);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    delay(10);

    // Kết nối Wi-Fi
    setup_wifi(ssid, password);

    // Tích hợp CA Certificate cho kết nối TLS
    tlsClient.setCACert(ca_cert);

    // Cấu hình MQTT
    mqttClient.setCallback(mqttCallback);
    mqttClient.setServer(mqttBroker, mqttPort); // Sử dụng URL và port của HiveMQ
    mqttPublishTicker.attach(1, mqttPublish); // Gửi dữ liệu mỗi giây
}

void loop()
{
    if (!mqttClient.connected())
    {
        mqttReconnect();
    }
    mqttClient.loop();
}
