#include "LogPerfMonitor.h"

#include <chrono>
#include <exception>

namespace log4cxy
{

const std::chrono::seconds PERIOD(1);

void LogPerfMonitor::runMonitorThread()
{
  _initStat = _writerToTrack->getEventCount();

  std::thread thread(&monitorThreadFunc, this);
  _selfThread = std::move(thread);
}

void LogPerfMonitor::wrapUp() throw()
{
  try
  {
    if (!_selfThread.joinable())
      return;

    _logTo.log(ALERT, "LogPerfMonitor.wrapUp...");
    {
    std::lock_guard<std::mutex> lock(_writerMutex);
    _writerToTrack.reset();
    }

    _selfThread.join();
  }
  catch(const std::exception& err)
  {
    _logTo.log(ERROR, "LogPerfMonitor.wrapUp - %1%", err.what());
  }
}

void LogPerfMonitor::monitorThreadFunc(LogPerfMonitor* self)
{
  self->_logTo.log(ALERT, "LogPerfMonitor.runMonitorThread - thread %1% started",
                          std::this_thread::get_id());

  LogWriteEventCount stat(self->_initStat);
  StreamWriterThreadSafePtr writer;
  {
  std::lock_guard<std::mutex> lock(self->_writerMutex);
  writer = self->_writerToTrack;
  }

  while(writer)
  {
    std::this_thread::sleep_for(PERIOD);

    LogWriteEventCount statNew = writer->getEventCount();

    self->_logTo.log(ALERT, "log writer performance %1% messages/sec, %2% flush/sec",
                            (statNew.getNumWrites() - stat.getNumWrites()),
                            (statNew.getNumFlush() - stat.getNumFlush()));
    stat = statNew;

    // update to catch stop event
    std::lock_guard<std::mutex> lock(self->_writerMutex);
    writer = self->_writerToTrack;
  }
}

} // ns log4cxy
