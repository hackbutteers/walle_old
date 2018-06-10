#ifndef WALLE_LOG_CHANNEL_H_
#define WALLE_LOG_CHANNEL_H_
#include <walle/common/config_able.h>
#include <memory>

namespace walle {
namespace log {

class Message;
typedef std::shared_ptr<Message> MessagePtr;

class Channel;
typedef std::shared_ptr<Channel> ChannelPtr;

class Channel : public ConfigAble, public std::enable_shared_from_this<Channel>{
public:
    /** 
     * @brief  create a channel
     * @note   
     * @retval 
     */
    Channel();    
    /** 
     * @brief  open a channel for log.
     * @note   default do nothing.
     * @retval 
     */
    virtual bool open();

    /** 
     * @brief  close the channel
     * @note   
     * @retval None
     */
    virtual void close();

    /** 
     * @brief  
     * @note   
     * @param  name: 
     * @param  value: 
     * @retval None
     */    
    bool set_property(const std::string& name, const std::string& value);

    bool get_property(const std::string& name, std::string& value);

    virtual void log(Message &msg) = 0;

    virtual void log(MessagePtr &msg) = 0;
    virtual void flush() = 0;
    virtual ~Channel();
};
} //namespace log
} //namespace walle 
#endif //WALLE_LOG_CHANNEL_H_