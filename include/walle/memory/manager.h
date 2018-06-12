#ifndef WALLE_MEMORY_MANAGER_H_
#define WALLE_MEMORY_MANAGER_H_
#include <walle/config/base.h>
#include <algorithm>
#include <atomic>
#include <memory>
#include <string>

namespace walle {
namespace mem {
class Manager;
typedef std::shared_ptr<Manager> ManagerPtr;

class Manager {
public:
    explicit Manager(const ManagerPtr &supuer, const std::string &name);
    ~Manager();

    ManagerPtr super();

    size_t total() const;

    void add(size_t cnt);


    void sub(size_t cnt);

private:
    static const bool kDebug = false;
private:
    ManagerPtr              _super;
    std::string             _name;
    std::atomic<size_t>     _total;
    std::atomic<size_t>     _peak;
    std::atomic<size_t>     _alloc_count;

};

WALLE_FORCE_INLINE Manager::Manager(const ManagerPtr &supermanager, const std::string &name)
    :_super(supermanager),
     _name(name)
{

}

WALLE_FORCE_INLINE  ManagerPtr Manager::super()
{
    return _super;
}
WALLE_FORCE_INLINE size_t Manager::total() const
{
    return _total;
}

}
} //bamespace walle
#endif //WALLE_MEMORY_MANAGER_H_