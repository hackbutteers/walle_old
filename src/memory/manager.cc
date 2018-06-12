#include <walle/memory/manager.h>

namespace walle {
namespace mem {
Manager::~Manager()
{
    if(kDebug) {
        return;
    }
}

void Manager::add(size_t cnt)
{
    size_t current = (_total += cnt);
    _peak = std::max(_peak.load(), current);
    ++_alloc_count;
    if (_super) {
        _super->add(cnt);
    }
}
void Manager::sub(size_t cnt)
{
    WALLE_ASSERT(cnt <= _total, "sub size > total");
    _total -= cnt;
    if(_super) {
        _super->sub(cnt);
    }
}

ManagerPtr g_bypass_manager(new Manager(ManagerPtr(), "Bypass"));

}
}