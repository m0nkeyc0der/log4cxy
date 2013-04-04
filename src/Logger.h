#ifndef LOGGER_H
#define LOGGER_H

#include <cstring>
#include <fstream>
#include <string>
#include <mutex>

#include <boost/format.hpp>

class Logger
{
public:
  enum Level
  {
    TRACE = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4, MAX_LEVEL = 4
  };

  Logger(const char* filename, Level minLogLevel);

  //! Asynchronously write a message if level >= minLogLevel
  void log(int level, const char* begin, const char* end);

  void log(int level, const char* message)
  {
    log(level, message, message + std::strlen(message));
  }
  void log(int level, const std::string& message)
  {
    log(level, message.c_str(), message.c_str() + message.size());
  }

  // boost::format syntax
  template<typename... Args>
  void log(int level, const char* format, const Args&... args)
  {
    if (level >= _minLogLevel && _ofstream)
    {
      boost::format f(format);
      bind(f, args...);
      log(level, boost::str(f));
    }
  }

  //! Synchronously flush writing queue
  void flush();

private:
  Level _minLogLevel;

  std::ofstream _ofstream;
  std::mutex _ofstreamMutex;

  template<typename T, typename... Args>
  static void bind(boost::format& f, const T& arg1, const Args&... args)
  {
    bind(f % arg1, args...);
  }

  static void bind(boost::format& f) {} // = terminal specialization

  static std::string formatLogLine(int level, const char* begin, const char* end);
};

#endif /* LOGGER_H */
