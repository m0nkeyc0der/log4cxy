#ifndef FILE_LOG_WRITER_H
#define FILE_LOG_WRITER_H

#include <fstream>

#include "StreamWriter.h"

namespace log4cxy
{

class FileLogWriter : public StreamWriter
{
public:
  FileLogWriter(const char* filename)
    : StreamWriter(_ofstream)
    , _ofstream(filename)
  {
    if (!_ofstream)
      std::cerr << "cannot open log file " << filename << std::endl;
  }

private:
  std::ofstream _ofstream;
};

} // ns log4cxy

#endif /* FILE_LOG_WRITER_H */
