#include "common.h"
#include <fstream>
#include <iostream>

namespace mckeys {

static Config * instance = NULL;

using namespace std;

// static
Config * Config::getInstance()
{
  if (instance == NULL)
    instance = new Config();

  return instance;
}

Config::~Config()
{
  logger->trace(CONTEXT, "Deleting logger");
  delete logger;
  instance = NULL;
}

/**
 * Set the discard threshold for stats. When displaying cache keys, only keys
 * being requested at or above this rate will be displayed.
 */
void Config::setDiscardThreshold(const double threshold)
{
  if (threshold < 0.0) {
    throw range_error("threshold must be >= 0");
  }
  logger->debug(CONTEXT, "Setting discard threshold to %.02f", threshold);
  discardThreshold = threshold;
}
double Config::getDiscardThreshold() const
{
  return discardThreshold;
}

void Config::setInterface(const string &value)
{
  logger->debug(CONTEXT, "Setting interface to %s", value.c_str());
  interface = value;
}
string Config::getInterface() const
{
  return interface;
}

void Config::setLogfile(const string &value) {
  ofstream* ofs = new ofstream(value.c_str(), std::ofstream::out);
  if (ofs->fail()) {
    delete ofs;
    throw MemkeysConfigurationError("Can't open " + value + " for writing");
  }
  logfile = value;
  logger->setHandler(ofs);
  logger->info(CONTEXT, "Now logging to %s", value.c_str());
}
string Config::getLogfile() const
{
  return logfile;
}

/**
 * Set the port to use for listening.
 */
void Config::setPort(const int _port)
{
  REQUIRE_UINT("port", _port, uint16_t);
  logger->debug(CONTEXT, "Setting port to %d", _port);
  port = (uint16_t)_port;
}
uint16_t Config::getPort() const
{
  return port;
}
string Config::getPortAsString() const
{
  return to_string((llui_t)getPort());
}

/**
 * Set the rate (in MS) at which the UI is refreshed.
 */
void Config::setRefreshInterval(const int interval)
{
  REQUIRE_UINT("refreshInterval", interval, uint16_t);
  refreshInterval = (uint16_t)interval;
}
uint16_t Config::getRefreshInterval() const
{
  return refreshInterval;
}

/**
 * Which report should be used by default?
 */
void Config::setReportType(const std::string &value) {
  reportType = ReportType::fromString(value);
}

ReportType Config::getReportType() const {
  return reportType;
}

/**
 * get or set
 */
void Config::setCaptureType(const std::string &type) {
    captureType = CaptureType::fromString(type);
}

CaptureType Config::getCaptureType() const {
  return captureType;
}

/**
 * Manage how verbose we get.
 */
void Config::makeLessVerbose()
{
  Level level = logger->getLevel();
  if (level >= Level::FATAL) {
    logger->warning(CONTEXT, "Log level already at or above FATAL");
    return;
  }
  adjustLoggerLevel(Level::fromValue(level.getValue() + 1));
}
void Config::increaseVerbosity()
{
  Level level = logger->getLevel();
  if (level <= Level::TRACE) {
    logger->warning(CONTEXT, "Log level already at or below TRACE");
    return;
  }
  adjustLoggerLevel(Level::fromValue(level.getValue() - 1));
}
Level Config::verbosity() const
{
  return logger->getLevel();
}

string Config::toString() const
{
  ostringstream configs;
  string lfile;
  if (logfile.empty()) {
    lfile = "stdout";
  } else {
    lfile = logfile;
  }
  configs << setw(20) << "Discard Threshold";
  configs << ": " << getDiscardThreshold() << endl;
  configs << setw(20) << "Interface";
  configs << ": " << getInterface() << endl;
  configs << setw(20) << "Port";
  configs << ": " << getPort() << endl;
  configs << setw(20) << "Refresh Interval";
  configs << ": " << getRefreshInterval() << "ms" << endl;

  configs << setw(20) << "Capture Type";
  configs << ": " << getCaptureType().getName() << endl;
  configs << setw(20) << "Verbosity";
  configs << ": " << verbosity().getName() << endl;
  configs << setw(20) << "Logfile";
  configs << ": " << lfile;
  return configs.str();
}

// private constructor
Config::Config()
  : discardThreshold(0.0),
    interface(""),
    _isPromiscuous(true),
    port(11211),
    _readTimeout(1000),
    refreshInterval(500),
    _snapLength(1518),
    logger(Logger::getLogger("config")),
    logfile(),
    reportType(ReportType::NCURSES),
    captureType(CaptureType::GET)
{}

void Config::adjustLoggerLevel(const Level &newLevel)
{
  logger->setLevel(newLevel);
  Logger::getRootLogger()->setLevel(newLevel);
}

} // end namespace
