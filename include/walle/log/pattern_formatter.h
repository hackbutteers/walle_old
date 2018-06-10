#ifndef WALLE_LOG_PATTERN_FORMATTER_H_
#define WALLE_LOG_PATTERN_FORMATTER_H_
#include <walle/log/formatter.h>
#include <walle/common/singleton.h>
#include <vector>
#include <memory>

namespace walle {
namespace log {

class flag_formatter;

/**  
 * @brief  pattern stirng
 * @note   
 *       * %s log_name
 *       * %v text
 *       * %l log level 
 *       * %p log level("INFO", "DEBUG")
 *       * %q log level short("I", "D")
 *       * %P process id
 *       * %T thread name
 *       * %I thread id
 *       * %U source file
 *       * %u source line
 *       * %w date/time abbreviated weekday (Mon, Tue, ...)
 *       * %W date/time full weekday (Monday, Tuesday, ...)
 *       * %b date/time abbreviated month (Jan, Feb, ...)
 *       * %B date/time full month (January, February, ...)
 *       * %d date/time zero-padded day of month (01 .. 31)
 *       * %e date/time day of month (1 .. 31) 
 *       * %f date/time space-padded day of month ( 1 .. 31)
 *       * %m date/time zero-padded month (01 .. 12)
 *       * %n
 *       * %o
 *       * %y date/time year without century (70)
 *       * %Y date/time year with century (1970)
 *       * %H 
 *       * %a 
 *       * %A 
 *       * %M 
 *       * %S
 *       * %i
 *       * $0
 *       * $1
 *        
 * 
 * @retval None
 */

typedef std::shared_ptr<walle::log::flag_formatter> log_flag_formatter_ptr; 
class log_flag_formatter_map : public Singleton<log_flag_formatter_map>{
public:
    log_flag_formatter_map();
    ~log_flag_formatter_map();
    log_flag_formatter_ptr get_flag_formatter(char c);
    log_flag_formatter_ptr get_char_formatter(char p);
    log_flag_formatter_ptr get_string_formatter(const std::string &str);
private:
    std::vector<log_flag_formatter_ptr>  flag_formatters;  
};


class PatternFormatter : public Formatter {
public:
    /** 
     * @brief  default log formatter
     * @note   
     * @retval 
     */
    PatternFormatter();    
    /** 
     * @brief  destructor
     * @note   
     * @retval 
     */
    ~PatternFormatter();

    /** 
     * @brief  
     * @note   
     * @param  &name: 
     * @param  &value: 
     * @retval 
     */
    bool set_property(const std::string &name, const std::string &value);

    /** 
     * @brief  
     * @note   
     * @param  &name: 
     * @param  &value: 
     * @retval 
     */
    bool get_property(const std::string &name, std::string &value);

    /** 
     * @brief  
     * @note   
     * @param  &msg: 
     * @retval None
     */
    void format(Message &msg);
private:
    /** 
     * @brief  
     * @note   
     * @retval 
     */
    bool compile_pattern();

    /** 
     * @brief  
     * @note   
     * @param  c: 
     * @retval None
     */
    bool handle_char(char c);

private:
    typedef std::vector<log_flag_formatter_ptr> FlagFormatterVec;
    typedef FlagFormatterVec::iterator FlagFormatterVecItr;
    std::string         _pattern;
    bool                _local;
    FlagFormatterVec    _flag_formatters;

}; //class PatternFormatter

} //namespace log
} //namespace walle
#endif //WALLE_LOG_PATTERN_FORMATTER_H_