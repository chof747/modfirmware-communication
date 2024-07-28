#ifndef MQTT_H
#define MQTT_H

#include <modfw_component.h>
#include "wificonfig.h"
#include "WiFiManager.h"
#include <PubSubClient.h>
#include <vector>
#include <map>

#ifndef MQTT_STATE_TOPIC
#define MQTT_STATE_TOPIC "state"
#endif

#ifndef MQTT_TELE_TOPIC
#define MQTT_TELE_TOPIC "tele"
#endif

#ifndef MQTT_COMMAND_TOPIC
#define MQTT_COMMAND_TOPIC "cmnd"
#endif


class Client;
class WiFiClass;

namespace ModFirmWare
{

  class Application;

  class Mqtt : public Component, public ConfigComponent
  {
  public:
    enum TopicRegime {
      TYPEDEVICE,
      DEVICETYPE,
      TOPICONLY
    };
    using Callback = std::function<void(const char* topic, const char* payload)>;

    Mqtt();

    bool setup(Application *);
    void loop();

    void setClient(Client* client) { this->client = client; mqttClient.setClient(*client); }
    void setTopicRegime(const TopicRegime topicRegime); 
    const TopicRegime getTopicRegime() const { return topicRegime; }

    void addParameters(WiFiManager *wifiManager);
    
    void sendMessage(const char* msgType, const char* topic, const char* message);
    void subscribe(const char *msgType, const char* topic, Callback callback);


  protected:
    typedef std::map<String, std::vector<Callback>> subscribers_t;

    PubSubClient mqttClient;
    TopicRegime topicRegime;
    subscribers_t subscribers;

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