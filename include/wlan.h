#ifndef WLAN_H
#define WLAN_H

#include <modfw_component.h>
#include "wificonfig.h"
#include <vector>

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#define WIFICLIENT WifiClient
#endif


// according to IEEE Wifi Standard
#define MAX_SSID_LENGTH 35
#define MAX_PASSWD_LENGTH 64

/* TODO

Implement a simple captive web portal to enter the SSID and password
this will be different from the rest of the configuration as other things can be entered in different modes
Therefore the WLAN module will go automatically into this mode and the rest of the configurations
can then be done in different modes (e.g. via MQTT messages or via other web portals)

*/

namespace ModFirmWare
{
    typedef std::vector<ConfigComponent *> configComponentsVector_t;

    class WLan : public Component
    {
    public:
        WLan(const char *configSSID, const char *configPassWd);
        WLan();

        void resetWifiSettings();

        size_t addConfigurator(ConfigComponent* configurator);
        Client* getClient() { return &client; };

        virtual bool setup(Application *);
        virtual void loop();

    protected:
        char ssid[MAX_SSID_LENGTH];

    private:

        configComponentsVector_t configurators;
        WiFiClient client;

        const char *configSSID;
        const char *configPassWd;

        void initializeAndAddParametersFromConfigurators();
        void notifySaveConfig();
    };
};

#endif // WLAN_H