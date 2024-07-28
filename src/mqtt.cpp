#include "mqtt.h"
#include <wifi.h>
#include "split.hpp"
#include "modfirmware.h"

using namespace ModFirmWare;

#define LOGTAG "mqtt"
#define DELIMITER "|#|"

#ifndef MQTT_CLIENT_ID_PREFIX
#define MQTT_CLIENT_ID_PREFIX "mqcid"
#endif

#ifndef MQTT_RECONNECT_WAIT_MS
#define MQTT_RECONNECT_WAIT_MS 5000
#endif

#ifndef MQTT_CONNECT_MESSAGE
#define MQTT_CONNECT_MESSAGE "connected"
#endif

#ifndef MQTT_TOPIC_SEPERATOR
#define MQTT_TOPIC_SEPERATOR "/"
#endif

const char *_strTopicRegime(const Mqtt::TopicRegime tp)
{
  switch (tp)
  {
  case Mqtt::TopicRegime::DEVICETYPE:
    return "device/type/topic";
  case Mqtt::TopicRegime::TYPEDEVICE:
    return "type/device/topic";
  case Mqtt::TopicRegime::TOPICONLY:
    return "topic";
  default:
    return "";
  }
}

Mqtt::Mqtt() : Component(), ConfigComponent(),
               mqttClient(), topicRegime(TopicRegime::TYPEDEVICE), client(NULL), app(NULL),
               lastUnsuccessfulReconnect(0),
               customParamMqttServer("mqtt4server", "MQTT Server", mqttServer, 40),
               customParamMqttPort("mqtt4port", "MQTT Port", mqttPort, 6),
               customParamMqttUser("mqtt4user", "MQTT User", mqttUser, 32),
               customParamMqttPassWd("mqtt4password", "MQTT Password", mqttPassWd, 32),
               subscribers()
//****************************************************************************************
{
  strcpy(mqttServer, "");
  strcpy(mqttPort, "1883");
  strcpy(mqttUser, "");
  strcpy(mqttPassWd, "");
}

bool Mqtt::setup(Application *app)
//****************************************************************************************
{
  mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length)
                         { callback(topic, payload, length); });
  this->app = app;
  return true;
}

void Mqtt::loop()
//****************************************************************************************
{
  if (!mqttClient.connected())
  {
    reconnect();
  }
  else
  {
    mqttClient.loop();
  }
}

void Mqtt::setTopicRegime(const TopicRegime topicRegime)
//****************************************************************************************
{
  this->topicRegime = topicRegime;
  logger->debug(LOGTAG, "Changed regime to create topics to %s",
                _strTopicRegime(this->topicRegime));
}

void Mqtt::addParameters(WiFiManager *wifiManager)
//****************************************************************************************
{
  wifiManager->addParameter(&customParamMqttServer);
  wifiManager->addParameter(&customParamMqttPort);
  wifiManager->addParameter(&customParamMqttUser);
  wifiManager->addParameter(&customParamMqttPassWd);

  return;
}

void Mqtt::sendMessage(const char *msgType, const char *topic, const char *message)
//****************************************************************************************
{
  if ((NULL == topic) || (NULL == message) || (NULL == app))
  {
    return;
  }
  String composedTopic = composeTopic(msgType, topic);

  logger->debug(LOGTAG, "Sending topic='%s', message='%s' to server %s",
                composedTopic.c_str(), message, mqttServer);
  mqttClient.publish(composedTopic.c_str(), message);
}

void Mqtt::subscribe(const char *msgType, const char *topic, Callback callback)
//****************************************************************************************
{
  String topicix = composeTopic(msgType, topic);
  subscribers[topicix].push_back(callback);
  mqttClient.subscribe(topicix.c_str());
}

String Mqtt::composeTopic(const char *msgType, const char *topic)
//****************************************************************************************
{
  const char *device = (NULL != app)
                           ? app->getDeviceName()
                           : NULL;

  String composed = "";
  String seperator(MQTT_TOPIC_SEPERATOR);
  switch (topicRegime)
  {
  case TopicRegime::DEVICETYPE:
    composed += (NULL != device) ? device + seperator : "";
    composed += msgType + seperator;
    break;
  case TopicRegime::TYPEDEVICE:
    composed += msgType + seperator;
    composed += (NULL != device) ? device + seperator : "";
    break;
  }

  composed += topic;
  return composed;
}

void Mqtt::setConfiguration()
//****************************************************************************************
{
  logger->debug(LOGTAG, "MQTT Configuration: server = %s, user = %s, port = %s",
                mqttServer, mqttUser, mqttPort);

  mqttClient.setServer(mqttServer, atoi(mqttPort));
}

void Mqtt::reconnect()
//****************************************************************************************
{
  if ((mqttClient.connected()) || ((millis() - lastUnsuccessfulReconnect) < MQTT_RECONNECT_WAIT_MS))
  {
    // only proceed if it is not connected and enough time has passed since the last
    // unsuccessful reconnect
    return;
  }

  String clientId = MQTT_CLIENT_ID_PREFIX;
  clientId += '-';
  clientId += String(random(0xffff), HEX);

  logger->debug(LOGTAG, "Trying to connect to MQTT server %s with client ID = %s",
                mqttServer, clientId.c_str());
  bool ok = mqttClient.connect(clientId.c_str(), mqttUser, mqttPassWd);
  // bool ok = false;

  if (ok)
  {
    logger->info(LOGTAG, "Successfully connected to MQTT server %s. Client ID = %s",
                 mqttServer, clientId.c_str());
    subscribeTopics();

    String connectMessage = MQTT_CONNECT_MESSAGE;
    connectMessage += " client id = " + clientId;
    sendMessage(MQTT_STATE_TOPIC, "mqtt", connectMessage.c_str());
  }
  else
  {
    logger->error(LOGTAG, "Could not connect to MQTT server %s, rc = %d",
                  mqttServer, mqttClient.state());
    lastUnsuccessfulReconnect = millis();
  }
}

void Mqtt::subscribeTopics()
//****************************************************************************************
{
  for (subscribers_t::iterator it = subscribers.begin(); it != subscribers.end(); ++it) 
  {
      mqttClient.subscribe(it->first.c_str()); 
  }
}

void Mqtt::callback(char *topic, byte *payload, unsigned int length)
//****************************************************************************************
{
  String topicix = topic;
  char *message = new char[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  if (subscribers.find(topicix) != subscribers.end())
  {
    for (const auto &callback : subscribers[topic])
    {
      callback(topic, message);
    }
  }

  delete[] message;
}

size_t Mqtt::readParameters(String configLine)
//****************************************************************************************
{
  String server;
  String user;
  String passwd;
  String port;

  splitString(configLine, DELIMITER, server, port, user, passwd);

  strcpy(mqttServer, server.c_str());
  strcpy(mqttUser, user.c_str());
  strcpy(mqttPassWd, passwd.c_str());

  if (0 != port.toInt())
  {
    strcpy(mqttPort, port.c_str());
    setConfiguration();
    return 4;
  }
  else
  {
    logger->warn(LOGTAG, "Invalid port %s specified in config, mqtt-port remains %s!", port.c_str(), mqttPort);
    return 3;
  }
}
String Mqtt::writeParameters(WiFiManager *wifiManager)
//****************************************************************************************

{
  strcpy(mqttServer, customParamMqttServer.getValue());
  String port(customParamMqttPort.getValue());
  strcpy(mqttUser, customParamMqttUser.getValue());
  strcpy(mqttPassWd, customParamMqttPassWd.getValue());

  if (0 != port.toInt())
  {
    strcpy(mqttPort, port.c_str());
  }
  else
  {
    logger->warn(LOGTAG, "Invalid port %s specified in config, mqtt-port remains %s!", port.c_str(), mqttPort);
  }

  return String(mqttServer) + DELIMITER +
         mqttPort + DELIMITER +
         mqttUser + DELIMITER +
         mqttPassWd;
}