#ifndef WALLE_LOG_LOG_CONFIG_H_
#define WALLE_LOG_LOG_CONFIG_H_
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace walle {
namespace log {

struct  LogConfig{
    typedef std::map<std::string, std::string> PropertyMap;
    LogConfig() = default;
    ~LogConfig() = default;
    std::string log_name;
    std::string level;
    std::map<std::string, PropertyMap> channel_config;
};

typedef std::shared_ptr<LogConfig> LogConfigPtr;
struct LogConfigManager {
    typedef std::shared_ptr<std::map<std::string, LogConfigPtr> > log_config_map_ptr_type;
    log_config_map_ptr_type  log_config_map;
};

} //namespace log
} //namespace walle
#endif //WALLE_LOG_LOG_CONFIG_H_