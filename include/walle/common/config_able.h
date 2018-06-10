#ifndef WALLE_COMMON_CONFIG_ABLE_H_
#define WALLE_COMMON_CONFIG_ABLE_H_
#include <string>

namespace walle {

class ConfigAble {
public:
    /** 
     * @brief  create a config
     * @note   
     * @retval 
     */
    ConfigAble();

    /** 
     * @brief  destroys the config
     * @note   
     * @retval None
     */
    virtual ~ConfigAble();
    
    /** 
     * @brief  set the property with given name to the given value.
     * @note   
     * @param  name: 
     * @param  value: 
     * @retval None
     */
    virtual bool set_property(const std::string& name, const std::string& value) = 0;

    /** 
     * @brief  get the property with the given name
     * @note   
     * @param  name: 
     * @param  value: 
     * @retval None
     */
    virtual bool get_property(const std::string& name, std::string& value) = 0;

};

}
#endif //WALLE_COMMON_CONFIG_ABLE_H_