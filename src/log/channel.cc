#include <walle/log/channel.h>

namespace walle {
namespace log {

Channel::Channel()
{

}
Channel::~Channel()
{

}

bool Channel::open()
{
    return false;
}

void Channel::close()
{

}

bool Channel::set_property(const std::string& name, const std::string& value)
{
    return false;
}

bool Channel::get_property(const std::string& name, std::string& value)
{
    return false;
}

} //namespace log
} //namespace walle