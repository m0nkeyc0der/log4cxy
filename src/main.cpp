#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <thread>

#include "Logger.h"

namespace log4cxy
{

void writeToLog(Logger log, int numMessages)
{
  for(int i = 1; i <= numMessages; ++i)
  {
    log.log(i % (LogLevel::MAX + 1),
            "Hello from thread %1% iteration %2%", std::this_thread::get_id(), i);
  }
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
    std::cerr << "next runThreads raises an exception: " << err.what() << std::endl;

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

  Logger log = Logger::create("1.log", LogLevel::INFO);

  int num_messages;
  if (argc < 2 || !(num_messages = atoi(argv[1])))
    num_messages = 1000; // = default

  int num_threads;
  if (argc < 3 || !(num_threads = atoi(argv[2])))
    num_threads = 4; // = default

  log.log(LogLevel::INFO, "setting number of threads = %1%, number of messages per thread = %2%",
                          num_threads,
                          num_messages);

  try
  {
    runThreads(log, num_threads, num_messages);
  }
  catch(const std::exception& err)
  {
    std::cerr << "runThreads error: " << err.what() << std::endl;
    log.log(LogLevel::ERROR, "runThreads error: %1%", err.what());

    return 1;
  }

  return 0;
}
