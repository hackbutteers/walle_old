#ifndef WALLE_LOG_FACTORY_H_
#define WALLE_LOG_FACTORY_H_
#include <walle/common/dynamic_factory.h>
#include <walle/common/singleton.h>
#include <walle/log/channel.h>
#include <walle/log/formatter.h>

namespace walle {
namespace log {

class Factory : public walle::Singleton<Factory> {
public:
    typedef AbstractInstantiator<Channel>   ChannelInstantiator;
    typedef std::shared_ptr<ChannelInstantiator> ChannelInstantiatorPtr;
	typedef AbstractInstantiator<Formatter> FormatterFactory;
    typedef std::shared_ptr<FormatterFactory> FormatterFactoryPtr;

    Factory();

    ~Factory();

    void register_channel_class(const std::string &name, ChannelInstantiatorPtr ptr );

    void register_formatter_class(const std::string &name, FormatterFactoryPtr ptr );

    ChannelPtr create_channel(const std::string &name);

    FormatterPtr create_formatter(const std::string &name);
private:
    void register_buildin();

private:
    DynamicFactory<Channel>   _channel_factory;
    DynamicFactory<Formatter> _formatter_factory;
};
} //namespace log
} //namespace walle
#endif //WALLE_LOG_FACTORY_H_