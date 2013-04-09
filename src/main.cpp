#include <atomic>
#include <cassert>
#include <chrono>
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

void writeToLog(Logger log, int numSeconds)
{
  using namespace std::chrono;

  try
  {
    // querying system clock is quite expensive for some OS, thus we would better
    // synchronize via atomic variable
    auto momentToStop = steady_clock::now() + seconds(numSeconds);
    volatile std::atomic_bool stopFlag(false);
    std::thread thread(
        [momentToStop,&stopFlag]
        {
          std::this_thread::sleep_until(momentToStop);
          stopFlag.store(true);
        });

    for(unsigned long i = 0; !stopFlag; ++i)
    {
      log.formatLog(++i % LogLevel::ALERT,
                    "Hello from thread %1% iteration %2%", std::this_thread::get_id(), i);
    }

    thread.join();
  }
  catch(const std::exception& err)
  {
    log.formatLog(ERROR, "writeToLog raises an exception: %1%",
                         err.what());
  }

  log.flush();
}

void runThreads(Logger log,
                Logger perfLog,
                int numThreadsToCreate,
                int ttlNumSeconds,
                float spawnDelay,
                volatile std::atomic_int* sharedWorkingThreadsCount = 0)
{
  volatile std::atomic_int swtcTheOnlyRootInstance (0);
  if (!sharedWorkingThreadsCount)
    sharedWorkingThreadsCount = &swtcTheOnlyRootInstance;

  // spawn the worker...
  std::thread worker(
      [log, &perfLog, ttlNumSeconds, sharedWorkingThreadsCount]
      {
        perfLog.formatLog(ALERT,
                          "runThreads worker tid=%1% started, number of active threads = %2%",
                          std::this_thread::get_id(),
                          ++*sharedWorkingThreadsCount);

        writeToLog(log, ttlNumSeconds);

        perfLog.formatLog(ALERT,
                          "runThreads worker tid=%1% stopped, number of active threads = %2%",
                          std::this_thread::get_id(),
                          --*sharedWorkingThreadsCount);
      });

  // spawn the next worker...
  try
  {
    if (--numThreadsToCreate > 0)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(spawnDelay * 1000.)));

      runThreads(log,
                 perfLog,
                 numThreadsToCreate,
                 ttlNumSeconds,
                 spawnDelay,
                 sharedWorkingThreadsCount);
    }
  }
  catch(const std::exception& err)
  {
    log.formatLog(ERROR, "runThreads tid=%1% numTreadsToCreate=%2% raises an exception: %3%",
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

  int num_threads;
  if (argc < 2 || !(num_threads = atoi(argv[1])))
    num_threads = 15; // = default

  int ttl_num_seconds; // to each thread for
  if (argc < 3 || !(ttl_num_seconds = atoi(argv[2])))
    ttl_num_seconds = 10; // = default

  float spawn_delay = 2.;
  if (argc >= 4)
    spawn_delay = atof(argv[3]);

  const char* logfile = "trace.log";
  if (argc >= 5)
    logfile = argv[4];

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

      log.formatLog(LogLevel::ALERT, "setting number of threads = %1%, "
                                     "number of seconds to run = %2%, "
                                     "each thread starts with %3% sec delay",
                                     num_threads,
                                     ttl_num_seconds,
                                     spawn_delay);

      runThreads(log, perfLog, num_threads, ttl_num_seconds, spawn_delay);
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
