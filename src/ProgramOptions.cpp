#include "ProgramOptions.h"

#include <iostream>

using namespace boost::program_options;

ProgramOptions::ProgramOptions(int argc, char* argv[]) : _isValid(false)
{
  try
  {
    variables_map vm;
    store(parse_command_line(argc, argv, desc()), vm);
    notify(vm);
    _isValid = (_numThread > 0 && _threadTimeLimit > 0 && _delay > 0);
    _needHelp = vm.count("help");
  }
  catch(const boost::program_options::error& err)
  {
    _isValid = false;
    std::cerr << err.what() << std::endl;
  }
}

void
ProgramOptions::printHelp()
{
  std::cout << desc();
}

options_description ProgramOptions::desc()
{
  options_description desc("Allowed options");
  desc.add_options()
      ("num-threads", boost::program_options::value<int>(&_numThread)->default_value(15),
                      "number of threads for test run")

      ("time",        boost::program_options::value<int>(&_threadTimeLimit)->default_value(10),
                      "time each thread will run for")

      ("delay",       boost::program_options::value<int>(&_delay)->default_value(2),
                      "threads do not start simultaneously, but with a delay (seconds)")

      ("log",         boost::program_options::value<std::string>(&_logFile)->default_value("trace.log"),
                      "log file name for trace log")

      ("help",        "produce help message");

  return desc;
}
