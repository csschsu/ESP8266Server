Code for a ESP8266 processor with builtin WIFI and support for one ds18b20 temperature sensor in a local network with a router and a prometheus instance running on a raspberry pi 3.

Read requests are issued from the prometheus instance to a web server running in the esp8266 using the standard "pull" request for monitoring. 