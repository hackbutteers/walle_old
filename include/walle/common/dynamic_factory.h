#ifndef WALLE_COMMON_DYNAMIC_FACTORY_H_
#define WALLE_COMMON_DYNAMIC_FACTORY_H_
#include <walle/config/base.h>
#include <walle/common/instantiator.h>
#include <walle/sys/mutex.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace walle {
template <class Base>
class DynamicFactory{
public:
    typedef AbstractInstantiator<Base>       AbstractFactory;
    typedef std::shared_ptr<AbstractFactory> AbstractFactoryPtr;
    DynamicFactory()
    {

    } 
    ~DynamicFactory()
    {

    }

    Base* create_instance(const std::string& className) const
    {
        UniqueLock<Mutex> lock(_mutex);
        typename  FactoryMap::const_iterator it = _map.find(className);
        if (it != _map.end()) {
			return it->second->create_instance();
        } 
        return WALLE_NULL;
    }

    void register_class(const std::string& className, AbstractFactoryPtr pAbstractFactory)
    {
        UniqueLock<Mutex> lock(_mutex);
        typename FactoryMap::iterator it = _map.find(className);
        if(it == _map.end()) {
            _map[className] = pAbstractFactory;
        }

    }
    void unregister_class(const std::string& className)
    {
        UniqueLock<Mutex> lock(_mutex);
        typename FactoryMap::iterator it = _map.find(className);
        if(it != _map.end()) {
            _map.erase(className);
        }

    }
    bool is_class(const std::string& className) const
    {
        UniqueLock<Mutex> lock(_mutex);
        typename FactoryMap::iterator it = _map.find(className);
        if(it == _map.end()) {
            return false;
        }
        return true; 
    }
private:
    typedef std::unordered_map<std::string, AbstractFactoryPtr> FactoryMap;
    FactoryMap                              _map;
    mutable walle::Mutex                    _mutex;
private:
    WALLE_NON_COPYABLE(DynamicFactory);
};

}
#endif //WALLE_COMMON_DYNAMIC_FACTORY_H_