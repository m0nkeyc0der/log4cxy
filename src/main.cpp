#include <cstdlib>
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

  writeToLog(log, num_messages);

  return 0;
}
