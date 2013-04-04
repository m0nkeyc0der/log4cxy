#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <thread>

#include "Logger.h"

void writeToLog(Logger& log, int numMessages)
{
  for(int i = 1; i <= numMessages; ++i)
  {
    log.log(i % (Logger::MAX_LEVEL + 1),
            "Hello from thread %1% iteration %2%", std::this_thread::get_id(), i);
  }
}

void runThreads(Logger& log, int numThreads, int numMessages)
{
  std::thread worker(writeToLog, std::ref(log), numMessages);

  if (numThreads > 1)
    runThreads(log, numThreads - 1, numMessages);

  worker.join();
}

int main(int argc, char* argv[])
{
  using std::atoi;

  Logger log("1.log", Logger::INFO);

  int num_messages;
  if (argc < 2 || !(num_messages = atoi(argv[1])))
    num_messages = 1000; // = default

  int num_threads;
  if (argc < 3 || !(num_threads = atoi(argv[2])))
    num_threads = 1; // = default

  log.log(Logger::INFO, "setting number of threads = %1%, number of messages per thread = %2%",
                        num_threads,
                        num_messages);

  try
  {
    runThreads(log, num_threads, num_messages);
  }
  catch(const std::exception& err)
  {
    std::cerr << err.what() << std::endl;
    log.log(Logger::ERROR, err.what());
  }

  return 0;
}
