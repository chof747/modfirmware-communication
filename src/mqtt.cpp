#include "mqtt.h"
#include <wifi.h>
#include <PubSubClient.h>

using namespace ModFirmWare;

PubSubClient mqttClient();

bool Mqtt::setup(Application* app)
{
  return true;
}

void Mqtt::loop()
{

}