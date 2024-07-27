#ifndef MQTT_H
#define MQTT_H

#include <modfw_component.h>
#include "wificonfig.h"
#include "WiFiManager.h"
#include <PubSubClient.h>

class Client;
class WiFiClass;

namespace ModFirmWare
{

  class Application;

  class Mqtt : public Component, public ConfigComponent
  {
  public:
    Mqtt();
    enum TopicRegime {
      TYPEDEVICE,
      DEVICETYPE,
      TOPICONLY
    };

    bool setup(Application *);
    void loop();

    void setClient(Client* client) { this->client = client; mqttClient.setClient(*client); }
    void setTopicRegime(const TopicRegime topicRegime); 
    const TopicRegime getTopicRegime() const { return topicRegime; }

    void addParameters(WiFiManager *wifiManager);
    
    void sendMessage(const char* msgType, const char* topic, const char* message);

  protected:
    PubSubClient mqttClient;

    TopicRegime topicRegime;

    const char* getFileName() const { return "/mqtt.conf"; }
    String composeTopic(const char* msgType, const char* topic);

    void setConfiguration();
    void reconnect();

    void subscribeTopics();
    void callback(char* topic, byte* payload, unsigned int length);

    size_t readParameters(String configLine);
    String writeParameters(WiFiManager* wifiManager);

  private:

    Client* client;
    Application* app;

    WiFiManagerParameter customParamMqttServer;
    WiFiManagerParameter customParamMqttPort;
    WiFiManagerParameter customParamMqttUser;
    WiFiManagerParameter customParamMqttPassWd;


    char mqttServer[40];
    char mqttUser[32];
    char mqttPassWd[32];
    char mqttPort[6];

    unsigned long lastUnsuccessfulReconnect;

  };
};

#endif // MQTT_H