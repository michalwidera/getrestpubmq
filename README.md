# getrestpubmq
Small example of get rest and publish mmqt in loop

__TASK__

Write an application in C++, which makes REST calls to OpenWeatherMap online service in order to read current weather conditions in Warsaw.

The application should do it periodically (every 60 seconds) and every time it gets the fresh data, it should parse received JSON message and publish the current temperature in degrees of Celsius to local MQTT bus.

Address of the service (API details for reference):

[https://openweathermap.org/api/one-call-api]

The output MQTT message should be published to the following MQTT topic:

temperature_warsaw and should contain the following payload:

```
{
"id": "temperature",
"value": <value of the temperature (FLOAT)>,
"timestamp": <current unix timestamp (INTEGER)>
}
```

Note, that in order to use the API of the service one needs to create an account in the service and get API key. Creation of the account is free of charge.

__INSTALLATION__

After downloading there need to create api.key file in main directory.

**_NOTE:_** First, please create file api.key in main project directory and fill it with code from OpenWeatherMap account. Do not put enter at the end of line.

This tool requires few open source libraries.
I've chosen following two:

- [Paho mqtt cpp](https://github.com/eclipse/paho.mqtt.cpp) / [Paho mqtt c](https://github.com/eclipse/paho.mqtt.c)
- [Boost](https://www.boost.org/)

___Boost___

First, get boost and core build requirements from distribution repostiory (1.74 or higher).

```
$ sudo apt-get -y install gcc cmake libboost-all-dev make build-essential libcurl4-openssl-dev curl doxygen
```

___Paho MQTT___

Here is procedure of paho lib set installation
(simillar as here: https://github.com/eclipse/paho.mqtt.cpp)

Install Paho C library in system structure

```
$ git clone https://github.com/eclipse/paho.mqtt.c.git
$ cd paho.mqtt.c
$ cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON -DPAHO_WITH_SSL=OFF -DPAHO_HIGH_PERFORMANCE=ON
$ sudo cmake --build build/ --target install
$ sudo ldconfig
$ cd ..
```

Install Paho C++ library in system structure

```
$ git clone https://github.com/eclipse/paho.mqtt.cpp
$ cd paho.mqtt.cpp
$ cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE -DPAHO_WITH_SSL=OFF
$ sudo cmake --build build/ --target install
$ sudo ldconfig
$ cd ..
```

**_NOTE:_** Without Paho MQTT libraries following line of code will fail. On Ubuntu LTS 20.04 I need to set -DPAHO_WITH_SSL=OFF

```
#include "mqtt/async_client.h"
```

__BUILD & FIRST RUN__

First, step into project directory.
Then call cmake, make
And call executable - rtoy

```
$ cd getrestpubmq
$ cmake .
...
$ make
$ ./build/rtoy
```

__RUNING & TESTING__

__Unit test__

Project containt Unit Test coverage.

Issuing make should got:

```
$ make test
Running tests...
Test project /home/michal/getrestpubmq
    Start 1: ut-test_mgrapikey
1/4 Test #1: ut-test_mgrapikey ................   Passed    0.00 sec
    Start 2: ut-test_mgrpayload
2/4 Test #2: ut-test_mgrpayload ...............   Passed    0.00 sec
    Start 3: ut-test_mqttclient
3/4 Test #3: ut-test_mqttclient ...............   Passed    0.00 sec
    Start 4: ut-test_restclient
4/4 Test #4: ut-test_restclient ...............   Passed    0.00 sec

100% tests passed, 0 tests failed out of 4

Total Test time (real) =   0.02 sec
```

___Sandbox test___

I've been testing this code with mosquitto MQTT broker.
Please install for testing purposes:
```
$ sudo apt install mosquitto mosquitto-clients
```

Start in first terminal mosquitto broker.
And in second terminal subsribe topic.
```
Term 1> $ mosquitto
Term 2> $ mosquitto_sub -t temperature_warsaw
```

Create file in project directory with api key from open weather map.
This file name is api.key and need 32 hexadecimal signs.
No enter at the end no extra spaces or tabs.
Then if build process has been successful we can launch in project directory
```
$ ./build/rtoy
```

We can observe results on mosquitto_sub terminal.
Every 60 seconds temperature measuremt are presented on screen.
mosquitto presents connection and disconnection client actions.
```
$ mosquitto_sub -t temperature_warsaw
{
"id": "temperature",
"value": 6.2,
"timestamp": 1618706116
}
{
"id": "temperature",
"value": 6.19,
"timestamp": 1618706176
}
```

**_NOTE:_** Use Ctrl+C to stop rtoy, mosquitto and mosquitto_sub

**_NOTE:_** This solution has been tested following distributions: WSL2 and Ubuntu LTS 20.4