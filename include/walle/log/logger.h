#ifndef WALLE_LOG_LOGGER_H_
#define WALLE_LOG_LOGGER_H_
#include <walle/log/channel.h>
#include <walle/log/level.h>
#include <walle/log/message.h>
#include <walle/format/format.h>
#include <walle/format/printf.h>
#include <string>
#include <memory>
#include <iostream>

namespace walle {
namespace log {
class Logger;
typedef std::shared_ptr<Logger> LoggerPtr;

class Logger {
public:
    Logger(const std::string &name, const ChannelPtr &cptr, Level l)
        :_name(name),
         _channel(cptr),
         _level(l)
    {

    }
    ~Logger() {}
    void set_channel(const ChannelPtr &cptr)
    {
        _channel = cptr;
    }
    void set_level(Level l)
    {
        _level = l;
    }

    std::string name() const
    {
        return _name;
    }
    /** 
     * @brief  trace
     * @note   
     * @param  *file: 
     * @param  line: 
     * @param  *msg: 
     * @retval None
     */
    void trace(const char *file, int line, const char *msg);

    template <typename T>
    void trace(const char *file, int line, const T &msg);

    template <typename Arg1, typename... Args> 
    void trace(const char *file, int line, const char* fmt, const Arg1&, const Args&... args);

    /** 
     * @brief  debug
     * @note   
     * @param  *file: 
     * @param  line: 
     * @param  *msg: 
     * @retval None
     */
    void debug(const char *file, int line, const char *msg);

    template <typename T>
    void debug(const char *file, int line, const T &msg);

    template <typename Arg1, typename... Args> 
    void debug(const char *file, int line, const char* fmt, const Arg1&, const Args&... args);

    /** 
     * @brief  info
     * @note   
     * @param  *file: 
     * @param  line: 
     * @param  *msg: 
     * @retval None
     */
    void info(const char *file, int line, const char *msg);

    template <typename T>
    void info(const char *file, int line, const T &msg);

    template <typename Arg1, typename... Args> 
    void info(const char *file, int line,const char* fmt, const Arg1&, const Args&... args);

    /** 
     * @brief  warn
     * @note   
     * @param  *file: 
     * @param  line: 
     * @param  *msg: 
     * @retval None
     */
    void warn(const char *file, int line, const char *msg);

    template <typename T>
    void warn(const char *file, int line, const T &msg);

    template <typename Arg1, typename... Args> 
    void warn(const char *file, int line,const char* fmt, const Arg1&, const Args&... args);

    /** 
     * @brief  error
     * @note   
     * @param  *file: 
     * @param  line: 
     * @param  *msg: 
     * @retval None
     */
    void error(const char *file, int line, const char *msg);

    template <typename T>
    void error(const char *file, int line, const T &msg);

    template <typename Arg1, typename... Args> 
    void error(const char *file, int line,const char* fmt, const Arg1&, const Args&... args);

    /** 
     * @brief  critical
     * @note   
     * @param  *file: 
     * @param  line: 
     * @param  *msg: 
     * @retval None
     */
    void critical(const char *file, int line, const char *msg);

    template <typename T>
    void critical(const char *file, int line, const T &msg);

    template <typename Arg1, typename... Args> 
    void critical(const char *file, int line,const char* fmt, const Arg1&, const Args&... args);

    /** 
     * @brief  fatal
     * @note   
     * @param  *file: 
     * @param  line: 
     * @param  *msg: 
     * @retval None
     */
    void fatal(const char *file, int line, const char *msg);

    template <typename T>
    void fatal(const char *file, int line, const T &msg);

    template <typename Arg1, typename... Args> 
    void fatal(const char *file, int line, const char* fmt, const Arg1&, const Args&... args);  

    /** 
     * @brief  
     * @note   
     * @param  lvl: 
     * @param  *file: 
     * @param  line: 
     * @param  fmt: 
     * @param  args: 
     * @retval None
     */
    template <typename... Args> 
    void log(Level lvl, const char *file, int line, const char* fmt, const Args&... args);

    /** 
     * @brief  
     * @note   
     * @param  lvl: 
     * @param  *file: 
     * @param  line: 
     * @param  &msg: 
     * @retval None
     */
    template <typename T> 
    void log(Level lvl, const char *file, int line, const T &msg);

    /** 
     * @brief  
     * @note   
     * @param  lvl: 
     * @param  file: 
     * @param  line: 
     * @param  *msg: 
     * @retval None
     */
    template <typename T> 
    void log(Level lvl, const char* file, int line, const char *msg);

    bool should_log(Level lvl)
    {
        return lvl >= _level;
    }
private:
    const std::string  _name;
    ChannelPtr         _channel;
    Level              _level;
};



template <typename T> 
WALLE_FORCE_INLINE void Logger::log(Level lvl, const char* file, int line, const char *cmsg)
{
    ChannelPtr ptr = _channel;
    if(_level <= lvl && ptr) {
        Message msg;
        msg.level = lvl;
        msg.log_name = walle::StringRef(_name);
        msg.file = file;
        msg.line = line;
        msg.raw<< cmsg;
        ptr->log(msg);
    }
}

template <typename T>  
WALLE_FORCE_INLINE void Logger::log(Level lvl, const char *file, int line, const T &cmsg)
{
    ChannelPtr ptr = _channel;
    if(_level <= lvl && ptr) {
        Message msg;
        msg.level = lvl;
        msg.log_name = walle::StringRef(_name);
        msg.file = file;
        msg.line = line;
        msg.raw<<cmsg;
        ptr->log(msg);
    }
}

template <typename... Args> 
WALLE_FORCE_INLINE void Logger::log(Level lvl, const char *file, 
    int line, const char* fmt, const Args&... args)
{
    ChannelPtr ptr = _channel;
   if(_level <= lvl && ptr) {
        Message msg;
        msg.level = lvl;
        msg.log_name = walle::StringRef(_name);
        msg.file = file;
        msg.line = line;
        walle::printf(msg.raw, fmt, args...);
        ptr->log(msg);
    }    
}

WALLE_FORCE_INLINE void Logger::trace(const char *file, int line, const char *msg)
{
    log(Level::eTrace, file, line, msg);
}

template <typename T>
WALLE_FORCE_INLINE void Logger::trace(const char *file, int line, const T &msg)
{
    log(Level::eTrace, file, line, msg);
}

template <typename Arg1, typename... Args> 
WALLE_FORCE_INLINE void Logger::trace(const char *file, int line,const char* fmt, const Arg1 &arg1, const Args&... args)
{
    log(Level::eTrace, file, line, fmt, arg1, args...);
}

WALLE_FORCE_INLINE void Logger::debug(const char *file, int line, const char *msg)
{
    log(Level::eDebug, file, line, msg);
}

template <typename T>
WALLE_FORCE_INLINE void Logger::debug(const char *file, int line, const T &msg)
{
    log(Level::eDebug, file, line, msg);
}

template <typename Arg1, typename... Args> 
WALLE_FORCE_INLINE void Logger::debug(const char *file, int line, const char* fmt, const Arg1 &arg1, const Args&... args)
{
    log(Level::eDebug, file, line, fmt, arg1, args...);
}

WALLE_FORCE_INLINE void Logger::info(const char *file, int line, const char *msg)
{
    log(Level::eInfo, file, line, msg);
}

template <typename T>
WALLE_FORCE_INLINE void Logger::info(const char *file, int line, const T &msg)
{
    log(Level::eInfo, file, line, msg);
}

template <typename Arg1, typename... Args> 
WALLE_FORCE_INLINE void Logger::info(const char *file, int line, const char* fmt, const Arg1 &arg1, const Args&... args)
{
    log(Level::eInfo, file, line, fmt, arg1, args...);
}

WALLE_FORCE_INLINE void Logger::warn(const char *file, int line, const char *msg)
{
    log(Level::eWarn, file, line, msg);
}

template <typename T>
WALLE_FORCE_INLINE void Logger::warn(const char *file, int line, const T &msg)
{
    log(Level::eWarn, file, line, msg);
}

template <typename Arg1, typename... Args> 
WALLE_FORCE_INLINE void Logger::warn(const char *file, int line, const char* fmt, const Arg1 &arg1, const Args&... args)
{
    log(Level::eWarn, file, line, fmt, arg1, args...);
}

WALLE_FORCE_INLINE void Logger::error(const char *file, int line, const char *msg)
{
    log(Level::eError, file, line, msg);
}

template <typename T>
WALLE_FORCE_INLINE void Logger::error(const char *file, int line, const T &msg)
{
    log(Level::eError, file, line, msg);
}

template <typename Arg1, typename... Args> 
WALLE_FORCE_INLINE void Logger::error(const char *file, int line, const char* fmt, const Arg1 &arg1, const Args&... args)
{
    log(Level::eError, file, line, fmt, arg1, args...);
}

WALLE_FORCE_INLINE void Logger::critical(const char *file, int line, const char *msg)
{
    log(Level::eCritical, file, line, msg);
}

template <typename T>
WALLE_FORCE_INLINE void Logger::critical(const char *file, int line, const T &msg)
{
    log(Level::eCritical, file, line, msg);
}

template <typename Arg1, typename... Args> 
WALLE_FORCE_INLINE void Logger::critical(const char *file, int line, const char* fmt, const Arg1 &arg1, const Args&... args)
{
    log(Level::eCritical, file, line, fmt, arg1, args...);
}

WALLE_FORCE_INLINE void Logger::fatal(const char *file, int line, const char *msg)
{
    log(Level::eFatal, file, line, msg);
}

template <typename T>
WALLE_FORCE_INLINE void Logger::fatal(const char *file, int line, const T &msg)
{
    log(Level::eFatal, file, line, msg);
}

template <typename Arg1, typename... Args> 
WALLE_FORCE_INLINE void Logger::fatal(const char *file, int line, const char* fmt, const Arg1 &arg1, const Args&... args)
{
    log(Level::eFatal, file, line, fmt, arg1, args...);
}

} //namespace log
} //namespace walle
#endif //WALLE_LOG_LOGGER_H_