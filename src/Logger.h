#ifndef LOGGER_H
#define LOGGER_H

#include <cstring>
#include <fstream>

class Logger
{
public:
  enum Level
  {
    TRACE = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4
  };

  Logger(const char* filename, Level minLogLevel);

  //! Asynchronously write a message if level >= minLogLevel
  void log(int level, const char* begin, const char* end);
  void log(int level, const char* message)
  {
    log(level, message, message + std::strlen(message));
  }

  //! Synchronously flush writing queue
  void flush();

private:
  Level _minLogLevel;
  std::ofstream _ofstream;
};

#endif /* LOGGER_H */
