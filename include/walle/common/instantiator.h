#ifndef WALLE_COMMON_INSTANTIATOR_H_
#define WALLE_COMMON_INSTANTIATOR_H_
#include <walle/config/base.h>

namespace walle {

template <typename BASE>
class AbstractInstantiator {
public:
    AbstractInstantiator()
    {

    }
    virtual ~AbstractInstantiator()
    {

    }

    virtual BASE* create_instance() const = 0;
private:
    WALLE_NON_COPYABLE(AbstractInstantiator);
};

template <typename C, typename BASE>
class Instantiator : public AbstractInstantiator<BASE> {
public:
    Instantiator()
    {

    }
    virtual ~Instantiator()
    {

    }
    BASE* create_instance() const
    {
        return new C;
    }

};

} //namespace walle
#endif //WALLE_COMMON_INSTANTIATOR_H_