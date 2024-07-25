#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <Arduino.h>

class WiFiManager;

namespace ModFirmWare
{
  class LogEngine;
  class ConfigComponent
  {
  public:
    ConfigComponent();
    virtual size_t readConfig();
    virtual void saveConfig(WiFiManager *wifiManager);
    virtual void addParameters(WiFiManager *wifiManager) = 0;

  protected:
    
    virtual const char* getFileName() const = 0;
    virtual size_t readParameters(String configLine) = 0;
    virtual String writeParameters(WiFiManager* wifiManager) = 0;

    bool initialize();

  private:
    
    LogEngine* clogger;
    bool fsReady;
  };

};

#endif // WIFI_CONFIG_H