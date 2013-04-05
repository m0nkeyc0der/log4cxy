#include "Logger.h"

// stdlib headers
#include <ctime>
#include <iomanip>
#include <sstream>

// sys headers
#include <pthread.h>
#include <sys/time.h>

// project headers
#include "FileLogWriter.h"
#include "SyncLogWriter.h"

namespace log4cxy
{

Logger Logger::create(const char* filename, LogLevel minLogLevel)
{
  LogWriterPtr fileWriter(new FileLogWriter(filename));
  LogWriterThreadSafePtr syncWriter(new SyncLogWriter(fileWriter));

  return Logger(minLogLevel, syncWriter);
}

void Logger::logImpl(int level, const char* begin, const char* end) throw()
{
  try
  {
    if (level < _minLogLevel || !_writerImpl->isValid())
      return;

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
