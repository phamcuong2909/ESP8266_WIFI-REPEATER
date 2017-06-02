This development will be discontinued here. Now moved to: https://github.com/martin-ger/esp_mqtt

# esp_uMQTT_broker
A basic MQTT Broker on the ESP8266

The broker does support:
- a subset of MQTT (CONNECT, DISCONNECT, SUBSCRIBE, UNSUBSCRIBE, PUBLISH, PING)
- MQTT protocoll versions v3.1 and v3.1.1 simultaniously
- a smaller number of clients (at least 5 have been tested) based on the config of lwip
- retained messages
- LWT
- QoS level 0

The broker does not yet support:
- username, password authentication
- QoS levels other than 0
- more than 5 TCP(=MQTT) clients
- non-clear sessions
- TLS

The complete functionality is included in the mqtt directory. The broker is started by simply including:

#include "mqtt_server.h"

bool MQTT_server_start(uint16_t portno, uint16_t max_subscriptions, uint16_t max_retained_topics);

in the user_init() function. The code can be used in any project that is compiled using the esp-open-sdk (https://github.com/pfalcon/esp-open-sdk). Thanks to Tuan PM for sharing his MQTT client library https://github.com/tuanpmt/esp_mqtt as a basis with us.

The current demo program is based on and included in the uMQTT_broker branch of the esp_wifi_repeater NAT router (without monitoring, acls, etc..), but it uses the router only as implementation environment. Binary firmware can be found in the firmware directory. For infos on flashing, building and using the provided demo program look into the master branch of this project (https://github.com/martin-ger/esp_wifi_repeater/blob/master/README.md).

Current status of the uMQTT broker can be seen by entering:

- show mqtt_broker

at the console (serial or telnet to port 7777).

# LOCAL client
The broker comes with a "LOCAL client", which means, the broker itself can publish and subscribe topics (without the need of an additional TCP connection). You can test this with the commands:

- publish [topic] [data]: this publishes a topic
- subscribe [topic]: subscribes to a topic, received topic will be printed to serial output
- unsubscribe [topic]: unsubscribes from a topic

This feature is meant to provide the basis for a local rule engine that can react on MQTT events, e.g. to switch GPIOs or send other messages (MQTT, HTTP,...). You can use this with the functions:

bool MQTT_local_publish(uint8_t* topic, uint8_t* data, uint16_t data_length, uint8_t qos, uint8_t retain);

bool MQTT_local_subscribe(uint8_t* topic, uint8_t qos);

bool MQTT_local_unsubscribe(uint8_t* topic);

void MQTT_local_onData(MqttDataCallback dataCb);


