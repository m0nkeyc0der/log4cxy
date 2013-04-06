#ifndef STREAM_WRITER_H
#define STREAM_WRITER_H

#include <iostream>

#include "LogWriteEventCount.h"
#include "LogWriter.h"

namespace log4cxy
{

class StreamWriter : public LogWriter
{
public:
  /**!
   * go ahead! call the constructor!
   *
   * @param os is allowed to be an uninitialized stream reference during this call
   */
  StreamWriter(std::ostream& os) : _stream(os)
  {
  }

  bool isValid() volatile { return (const_cast<std::ostream&>(_stream)).good(); }

  void writeLine(const std::string& message)
  {
    if (_stream)
    {
      _stream << message;
      _eventCount.countWrite();
    }
  }
  void writeLine(const std::string& message) volatile { throw VolatileIsForbidden(); }

  void flush()
  {
    if (_stream)
    {
      _stream.flush();
      _eventCount.countFlush();
    }
  }
  void flush() volatile { throw VolatileIsForbidden(); }

  const volatile LogWriteEventCount& getEventCount() const volatile { return _eventCount; }

private:
  class VolatileIsForbidden : public std::exception
  {
  public:
    VolatileIsForbidden() {}
    const char* what() const throw()
    {
      return "StreamWriter: calling non-thread-safe implementation is forbidden for this object";
    }
  };

  std::ostream& _stream;
  volatile LogWriteEventCount _eventCount;
};

typedef std::shared_ptr<volatile StreamWriter> StreamWriterThreadSafePtr;

} // ns log4cxy

#endif /* STREAM_WRITER_H */
