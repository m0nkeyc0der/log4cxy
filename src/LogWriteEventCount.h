#ifndef LOG_WRITE_EVENT_COUT_H
#define LOG_WRITE_EVENT_COUT_H

#include <atomic>

namespace log4cxy
{

class LogWriteEventCount
{
public:
  typedef unsigned long IntType;

  LogWriteEventCount() : _numWrites(0), _numFlush(0)
  {}

  LogWriteEventCount(const volatile LogWriteEventCount& rhs)
  {
    _numWrites = rhs.getNumWrites();
    _numFlush  = rhs.getNumFlush();
  }

  LogWriteEventCount& operator= (const LogWriteEventCount& rhs)
  {
    _numWrites = rhs.getNumWrites();
    _numFlush  = rhs.getNumFlush();
    return (*this);
  }

  LogWriteEventCount& operator= (const volatile LogWriteEventCount& rhs)
  {
    _numWrites = rhs.getNumWrites();
    _numFlush  = rhs.getNumFlush();
    return (*this);
  }

  void countWrite() volatile { ++_numWrites; }
  void countFlush() volatile { ++_numFlush;  }

  IntType getNumWrites() const { return _numWrites; }
  IntType getNumFlush()  const { return _numFlush; }

  IntType getNumWrites() const volatile { return _numWrites; }
  IntType getNumFlush()  const volatile { return _numFlush; }
private:
  std::atomic<IntType> _numWrites, _numFlush;
};

} // ns log4cxy

#endif /* LOG_WRITE_EVENT_COUT_H */
