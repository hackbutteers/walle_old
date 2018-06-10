#ifndef WALLE_LOG_LOG_REGISTRY_H_
#define WALLE_LOG_LOG_REGISTRY_H_
#include <walle/common/singleton.h>
#include <walle/config/base.h>
#include <walle/log/channel.h>
#include <walle/sys/mutex.h>
#include <walle/log/level.h>
#include <memory>
#include <unordered_map>

namespace walle {
namespace log {
class Logger;
typedef std::shared_ptr<Logger> LoggerPtr;

class LogRegistry :public walle::Singleton<LogRegistry> {
public:
    ~LogRegistry()
    {

    }
    LoggerPtr create(const std::string &name, const ChannelPtr &ptr, Level l);

    LoggerPtr get(const std::string &name);
    
    void      destroy(const std::string &name);

    size_t  count() const;

    void   names(std::vector<std::string> &ns);

private:
    LogRegistry() {} 
    typedef std::unordered_map<std::string, LoggerPtr> LoggerMap;
    LoggerMap                  _log_map;
    mutable walle::Mutex       _mutex;
};

WALLE_FORCE_INLINE size_t LogRegistry::count() const
{
    walle::UniqueLock<Mutex> lock(_mutex);
    return _log_map.size();
}
} //namespace log
} //namespace walle
#endif