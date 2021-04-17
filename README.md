# getrestpubmq
Small example of get rest and publish mmqt in loop

__TASK__

Write an application in C++, which makes REST calls to OpenWeatherMap online service
in order to read current weather conditions in Warsaw.
The application should do it periodically (every 60 seconds) and every time it
gets the fresh data, it should parse received JSON message and publish the
current temperature in degrees of Celsius to local MQTT bus.
Address of the service (API details for reference):
[https://openweathermap.org/api/one-call-api]
The output MQTT message should be published to the following MQTT topic:
temperature_warsaw
and should contain the following payload:
{
"id": "temperature",
"value": <value of the temperature (FLOAT)>,
"timestamp": <current unix timestamp (INTEGER)>
}
Note, that in order to use the API of the service one needs to create
an account in the service and get API key. Creation of the account is free of charge.
