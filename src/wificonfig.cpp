#include "wificonfig.h"
#include "logengine.h"
#include <SPIFFS.h>

using namespace ModFirmWare;

#define LOGTAG "CONFIG"

ConfigComponent::ConfigComponent() : fsReady(false), clogger(LogEngine::getInstance())
//****************************************************************************************
{
  initialize();
}

size_t ConfigComponent::readConfig()
//****************************************************************************************
{
  if (!fsReady)
    return 0;

  const char *filename = getFileName();

  if (!SPIFFS.exists(filename)) {
    clogger->info(LOGTAG, "Config file %s does not exist yet. Starting with blank config", filename);
    return 0;
  }

  clogger->debug(LOGTAG, "Reading config file %s", filename);
  File configFile = SPIFFS.open(filename, "r");

  if (!configFile)
  {
    clogger->error(LOGTAG, "Could not open file %s - config not read!", filename);
    return 0;
  }

  size_t parametersRead = readParameters(configFile.readString());
  configFile.close();

  return parametersRead;
}

void ConfigComponent::saveConfig(WiFiManager *wifiManager)
//****************************************************************************************
{
  if (!fsReady)
    return;

  const char *filename = getFileName();
  String configParameters = writeParameters(wifiManager);

    File configFile = SPIFFS.open(filename, "w");

  if (!configFile)
  {
    clogger->error(LOGTAG, "Could not open file %s - config not written!", filename);
    return;
  }

  configFile.print(configParameters);
  configFile.close();
}

bool ConfigComponent::initialize()
//****************************************************************************************
{
  if (!SPIFFS.begin(false))
  {
    clogger->error(LOGTAG, "Could not open SPIFFS file system - configuration not working!");
    fsReady = false;
  }
  else
  {
    fsReady = true;
  }

  return fsReady;
}