#include <walle/log/registry.h>

namespace walle {
namespace log {
using walle::Mutex;

Registry::Registry()
{

}
Registry::~Registry()
{

}

ChannelPtr Registry::get_channel(const std::string &name) const
{
    walle::UniqueLock<Mutex> lock(_mutex);
    ChannelMap::const_iterator it = _channel_map.find(name);
    if(it == _channel_map.end()) {
        return ChannelPtr();
    }
    return it->second;
}

FormatterPtr Registry::get_formatter(const std::string &name) const
{
    walle::UniqueLock<Mutex> lock(_mutex);
    FormatterMap::const_iterator it = _formatter_map.find(name);
    if(it == _formatter_map.end()) {
        return FormatterPtr();
    }
    return it->second;
}

void Registry::register_channel(const std::string &name, ChannelPtr &ptr)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    ChannelMap::iterator it = _channel_map.find(name);
    if (it == _channel_map.end()) {
        _channel_map[name] = ptr;
    }
}

void Registry::register_formatter(const std::string &name, FormatterPtr &ptr)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    FormatterMap::iterator it = _formatter_map.find(name);
    if (it == _formatter_map.end()) {
        _formatter_map[name] = ptr;
    }
}

void Registry::unregister_channel(const std::string &name)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    ChannelMap::iterator it = _channel_map.find(name);
    if (it != _channel_map.end()) {
        _channel_map.erase(name);
    }
}
void Registry::unregister_formatter(const std::string &name)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    FormatterMap::iterator it = _formatter_map.find(name);
    if (it != _formatter_map.end()) {
        _formatter_map.erase(name);
    }
}


void Registry::reset_channel(const std::string &name, ChannelPtr &ptr)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    _channel_map[name] = ptr;
}
void Registry::reset_formatter(const std::string &name, FormatterPtr &ptr)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    _formatter_map[name] = ptr;
}

void Registry::clear()
{
     walle::UniqueLock<Mutex> lock(_mutex);
     _channel_map.clear();
     _formatter_map.clear();
}
} //namespace log
} //namespace walle