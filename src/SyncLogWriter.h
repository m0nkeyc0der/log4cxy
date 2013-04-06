#ifndef SYNC_LOG_WRITER_H
#define SYNC_LOG_WRITER_H

#include "LogWriter.h"

#include <mutex>

namespace log4cxy
{

class SyncLogWriter : public LogWriter
{
public:
  SyncLogWriter(const LogWriterPtr& writerImpl) : _writerImpl(writerImpl)
  {
  }

  bool isValid() volatile { return raw(_writerImpl)->isValid(); }

  void writeLine(const std::string& message) { _writerImpl->writeLine(message); }
  void writeLine(const std::string& message) volatile
  {
    if (!isValid())
      return;

    std::lock_guard<std::mutex> lock(raw(_writerImplMutex));
    raw(_writerImpl)->writeLine(message);
  }

  void flush() { _writerImpl->flush(); }
  void flush() volatile
  {
    if (!isValid())
      return;

    std::lock_guard<std::mutex> lock(raw(_writerImplMutex));
    raw(_writerImpl)->flush();
  }

  virtual LogWriterPtr getImpl(FindImplPred pr)
  {
    if (pr(_writerImpl.get()))
      return _writerImpl;

    return LogWriterPtr();
  }

private:
  LogWriterPtr _writerImpl;
  std::mutex _writerImplMutex;

  template<typename T>
  static T& raw(volatile T& arg)
  {
    return const_cast<T&>(arg);
  }
};

} // ns log4cxy

#endif /* SYNC_LOG_WRITER_H */
