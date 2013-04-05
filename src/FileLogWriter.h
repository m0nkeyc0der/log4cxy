#ifndef FILE_LOG_WRITER_H
#define FILE_LOG_WRITER_H

#include <cassert>
#include <fstream>
#include <iostream>

#include "LogWriter.h"

namespace log4cxy
{

class FileLogWriter : public LogWriter
{
public:
  FileLogWriter(const char* filename) : _ofstream(filename)
  {
    if (!_ofstream)
      std::cerr << "cannot open log file " << filename << std::endl;
  }

  bool isValid() volatile { return (const_cast<std::ofstream&>(_ofstream)).good(); }

  void writeLine(const std::string& message)
  {
    if (_ofstream)
      _ofstream << message;
  }

  void writeLine(const std::string& message) volatile
  {
    throw volatile_is_forbidden();
  }

  void flush()
  {
    if (_ofstream)
      _ofstream.flush();
  }

  void flush() volatile
  {
    throw volatile_is_forbidden();
  }

private:
  class volatile_is_forbidden : public std::exception
  {
  public:
    volatile_is_forbidden() {}
    const char* what() const throw()
    {
      return "FileLogWriter: calling non-thread-safe implementation is forbidden for this object";
    }
  };

  std::ofstream _ofstream;
};

} // ns log4cxy

#endif /* FILE_LOG_WRITER_H */
