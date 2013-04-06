#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <thread>

#include "Logger.h"
#include "LogPerfMonitor.h"
#include "StreamWriter.h"

namespace log4cxy
{

void writeToLog(Logger log, int numMessages)
{
  for(int i = 1; i <= numMessages; ++i)
  {
    log.log(i % LogLevel::ALERT,
            "Hello from thread %1% iteration %2%", std::this_thread::get_id(), i);
  }

  log.flush();
}

void runThreads(Logger log, int numThreadsToCreate, int numMessages)
{
  std::thread worker(writeToLog, log, numMessages);

  try
  {
    if (--numThreadsToCreate > 0)
      runThreads(log, numThreadsToCreate, numMessages);
  }
  catch(const std::exception& err)
  {
    log.log(ERROR, "runThreads tid=%1% numTreadsToCreate=%2% raises an exception: %3%",
                   std::this_thread::get_id(),
                   numThreadsToCreate,
                   err.what());
  }

  worker.join();
}

} // ns log4cxy

int main(int argc, char* argv[])
{
  using std::atoi;
  using namespace log4cxy;

  int num_messages;
  if (argc < 2 || !(num_messages = atoi(argv[1])))
    num_messages = 1000; // = default

  int num_threads;
  if (argc < 3 || !(num_threads = atoi(argv[2])))
    num_threads = 4; // = default

  const char* logfile = "trace.log";
  if (argc >= 4)
    logfile = argv[3];

  try
  {
    Logger log = Logger::create(logfile, LogLevel::TRACE);

    // would like to have alert messages on stdout
    log.addChainLogger(Logger::create(std::cout, LogLevel::ALERT));

    // would like to have error messages on stderr & in error.log
    log.addChainLogger(Logger::create(std::cerr, LogLevel::ERROR));
    log.addChainLogger(Logger::create("error.log", LogLevel::ERROR));

    try
    {
      StreamWriterThreadSafePtr filestreamWriter = log.getWriterImpl<StreamWriter>();
      assert(filestreamWriter != 0);

      Logger perfLog = Logger::create("perfmonitor.log", LogLevel::INFO).addChainLogger(log);
      LogPerfMonitor perfMon(filestreamWriter, perfLog);
      perfMon.runMonitorThread();

      log.log(LogLevel::ALERT, "setting number of threads = %1%, number of messages per thread = %2%",
                               num_threads,
                               num_messages);

      runThreads(log, num_threads, num_messages);
    }
    catch(const std::exception& err)
    {
      log.log(LogLevel::ERROR, err.what());
      return 1;
    }
  }
  catch(const std::exception& err)
  {
    std::cerr << err.what();
    return 1;
  }

  return 0;
}
