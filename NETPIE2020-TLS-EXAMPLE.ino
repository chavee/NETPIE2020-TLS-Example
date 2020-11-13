#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DEVICE_ID       "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#define DEVICE_TOKEN    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

#define WIFI_SSID       "xxxxxxxx"
#define WIFI_PASSWORD   "xxxxxxxx"
#define MQTT_HOST       "mqtt.netpie.io"
#define MQTT_PORT       8883

// cert fingerprintn of mqtt.netpie.io:8883 (webtrust)
const char fingerprint[] PROGMEM = "54 42 FE E9 FE 2F A3 62 54 9F 14 D6 E7 7A 42 83 4C C8 1D 50";

// cert fingerprintn of mqtt.netpie.io:1884 (long-life private CA)
//const char fingerprint[] PROGMEM = "49 DF 2F 5E 27 0B 35 5B 6C A9 80 69 7E 10 C0 D8 99 24 86 C5";

WiFiClientSecure client; 
PubSubClient mqttclient(client); 
 
void msgcallback(char* topic, byte* payload, unsigned int len) {
    Serial.print("Incoming message : ");
    Serial.print(topic);
    Serial.print(" --> ");
    for (unsigned int i=0; i<len; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}
  
void connect() {
    while (!mqttclient.connected()) {
        Serial.println("Trying to connect to NETPIE MQTT... ");
        if (mqttclient.connect(DEVICE_ID, DEVICE_TOKEN, "")) {
            Serial.println("Connected...");
            mqttclient.subscribe("@msg/#");
            mqttclient.publish("@msg/hello", "Hello NETPIE 2020");
        }
        else {
            Serial.print("Connection failed, rc=");
            Serial.println(mqttclient.state());
            delay(2000);
        }
    }
}
 
void setup() {
    Serial.begin(115200);  
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    client.setFingerprint(fingerprint);
      
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected to WiFi");
    Serial.println(WiFi.localIP());

    mqttclient.setServer(MQTT_HOST, MQTT_PORT);
    mqttclient.setCallback(msgcallback);
}
 
void loop() {
    if (!mqttclient.connected()) {
        connect();
    }
    mqttclient.loop();
}
