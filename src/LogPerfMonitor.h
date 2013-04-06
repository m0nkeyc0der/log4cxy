#ifndef LOG_PERF_MONITOR_H
#define LOG_PERF_MONITOR_H

#include <mutex>
#include <thread>

#include "Logger.h"
#include "LogWriteEventCount.h"
#include "StreamWriter.h"

namespace log4cxy
{

class LogPerfMonitor
{
public:
  LogPerfMonitor(StreamWriterThreadSafePtr writerToTrack, Logger logTo)
    : _writerToTrack(writerToTrack)
    , _logTo(logTo)
  {}

  ~LogPerfMonitor() { wrapUp(); }

  void runMonitorThread();
  void wrapUp() throw();

private:
  std::mutex _writerMutex;
  StreamWriterThreadSafePtr _writerToTrack;
  Logger _logTo;
  std::thread _selfThread;
  LogWriteEventCount _initStat;

  static void monitorThreadFunc(LogPerfMonitor* self);
};

} // ns log4cxy

#endif /* LOG_PERF_MONITOR_H */
