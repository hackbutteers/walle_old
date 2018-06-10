#ifndef WALLE_LOG_FORMATTER_CHANNEL_H_
#define WALLE_LOG_FORMATTER_CHANNEL_H_
#include <walle/log/channel.h>
#include <walle/log/formatter.h>

namespace walle {
namespace log {

class FormatterChannel : public Channel {
public:
    FormatterChannel();
    FormatterChannel(FormatterPtr formatter);
    FormatterChannel(FormatterPtr formatter, ChannelPtr channel);
    ~FormatterChannel();
    void set_formatter(FormatterPtr formatter);

    void set_channel(ChannelPtr channel);

    FormatterPtr get_formatter() const;

    ChannelPtr get_channel() const;

    void log(Message &msg);
    void log(MessagePtr &msg);
    void flush();

    bool set_property(const std::string &name, const std::string &value);

    bool get_property(const std::string &name, std::string &value);
    bool open();

    void close();
private:
    FormatterPtr   _formatter;
    ChannelPtr     _channel;
};
} //namespace log
} //namespace walle
#endif