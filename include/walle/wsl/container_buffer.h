#ifndef WALLE_WSL_CONTAINER_BUFFER_H_
#define WALLE_WSL_CONTAINER_BUFFER_H_
#include <walle/wsl/internal/basic_buffer.h>
namespace wsl {

template <typename Container>
class container_buffer : public wsl::internal::basic_buffer<typename Container::value_type> {
private:
    Container &_container;

protected:
    void grow(std::size_t capacity) WALLE_OVERRIDE 
    {
        _container.resize(capacity);
        this->set(&_container[0], capacity);
    }

public:
    explicit container_buffer(Container &c)
    : basic_buffer<typename Container::value_type>(&c[0], c.size(), c.size()),
      _container(c) 
    {

    }
};

}
#endif //WALLE_WSL_CONTAINER_BUFFER_H_