#include "mqtt.h"
#include <wifi.h>
#include <PubSubClient.h>
#include "split.hpp"

using namespace ModFirmWare;

PubSubClient mqttClient();

#define LOGTAG "mqtt"
#define DELIMITER "|#|"

Mqtt::Mqtt() : Component(), ConfigComponent(),
  custom_mqtt_server("mqtt4server", "MQTT Server", mqtt_server, 40),
  custom_mqtt_port("mqtt4port", "MQTT Port", mqtt_port, 6),
  custom_mqtt_user("mqtt4user", "MQTT User", mqtt_user, 32),
  custom_mqtt_password("mqtt4password", "MQTT Password", mqtt_passwd, 32)
//****************************************************************************************
{
  strcpy(mqtt_server, "");
  strcpy(mqtt_port, "1883");
  strcpy(mqtt_user, "");
  strcpy(mqtt_passwd, "");
}

bool Mqtt::setup(Application *app)
//****************************************************************************************
{
  return true;
}

void Mqtt::loop()
//****************************************************************************************
{
}

void Mqtt::addParameters(WiFiManager *wifiManager)
//****************************************************************************************
{
    wifiManager->addParameter(&custom_mqtt_server);
    wifiManager->addParameter(&custom_mqtt_port);
    wifiManager->addParameter(&custom_mqtt_user);
    wifiManager->addParameter(&custom_mqtt_password);

    return;
}

size_t Mqtt::readParameters(String configLine)
//****************************************************************************************
{
  String server;
  String user;
  String passwd;
  String port; 

  splitString(configLine, DELIMITER, server, port, user, passwd);

  strcpy(mqtt_server, server.c_str());
  strcpy(mqtt_user, user.c_str());
  strcpy(mqtt_passwd, passwd.c_str());

  if ( 0 != port.toInt())
  {
    strcpy(mqtt_port, port.c_str());
    logger->debug(LOGTAG,"MQTT Configuration: server = %s, user = %s, port = %s", mqtt_server, mqtt_user, mqtt_port);
    return 4;
  }
  else
  {
    logger->warn(LOGTAG, "Invalid port %s specified in config, mqtt-port remains %s!", port.c_str(), mqtt_port);
    return 3;
  }

}
String Mqtt::writeParameters(WiFiManager* wifiManager)
//****************************************************************************************

{
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  String port(custom_mqtt_port.getValue());
  strcpy(mqtt_user, custom_mqtt_user.getValue());
  strcpy(mqtt_passwd, custom_mqtt_password.getValue());

  if (0 != port.toInt())
  {
    strcpy(mqtt_port, port.c_str());
  }
  else
  {
    logger->warn(LOGTAG, "Invalid port %s specified in config, mqtt-port remains %s!", port.c_str(), mqtt_port);
  }


  return String(mqtt_server) + DELIMITER +
         mqtt_port + DELIMITER +
         mqtt_user + DELIMITER +
         mqtt_passwd;
}