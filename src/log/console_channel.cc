#include <walle/log/channel/console_channel.h>

namespace walle {
namespace log {

walle::Mutex ConsoleChannel::_mutex;
ConsoleChannel::ConsoleChannel()
{

}

void ConsoleChannel::log(Message &msg)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    fwrite((msg.formatted).data(), sizeof(char), msg.formatted.size(), stdout);
    flush();
}

 void ConsoleChannel::flush()
 {
     fflush(stdout);
 }
ConsoleChannel::~ConsoleChannel()
{

}

} //namespace log
} //namespace walle