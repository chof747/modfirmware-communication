#include "wlan.h"

#include <modfirmware.h>
#include <wifi.h>
#include <WiFiManager.h>

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif


using namespace ModFirmWare;

#define LOGCOMPONENT "WIFI"
WiFiManager wifiMgr;

#define IS_VALID_KEY(KEY) ((WIFI_SSID == KEY) || (WIFI_PASSWD == KEY))

WLan::WLan(const char *configSSID, const char *configPassWd) : 
  Component(), configurators(), configSSID(configSSID), configPassWd(configPassWd),
  client()
//****************************************************************************************
{
}

WLan::WLan() : Component(), configurators(), client()
//****************************************************************************************
{
}

size_t WLan::addConfigurator(ConfigComponent *configurator)
//****************************************************************************************
{
  if (nullptr != configurator)
  {
    configurators.push_back(configurator);
  }

  return configurators.size();
}

void WLan::resetWifiSettings()
//****************************************************************************************
{
  wifiMgr.resetSettings();
}

bool WLan::setup(Application *app)
//****************************************************************************************
{
  if (Component::setup(app))
  {
    wifiMgr.setSaveConfigCallback([this]()
                                  { notifySaveConfig(); });

    initializeAndAddParametersFromConfigurators();
    //wifiMgr.resetSettings();

    logger->info(LOGCOMPONENT, "Starting Wifi!");
    if (wifiMgr.autoConnect(configSSID, configPassWd))
    {
      logger->info(LOGCOMPONENT, "Connected to WiFi %s with IP %s",
                   WiFi.SSID().c_str(),
                   WiFi.localIP().toString().c_str());
      return true;
    }
    else
    {
      logger->error(LOGCOMPONENT, "Failed to connect");
      return false;
    }
  }
  else
  {
    return false;
  }
}

void WLan::loop()
//****************************************************************************************
{
}

void WLan::initializeAndAddParametersFromConfigurators()
//****************************************************************************************
{
  for (configComponentsVector_t::iterator i = configurators.begin(); i != configurators.end(); ++i)
  {
    (*i)->readConfig();
    (*i)->addParameters(&wifiMgr);
  }
}

void WLan::notifySaveConfig()
//****************************************************************************************
{
  for (configComponentsVector_t::iterator i = configurators.begin(); i != configurators.end(); ++i)
  {
    (*i)->saveConfig(&wifiMgr);
  }
}