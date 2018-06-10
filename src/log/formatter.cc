#include <walle/config/base.h>
#include <walle/log/formatter.h>

namespace walle {
namespace log {
Formatter::Formatter()
{

}

Formatter::~Formatter()
{

}

bool Formatter::set_property(const std::string& name, const std::string& value)
{
    WALLE_UNUSED(name);
    WALLE_UNUSED(value);
    WALLE_ASSERT(false, "no impl");
}

bool Formatter::get_property(const std::string& name, std::string& value)
{
    WALLE_UNUSED(name);
    WALLE_UNUSED(value);
    WALLE_ASSERT(false, "no impl");
}

} //namespace log
} //namespace walle