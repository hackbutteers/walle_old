#ifndef WALLE_LOG_MESSAGE_H_
#define WALLE_LOG_MESSAGE_H_
#include <walle/time/timestamp.h>
#include <walle/sys/this_thread.h>
#include <walle/format/format.h>
#include <walle/log/level.h>
#include <walle/config/base.h>
#include <string>
#include <memory>

namespace walle {
namespace log {

class Message {
public:
    static const char *kNullFile;
    static const int   kNullLine;  
    StringRef                           log_name;
    walle::Timestamp                    timestamp;
    Level                               level;
    walle::this_thread::tid_t           tid;
    StringRef                           thread_name;
    long                                pid;
    const char                         *file;
    int                                 line;
    size_t                              msg_id;
    walle::MemoryWriter                 raw;
    walle::MemoryWriter                 formatted;
    Message()
    : log_name(""),
      timestamp(),
      level(Level::eTrace),
      tid(0),
      thread_name(""),
      pid(0),
      file(kNullFile),
      line(kNullLine),
      msg_id(0),
      raw(),
      formatted()
    {

    }
};

typedef std::shared_ptr<Message> MessagePtr;
}
}
#endif //WALLE_LOG_MESSAGE_H_
