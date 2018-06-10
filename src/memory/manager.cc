#include <walle/memory/manager.h>

namespace walle {
namespace mem {
Manager::~Manager()
{
    if(kDebug) {
        return;
    }
}

ManagerPtr g_bypass_manager(new Manager(ManagerPtr(), "Bypass"));

}
}