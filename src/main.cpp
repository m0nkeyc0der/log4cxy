#include <iostream>

#include "Logger.h"

int main()
{
  Logger log("1.log", Logger::DEBUG);

  const char test_msg_1[] = "debug test message";
  log.log(1, test_msg_1, test_msg_1 + sizeof(test_msg_1));

  log.log(2, "info test message");
  log.log(3, "warn test message sample");
  log.log(4, "got an error message!!");

  return 0;
}
