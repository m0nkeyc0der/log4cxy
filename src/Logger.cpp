#include "Logger.h"

#include <iostream>
#include <iterator>

Logger::Logger(const char* filename, Level minLogLevel)
  : _minLogLevel(minLogLevel)
  , _ofstream(filename)
{
  if (!_ofstream)
    std::cerr << "cannot open log file " << filename << std::endl;
}

void Logger::log(int level, const char* begin, const char* end)
{
  if (level < _minLogLevel)
    return;

  bool isBadRange = (begin > end);
  if (isBadRange)
  {
    log(ERROR, "Logger - bad range in logger arguments");
    return;
  }

  if (_ofstream)
  {
    _ofstream.write(begin, end - begin);
    _ofstream << std::endl;
  }
}

void Logger::flush()
{
  if (_ofstream)
    _ofstream.flush();
}
