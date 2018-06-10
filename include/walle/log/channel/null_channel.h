#ifndef WALLE_LOG_CHANNEL_NULL_CHANNEL_H_
#define WALLE_LOG_CHANNEL_NULL_CHANNEL_H_
#include <walle/log/message.h>
#include <walle/config/base.h>

namespace walle {
namespace log {

class ConsoleChannel: public Channel {
public:
    ConsoleChannel()
    {

    }

    void log(Message &msg)
    {
        WALLE_UNUSED(msg);
    }

    ~ConsoleChannel()
    {

    }
    void log(MessagePtr &msg)
    {
        WALLE_UNUSED(msg);
    }
    void flush()
    {
        
    }

};

}
}
#endif