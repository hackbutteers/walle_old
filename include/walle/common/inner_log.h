#ifndef WALLE_COMMON_INNER_LOG_H_
#define WALLE_COMMON_INNER_LOG_H_
#include <walle/log/logger.h>
#include <walle/log/formatter_channel.h>
#include <walle/log/pattern_formatter.h>
#include <walle/log/channel/simple_file_channel.h>
#include <walle/common/singleton.h>

namespace walle {

class InnerLog : public Singleton<InnerLog> {
public:
    walle::log::LoggerPtr get_logger()
    {
        if(!_logger) {
            create_logger();
        }
        return _logger;
    }
private:
    void create_logger()
    {
        walle::log::FormatterChannel *pcl = new walle::log::FormatterChannel();
        walle::log::ChannelPtr ptr(pcl);
        walle::log::ChannelPtr file_ptr(new walle::log::MtSimpleFileChannel("./walle_log.txt"));
        pcl->set_channel(walle::log::ChannelPtr(file_ptr));
        walle::log::FormatterPtr fptr(new walle::log::PatternFormatter());
        fptr->set_property("format", "%p [%v]");
        pcl->set_formatter(fptr);
        walle::log::LoggerPtr plog(new walle::log::Logger("walle_log", ptr, walle::log::Level::eInfo));
        _logger = plog;
    }
private:
    walle::log::LoggerPtr _logger;
};


}

#define GET_LOGGER walle::InnerLog::get_instance()->get_logger()
#define LOG_TRACE(fmt, ...)  if((GET_LOGGER)->should_log(walle::log::Level::eTrace))\
                                (GET_LOGGER)->trace(__FILE__, __LINE__, fmt, __VA_ARGS__); else\
                                void(0)
#define LOG_DEBUG(fmt, ...)  if((GET_LOGGER)->should_log(walle::log::Level::eDebug))\
                                (GET_LOGGER)->debug(__FILE__, __LINE__, fmt, __VA_ARGS__); else\
                                void(0)
#define LOG_INFO(fmt, ...)  if((GET_LOGGER)->should_log(walle::log::Level::eInfo))\
                                (GET_LOGGER)->info(__FILE__, __LINE__, fmt, __VA_ARGS__); else\
                                void(0)
#define LOG_WARN(fmt, ...)  if((GET_LOGGER)->should_log(walle::log::Level::eWarn))\
                                (GET_LOGGER)->warn(__FILE__, __LINE__, fmt, __VA_ARGS__); else\
                                void(0)
#define LOG_ERROR(fmt, ...)  if((GET_LOGGER)->should_log(walle::log::Level::eError))\
                                (GET_LOGGER)->error(__FILE__, __LINE__, fmt, __VA_ARGS__); else\
                                void(0)
#define LOG_CRITICAL(fmt, ...)  if((GET_LOGGER)->should_log(walle::log::Level::eCritical))\
                                (GET_LOGGER)->critical(__FILE__, __LINE__, fmt, __VA_ARGS__); else\
                                void(0)
#define LOG_FATAL(fmt, ...)  if((GET_LOGGER)->should_log(walle::log::Level::eFatal))\
                                (GET_LOGGER)->fatal(__FILE__, __LINE__, fmt, __VA_ARGS__); else\
                                void(0)
#endif