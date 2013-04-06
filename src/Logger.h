#ifndef LOGGER_H
#define LOGGER_H

#include <cstring>
#include <exception>
#include <memory>
#include <boost/format.hpp>

#include "LogLevel.h"
#include "LogWriter.h"

namespace log4cxy
{

class Logger
{
public:
  /*!
   * Asynchronously write a message to log if level >= minLogLevel.
   * Forward the message to all chain loggers (if any).
   */
  void log(int level, const char* begin, const char* end) throw()
  {
    logImpl(level, begin, end);
  }

  void log(int level, const char* message) throw()
  {
    logImpl(level, message, message + std::strlen(message));
  }
  void log(int level, const std::string& message) throw()
  {
    logImpl(level, message.c_str(), message.c_str() + message.size());
  }

  // boost::format syntax
  template<typename... Args>
  void log(int level, const char* format, const Args&... args) throw()
  {
    bool isFatal = true;
    try
    {
      if (level >= _minLogLevel && _writerImpl->isValid())
      {
        isFatal = false;
        boost::format f(format);
        bind(f, args...);
        const std::string& s = boost::str(f);

        isFatal = true;
        log(level, s);
      }
      else if(_chainLogger)
      {
        _chainLogger->log(level, format, args...);
      }
    }
    catch(const std::exception& err)
    {
      if (isFatal)
        std::cerr << "Logger fatal error: " << err.what() << std::endl;
      else
        log(ERROR, err.what());
    }
  }

  /*******************************/

  //! Synchronously flush writing queue
  void flush() throw();

  static Logger create(const char* filename, LogLevel minLogLevel);
  static Logger create(std::ostream& os, LogLevel minLogLevel);

  Logger& addChainLogger(Logger chainLogger);

  /*!
   * Search for LogWriter implementation within this logger.
   *
   * Does not propagate the search to chained loggers.
   *
   * @return empty shared_ptr if specific implementation was not found
   */
  template<class T>
  std::shared_ptr<T> getWriterImpl()
  {
    LogWriterPtr desiredWriterImpl;
    LogWriterPtr ourWriter = std::const_pointer_cast<LogWriter>(_writerImpl);

    LogWriter::FindImplPred pr = [](LogWriter* w){ return (dynamic_cast<T*>(w) != 0); };
    if (pr(ourWriter.get()))
      desiredWriterImpl = ourWriter;
    else
      desiredWriterImpl = ourWriter->getImpl(pr);

    return std::dynamic_pointer_cast<T>(desiredWriterImpl);
  }

private:
  LogLevel _minLogLevel;
  LogWriterThreadSafePtr _writerImpl;
  std::shared_ptr<Logger> _chainLogger;

  Logger(LogLevel minLogLevel, const LogWriterThreadSafePtr& writerImpl)
    : _minLogLevel(minLogLevel), _writerImpl(writerImpl)
  {}

  void logImpl(int level, const char* begin, const char* end) throw();

  template<typename T, typename... Args>
  static void bind(boost::format& f, const T& arg1, const Args&... args)
  {
    bind(f % arg1, args...);
  }

  static void bind(boost::format& f) {} // = terminal specialization

  static std::string formatLogLine(int level, const char* begin, const char* end);
};

} // ns log4cxy

#endif /* LOGGER_H */
