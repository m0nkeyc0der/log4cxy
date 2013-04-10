#ifndef ASYNC_LOG_WRITER_H
#define ASYNC_LOG_WRITER_H

#include "LogWriter.h"

// stdlib headers
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace log4cxy
{

class AsyncLogWriter : public LogWriter
{
public:
  AsyncLogWriter(const LogWriterPtr& writerImpl) : _writerImpl(writerImpl)
  {
    std::thread thread([this]{ workerThread(); });
    _queueConsumerThread = std::move(thread);
  }

  /*!
   * Have to use deleter() for shared_ptr !!!
   */
  ~AsyncLogWriter()
  {
    assert(!_queueConsumerThread.joinable() ||
           !"AsyncLogWriter worker thread is still joinable in destructor");
  }

  /*!
   * Although doing those things in destructor is also acceptable approach,
   * deleter() is more appropriate to handle any possible polymorphism in the future
   */
  static void deleter(AsyncLogWriter* asyncWriter)
  {
    if (asyncWriter->_queueConsumerThread.joinable())
    {
      std::lock_guard<std::mutex> lock(asyncWriter->_queueMutex);
      QueueItemAction emptyAction;
      asyncWriter->_queue.push(emptyAction);
      asyncWriter->_queueNonEmptyCondVar.notify_one();
    }

    asyncWriter->_queueConsumerThread.join();

    delete asyncWriter; // free memory
  }

  bool isValid() volatile { return raw(_writerImpl)->isValid(); }

  void writeLine(const std::string& message) { _writerImpl->writeLine(message); }
  void writeLine(const std::string& message) volatile
  {
    if (!isValid())
      return;

    AsyncLogWriter* raw = const_cast<AsyncLogWriter*>(this);

    {
    std::lock_guard<std::mutex> lock(raw->_queueMutex);
    raw->_queue.push([raw, message]{ raw->_writerImpl->writeLine(message); });
    }
    raw->_queueNonEmptyCondVar.notify_one();
  }

  void flush() { _writerImpl->flush(); }
  void flush() volatile
  {
    if (!isValid())
      return;

    AsyncLogWriter* raw = const_cast<AsyncLogWriter*>(this);

    {
    std::lock_guard<std::mutex> lock(raw->_queueMutex);
    raw->_queue.push([raw]{ raw->_writerImpl->flush(); });
    }
    raw->_queueNonEmptyCondVar.notify_one();
  }

  virtual LogWriterPtr getImpl(FindImplPred pr)
  {
    if (pr(_writerImpl.get()))
      return _writerImpl;

    return LogWriterPtr();
  }

private:
  typedef std::function<void ()>      QueueItemAction;
  typedef std::queue<QueueItemAction> Queue;

  LogWriterPtr _writerImpl;

  Queue _queue;
  std::mutex _queueMutex;
  // to signal the queue changes the state and becomes non-empty
  std::condition_variable _queueNonEmptyCondVar;
  std::thread _queueConsumerThread;

  template<typename T>
  static T& raw(volatile T& arg)
  {
    return const_cast<T&>(arg);
  }

  void workerThread()
  {
    bool done = false;
    while (!done)
    {
      // Catch _queue into workCopy and unlock mutex to allow background threads continue
      // asynchronous queue writes
      Queue workCopy;
      {
      std::unique_lock<std::mutex> lock(_queueMutex);
      do
      {
        _queueNonEmptyCondVar.wait(lock);
      } while (_queue.empty()); // loop to avoid spurious wakeups
      workCopy.swap(_queue);
      }

      for(; !workCopy.empty(); workCopy.pop())
      {
        QueueItemAction& itemAction = workCopy.front();

        if (!itemAction)
          done = true;
        else
          itemAction();
      }
    }
  }
};

} // ns log4cxy

#endif /* ASYNC_LOG_WRITER_H */
