# esp_uMQTT_broker
A basic MQTT Broker on the ESP8266

The broker does support:
- a subset of MQTT v3.1.1 (CONNECT, DISCONNECT, SUBSCRIBE, UNSUBSCRIBE, PUBLISH, PING)
- a smaller number of clients (at least 5 have been tested) based on the config of lwip
- retained messages
- LWT
- QoS level 0

The broker does not yet support:
- username, password authentication
- QoS levels other than 0
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


