#ifndef WALLE_LOG_FORMATTER_H_
#define WALLE_LOG_FORMATTER_H_
#include <walle/common/config_able.h>
#include <memory>

namespace walle {
namespace log {

class Message;
class Formatter;
typedef std::shared_ptr<Formatter> FormatterPtr;

class Formatter : public ConfigAble{
public:
    Formatter();
    virtual ~Formatter();
    virtual void format(Message &msg) = 0;

    /** 
     * @brief  no impl assert(false)
     * @note   
     * @param  name: 
     * @param  value: 
     * @retval 
     */
    bool set_property(const std::string& name, const std::string& value);

    /** 
     * @brief  no impl assert(false)
     * @note   
     * @param  name: 
     * @param  value: 
     * @retval 
     */
    bool get_property(const std::string& name, std::string& value);
};
} //namespace log
} //namespace walle
#endif //WALLE_LOG_FORMATTER_H_