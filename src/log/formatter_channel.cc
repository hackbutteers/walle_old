#include <walle/log/formatter_channel.h>
#include <walle/log/registry.h>

namespace walle {
namespace log {

FormatterChannel::FormatterChannel(FormatterPtr formatter, ChannelPtr channel)
    :_formatter(formatter),
     _channel(channel)
{

}

FormatterChannel::FormatterChannel(FormatterPtr formatter)
    :_formatter(formatter),
     _channel()
{

}

FormatterChannel::FormatterChannel()
{

}

FormatterChannel::~FormatterChannel()
{

}

void FormatterChannel::set_formatter(FormatterPtr formatter)
{
    _formatter = formatter;
}

void FormatterChannel::set_channel(ChannelPtr channel)
{
    _channel = channel;
}

FormatterPtr FormatterChannel::get_formatter() const
{
    return _formatter;
}

ChannelPtr FormatterChannel::get_channel() const
{
    return _channel;
}

void FormatterChannel::log(Message &msg)
{   
    ChannelPtr c = _channel;

    if (c) {
        FormatterPtr f = _formatter;
        if(f) {
            f->format(msg);
        }
        c->log(msg);
    }
}

void FormatterChannel::log(MessagePtr &msg)
{
    log(*msg);
}
void FormatterChannel::flush()
{

}

bool FormatterChannel::set_property(const std::string &name, const std::string &value)
{
    if (name == "channel") {
        ChannelPtr ptr = Registry::get_instance()->get_channel(value);
        if (ptr) {
            _channel = ptr;
            return true;
        }
        return false;
    } else if (name == "formatter") {
        FormatterPtr ptr = Registry::get_instance()->get_formatter(value);
        if (ptr) {
            _formatter = ptr;
            return true;
        }
        return false;
    }
    return false;
}

bool FormatterChannel::get_property(const std::string &name, std::string &value)
{
    return false;
}
bool FormatterChannel::open()
{
    if(_channel) {
        return _channel->open();
    }
    return true;
}

void FormatterChannel::close()
{
    if(_channel) {
        _channel->close();
    }
}

}
}