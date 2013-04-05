#ifndef LOG_WRITER_H
#define LOG_WRITER_H

#include <memory>
#include <string>

namespace log4cxy
{

class LogWriter
{
public:
  //! @return false to indicate, that the object implements a stub
  virtual bool isValid() volatile = 0;

  virtual void writeLine(const std::string& message) = 0;
  virtual void writeLine(const std::string& message) volatile = 0;

  virtual void flush() = 0;
  virtual void flush() volatile = 0;

  virtual ~LogWriter() {}
};

//! To call not thread-safe implementation
typedef std::shared_ptr<LogWriter> LogWriterPtr;

//! To call thread-safe implementation
typedef std::shared_ptr<volatile LogWriter> LogWriterThreadSafePtr;

} // ns log4cxy

#endif /* LOG_WRITER_H */
