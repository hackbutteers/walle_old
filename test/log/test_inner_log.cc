#include <walle/common/inner_log.h>
#include <google/gtest/gtest.h>
#include <cstdlib>

TEST(log, inner_log)
{
    (GET_LOGGER)->set_level(walle::log::Level::eInfo);
    LOG_TRACE("%s %d", "123 this should not display", 7);
    LOG_DEBUG("%s %d", "123 this should not display", 7);
    LOG_INFO("%s %d", "123", 7);
    LOG_WARN("%s %d", "123", 7);
    LOG_ERROR("%s %d", "123", 7);
    LOG_CRITICAL("%s %d", "123", 7);
    LOG_FATAL("%s [%d]", "123", 7);


}
