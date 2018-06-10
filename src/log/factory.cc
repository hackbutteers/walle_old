#include <walle/log/factory.h>

namespace walle {
namespace log {

Factory::Factory()
{
    register_buildin();
}

Factory::~Factory()
{

}

void Factory::register_channel_class(const std::string &name, ChannelInstantiatorPtr ptr )
{
    _channel_factory.register_class(name, ptr);
}

void Factory::register_formatter_class(const std::string &name, FormatterFactoryPtr ptr )
{
    _formatter_factory.register_class(name, ptr);
}

ChannelPtr Factory::create_channel(const std::string &name)
{
    ChannelPtr ret(_channel_factory.create_instance(name));
    return ret;
}

FormatterPtr Factory::create_formatter(const std::string &name)
{
    FormatterPtr ret(_formatter_factory.create_instance(name));
    return ret;
}

void Factory::register_buildin()
{

}
} //namespace log
} //namespace walle