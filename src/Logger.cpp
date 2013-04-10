#include "Logger.h"

// stdlib headers
#include <ctime>
#include <iomanip>
#include <sstream>

// sys headers
#include <pthread.h>
#include <sys/time.h>

// project headers
#include "AsyncLogWriter.h"
#include "FileLogWriter.h"
#include "SyncLogWriter.h"

namespace log4cxy
{

void Logger::flush() throw()
{
  try
  {
    _writerImpl->flush();
  }
  catch (const std::exception& err)
  {
    formatLog(ERROR, "Logger flush error: %1%", err.what());
  }

  if (_chainLogger)
    _chainLogger->flush();
}

Logger Logger::create(const char* filename, LogLevel minLogLevel)
{
  LogWriterPtr fileWriter(new FileLogWriter(filename));
//  LogWriterThreadSafePtr syncWriter(new SyncLogWriter(fileWriter));
  LogWriterThreadSafePtr asyncWriter(new AsyncLogWriter(fileWriter), AsyncLogWriter::deleter);

  return Logger(minLogLevel, asyncWriter);
}

Logger Logger::create(std::ostream& os, LogLevel minLogLevel)
{
  LogWriterPtr streamWriter(new StreamWriter(os));
//  LogWriterThreadSafePtr syncWriter(new SyncLogWriter(streamWriter));
  LogWriterThreadSafePtr asyncWriter(new AsyncLogWriter(streamWriter), AsyncLogWriter::deleter);

  return Logger(minLogLevel, asyncWriter);
}

Logger& Logger::addChainLogger(Logger chainLogger)
{
  if (!_chainLogger)
    _chainLogger.reset(new Logger(chainLogger));
  else
    _chainLogger->addChainLogger(chainLogger);

  return (*this);
}

void Logger::logImpl(int level, const char* begin, const char* end) throw()
{
  if (level >= _minLogLevel && _writerImpl->isValid())
  {
    try
    {
      bool isBadRange = (begin > end);
      if (isBadRange)
      {
        log(ERROR, "Logger - bad range in logger arguments");
        return;
      }

      const std::string& line = formatLogLine(level, begin, end);
      _writerImpl->writeLine(line);
    }
    catch (const std::exception& err)
    {
      std::cerr << "Logger fatal error: " << err.what() << std::endl;
    }
  }

  if (_chainLogger)
  {
    _chainLogger->logImpl(level, begin, end);
  }
}

std::string Logger::formatLogLine(int level, const char* begin, const char* end)
{
  std::ostringstream os;

  // getting high resolution wall time
  struct timeval s_timeval;
  ::gettimeofday(&s_timeval, 0);

  // timestamp
  static const char format[] = "YYYYMMDD HH:MM:SS";
  char buf[sizeof(format)];
  std::strftime(buf, sizeof(buf), "%Y%m%d %H:%M:%S", std::localtime(&s_timeval.tv_sec));
  os << buf;

  // microseconds
  os << "." << std::setw(6) << std::setfill('0') << s_timeval.tv_usec;

  os << ":T" << ::pthread_self();

  os << ":L" << level;

  os << ": ";
  os.write(begin, end - begin);
  os << std::endl;

  return os.str();
}

}
