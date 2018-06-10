#ifndef WALLe_LOG_CHANNEL_SIMPLE_FILE_CHANNEL_H_
#define WALLe_LOG_CHANNEL_SIMPLE_FILE_CHANNEL_H_
#include <walle/log/channel.h>
#include <walle/log/message.h>
#include <walle/common/sequence_write_file.h>
#include <walle/sys/mutex.h>
#include <memory>
namespace walle {
namespace log {

template <typename MUTEX>
class SimpleFileChannel :public Channel {
public:
    SimpleFileChannel(const std::string &fileName)
        :_mutex(), _file(new SequenceWriteFile(fileName))
    {
        WALLE_ASSERT(_file, "open failed");
    }

    void log(Message &msg)
    {
        walle::UniqueLock<MUTEX> lock(_mutex);
        _file->append(msg.formatted.data(), msg.formatted.size());
    }
    void log(MessagePtr &msg)
    {
        log(*msg);
    }

    void flush()
    {
        
    }
private:
    MUTEX                              _mutex;
    std::shared_ptr<SequenceWriteFile> _file;
};

typedef SimpleFileChannel<walle::Mutex> MtSimpleFileChannel;
typedef SimpleFileChannel<walle::NullMutex> stSimpleFileChannel;
} //namespace log
} //namespace walle 
#endif