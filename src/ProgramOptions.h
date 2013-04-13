#ifndef PROGRAM_OPTIONS_H
#define PROGRAM_OPTIONS_H

#include <boost/program_options.hpp>
#include <string>

class ProgramOptions
{
public:
  ProgramOptions(int argc, char* argv[]);

  bool isValid() const { return _isValid; }
  bool needHelp() const { return _needHelp; }
  void printHelp();

  int getNumThreads() const { return _numThread; }
  int getThreadTimeLimit() const { return _threadTimeLimit; }

  //! return delay before spawning new thread, seconds
  int getDelay() const { return _delay; }

  std::string getTraceLogFile() const { return _logFile; }

private:
  bool        _isValid;
  bool        _needHelp;
  int         _numThread;
  int         _threadTimeLimit;
  int         _delay;
  std::string _logFile;

  boost::program_options::options_description desc();
};

#endif // PROGRAM_OPTIONS_H
