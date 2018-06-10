#include <walle/log/channel/console_channel.h>
#include <walle/log/channel/tee_channel.h>
#include <walle/log/channel/simple_file_channel.h>
#include <walle/log/pattern_formatter.h>
#include <walle/log/formatter_channel.h>
#include <walle/format/printf.h>
#include <walle/log/logger.h>
#include <google/gtest/gtest.h>
#include <cstdlib>
#include <iostream>

using namespace walle::log;

TEST(log, console_channel)
{
 
    ChannelPtr ptr(new ConsoleChannel());
    Message msg;
    msg.formatted<<"hello";
    ptr->log(msg);
    
}
void set_msg(Message &msg)
{
    msg.log_name = walle::StringRef("test_log");
    msg.tid = 123;
    msg.thread_name =  walle::StringRef("test_tread");
    msg.pid = 3121;
    msg.msg_id = 456;
}
TEST(log, pattern)
{
    FormatterChannel *pcl = new FormatterChannel();
    ChannelPtr ptr(pcl);
    SingleTeeChannel *tcptr = new SingleTeeChannel();
    ChannelPtr console_ptr(new ConsoleChannel());
    tcptr->add_channel(console_ptr);
    //tcptr->add_channel(console_ptr);
    pcl->set_channel(ChannelPtr(tcptr));
    FormatterPtr fptr(new PatternFormatter());
    fptr->set_property("format", "[V:%V] [s:%s] [l:%l][p:%p] [q:%q] [P:%P] [T:%T] [I:%I]"
                                    " [N:%N] [U:%U] [u:%u] [w:%w] [W:%W]"
                        "[b:%b] [B:%B] [d:%d][e:%e] [f:%f] [m:%m][n:%n][o:%o]"
                        " [y:%y] [Y:%Y] [H:%H] [h:%h] [a:%a] [A:%A] [M:%M] [S:%S] [i:%i] [F:%F]"
                        "[z:%z] [Z:%z] [%v]");
    pcl->set_formatter(fptr);
    Message msg; 
    set_msg(msg);
    msg.raw<<"hello";
    ptr->log(msg);

}

TEST(log, logger)
{
    FormatterChannel *pcl = new FormatterChannel();
    ChannelPtr ptr(pcl);
    SingleTeeChannel *tcptr = new SingleTeeChannel();
    ChannelPtr console_ptr(new ConsoleChannel());
    ChannelPtr file_ptr(new MtSimpleFileChannel("./log"));
    tcptr->add_channel(console_ptr);
    tcptr->add_channel(file_ptr);
    pcl->set_channel(ChannelPtr(tcptr));
    FormatterPtr fptr(new PatternFormatter());
    fptr->set_property("format", "%p [%v]");
    pcl->set_formatter(fptr);
    LoggerPtr plog(new Logger("test_log", ptr, Level::eInfo));
    plog->info(__FILE__, __LINE__,  "%s %d", "info hello", 123);
    plog->warn(__FILE__, __LINE__, "%s", "info hello");
    plog->error(__FILE__, __LINE__, "%s", "info hello");
    plog->debug(__FILE__, __LINE__, "%s", "this should not display");
}