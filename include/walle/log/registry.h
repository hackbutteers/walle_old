#ifndef WALLE_LOG_REGISTRY_H_
#define WALLE_LOG_REGISTRY_H_
#include <walle/log/channel.h>
#include <walle/log/formatter.h>
#include <walle/sys/mutex.h>
#include <walle/common/singleton.h>
#include <unordered_map>

namespace walle {
namespace log {

class Registry : public walle::Singleton<Registry>{
public:
    Registry();
    ~Registry();
    ChannelPtr get_channel(const std::string &name) const;
    FormatterPtr get_formatter(const std::string &name) const;

    void register_channel(const std::string &name, ChannelPtr &ptr);
    void register_formatter(const std::string &name, FormatterPtr &ptr);

    void unregister_channel(const std::string &name);
    void unregister_formatter(const std::string &name);

    void reset_channel(const std::string &name, ChannelPtr &ptr);
    void reset_formatter(const std::string &name, FormatterPtr &ptr);
    void clear();
private:
    typedef std::unordered_map<std::string, ChannelPtr> ChannelMap;
    typedef std::unordered_map<std::string, FormatterPtr> FormatterMap;
    
    ChannelMap             _channel_map;
	FormatterMap           _formatter_map;
    mutable walle::Mutex   _mutex;
};
}
}
#endif //WALLE_LOG_REGISTRY_H_