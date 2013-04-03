#include "Logger.h"

// stdlib headers
#include <ctime>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>

// sys headers
#include <pthread.h>
#include <sys/time.h>

Logger::Logger(const char* filename, Level minLogLevel)
  : _minLogLevel(minLogLevel)
  , _ofstream(filename)
{
  if (!_ofstream)
    std::cerr << "cannot open log file " << filename << std::endl;
}

void Logger::log(int level, const char* begin, const char* end)
{
  if (!_ofstream || level < _minLogLevel)
    return;

  bool isBadRange = (begin > end);
  if (isBadRange)
  {
    log(ERROR, "Logger - bad range in logger arguments");
    return;
  }

  if (_ofstream)
  {
    const std::string& line = formatLogLine(level, begin, end);

    std::lock_guard<std::mutex> lock(_ofstreamMutex);
    _ofstream << line;
  }
}

void Logger::flush()
{
  if (_ofstream)
  {
    std::lock_guard<std::mutex> lock(_ofstreamMutex);
    _ofstream.flush();
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
