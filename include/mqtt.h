#ifndef MQTT_H
#define MQTT_H

#include <modfw_component.h>
#include "wificonfig.h"
#include "WiFiManager.h"

namespace ModFirmWare
{
  class Mqtt : public Component, public ConfigComponent
  {
  public:
    Mqtt();
    bool setup(Application *);
    void loop();

    void addParameters(WiFiManager *wifiManager);

  protected:
      const char* getFileName() const { return "/mqtt.conf"; }
    size_t readParameters(String configLine);
    String writeParameters(WiFiManager* wifiManager);

  private:

    WiFiManagerParameter custom_mqtt_server;
    WiFiManagerParameter custom_mqtt_port;
    WiFiManagerParameter custom_mqtt_user;
    WiFiManagerParameter custom_mqtt_password;


    char mqtt_server[40];
    char mqtt_user[32];
    char mqtt_passwd[32];
    char mqtt_port[6];

  };
};

#endif // MQTT_H