#ifndef WALLE_LOG_CONSOLE_CHANNEL_H_
#define WALLE_LOG_CONSOLE_CHANNEL_H_
#include <walle/log/channel.h>
#include <walle/log/message.h>
#include <walle/sys/mutex.h>
#include <cstdio>

namespace walle {
namespace log {

class ConsoleChannel: public Channel {
public:
    ConsoleChannel();

    void log(Message &msg);
    void log(MessagePtr &msg)
    {
        log(*msg);
    }
    void flush();
    
    ~ConsoleChannel();
private:
    static walle::Mutex _mutex;
};

} //namespace log
} //namespace walle
#endif //WALLE_LOG_CONSOLE_CHANNEL_H_