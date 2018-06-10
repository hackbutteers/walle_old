#ifndef WALLE_LOG_CHANNEL_TEE_CHANNEL_H_
#define WALLE_LOG_CHANNEL_TEE_CHANNEL_H_
#include <walle/log/channel.h>
#include <walle/sys/mutex.h>
#include <vector>

namespace walle {

namespace log {

template <typename MUTEX>
class TeeChannel : public Channel{
public:
    TeeChannel():_channels(), _mutex(){}
    ~TeeChannel(){}
    void add_channel(const ChannelPtr &channel)
    {
        WALLE_ASSERT(channel, "null");
        walle::UniqueLock<MUTEX> lock(_mutex);
        _channels.push_back(channel);
    }

    void remove_channel(const ChannelPtr &channel)
    {
        walle::UniqueLock<MUTEX> lock(_mutex);
        ChannelVec::iterator it = _channels.begin();
        for(; it != _channels.end(); it++) {
            if(channel == *it) {
                _channels.erase(it);
            }
        }
    }

    void close()
    {
        walle::UniqueLock<MUTEX> lock(_mutex);
        _channels.clear();
    }

    int count() const
    {
        walle::UniqueLock<MUTEX> lock(_mutex);
        return _channels.size();
    }

    void log(MessagePtr &msg)
    {
        log(*msg);
    }
    void flush()
    {

    }
    void log(Message &msg)
    {
         walle::UniqueLock<MUTEX> lock(_mutex);
        ChannelVec::iterator it = _channels.begin();
        for(; it != _channels.end(); it++) {
            (*it)->log(msg);
        }
    }
private:
    typedef std::vector<ChannelPtr> ChannelVec;

    ChannelVec      _channels;
    MUTEX           _mutex;
};
/*
WALLE_FORCE_INLINE TeeChannel::TeeChannel()
{

}
WALLE_FORCE_INLINE TeeChannel::~TeeChannel()
{

}

template <typename MUTEX>
WALLE_FORCE_INLINE int TeeChannel::count() const
{
    walle::UniqueLock<Mutex> lock(_mutex);
    return _channels.size();
}

WALLE_FORCE_INLINE void TeeChannel::close()
{
    walle::UniqueLock<Mutex> lock(_mutex);
    _channel.clear();
}

WALLE_FORCE_INLINE void TeeChannel::add_channel(const ChannelPtr &channel)
{
    WALLE_ASSERT(channel, "null");
    walle::UniqueLock<Mutex> lock(_mutex);
    _channel.push_back(channel);
}

WALLE_FORCE_INLINE void TeeChannel::log(Message &msg)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    ChannelVec::iterator it = _channel.begin();
    for(; it != _channel.end(); it++) {
        (*it)->log(msg);
    }
}

WALLE_FORCE_INLINE void TeeChannel::remove_channel(const ChannelPtr &channel)
{
    walle::UniqueLock<Mutex> lock(_mutex);
    ChannelVec::iterator it = _channel.begin();
    for(; it != _channel.end(); it++) {
        if(channel == *it) {
            _channel.erase(it);
        }
    }
}
*/
typedef TeeChannel<walle::Mutex> MultiTeeChannel;
typedef TeeChannel<walle::NullMutex> SingleTeeChannel;

} //namespace log
} //namespace walle
#endif //WALLE_LOG_CHANNEL_TEE_CHANNEL_H_