#ifndef WALLE_TIME_FORMATRER_H_
#define WALLE_TIME_FORMATRER_H_
#include <walle/format/format.h>
#include <walle/time/date_time.h>
#include <walle/common/config_able.h>
#include <walle/common/singleton.h>
#include <vector>
#include <memory>
#include <string>

namespace walle {

namespace format_detail {
class flag_formatter {
public:
    virtual void format(walle::MemoryWriter &our, const DateTime &dt) = 0;
};
 
}

typedef std::shared_ptr<walle::format_detail::flag_formatter> flag_formatter_ptr; 
class flag_formatter_map : public Singleton<flag_formatter_map>{
public:
    flag_formatter_map();
    ~flag_formatter_map();
    flag_formatter_ptr get_flag_formatter(char c);
    flag_formatter_ptr get_char_formatter(char p);
    flag_formatter_ptr get_string_formatter(const std::string &str);
private:
    std::vector<flag_formatter_ptr>  flag_formatters;  
};

class TimeFormatter :public ConfigAble {
public:
    TimeFormatter();
    ~TimeFormatter();
    bool set_property(const std::string& name, const std::string& value);

    /** 
     * @brief  get the property with the given name
     * @note   
     * @param  name: 
     * @param  value: 
     * @retval None
     */
    bool get_property(const std::string& name, std::string& value);
  
    void set_user_formatter(const std::string &pattern, const std::vector<flag_formatter_ptr> &fmt);
    void format(walle::MemoryWriter &out, DateTime &dt);
private:
    bool compile_pattern(std::string &pattern);
    bool handle_char(char c);
private:
    std::string                      _pattern;
    std::vector<flag_formatter_ptr>  _formatters;
};
}
#endif //WALLE_TIME_FLAG_FORMATRER_H_