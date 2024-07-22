#ifndef MQTT_H
#define MQTT_H

#include <modfw_component.h>

namespace ModFirmWare
{
    class Mqtt : public Component
    {
      bool setup(Application*);
      void loop();
    };
};


#endif //MQTT_H