#include <walle/log/log_registry.h>
#include <walle/log/logger.h>

namespace walle {
namespace log {


LoggerPtr LogRegistry::create(const std::string &name, const ChannelPtr &ptr, Level l)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    LoggerMap::iterator it = _log_map.find(name);
    if(it != _log_map.end()) {
        WALLE_ASSERT(false, "alread exists!");
        return it->second;
    }
    LoggerPtr log_ptr(new Logger(name, ptr, l));
    _log_map[name] = log_ptr;
    return log_ptr;
} 

LoggerPtr LogRegistry::get(const std::string &name)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    LoggerMap::iterator it = _log_map.find(name);
    if(it != _log_map.end()) {
        return it->second;
    }
    return LoggerPtr();
 }

void LogRegistry::destroy(const std::string &name)
{
     walle::UniqueLock<Mutex> lock(_mutex);
    LoggerMap::iterator it = _log_map.find(name);
    if(it != _log_map.end()) {
       _log_map.erase(it);
    }    
}

void LogRegistry::names(std::vector<std::string> &ns)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    LoggerMap::iterator it = _log_map.begin();
    while(it != _log_map.end()) {
        ns.push_back(it->first);
    }
}
} //namespace log
} //namespace walle