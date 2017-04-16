# esp_uMQTT_broker
A basic MQTT Broker on the ESP8266

This is a first proof of concept implementation of a basic MQTT Broker on the ESP8266. The broker does support:
- a smaller number of clients (at least 5 have been tested) based on the config of lwip
- up to 100 topic subscriptions
- arbitrary publications
- QoS level 0

The broker does not yet support:
- username, password authentication
- TLS
- QoS levels other than 0

The complete functionality is included in the mqtt directory. The broker is started by simply calling:

mqtt_server_start(1883)

in the user_init() function. The code can be be used in any project that is compiled using the esp-open-sdk (https://github.com/pfalcon/esp-open-sdk). Thanks to Tuan PM for sharing his MQTT client library https://github.com/tuanpmt/esp_mqtt as a basis with us.

The current demo programm is based on and included in the esp_wifi_repeater NAT router (without monitoring, acls, etc..), but it uses the router only as implementation environment. Binary firmware can be found in the firmware directory. For infos on flashing, building and using the provided demo programm look into the master branch of this project (https://github.com/martin-ger/esp_wifi_repeater/blob/master/README.md).

Current status of the uMQTT broker can be seen by entering:

- show mqtt_broker

at the console (serial or telnet to port 7777).


