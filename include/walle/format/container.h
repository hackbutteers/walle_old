#ifndef WALLE_FORMAT_CONTAINER_H_
#define WALLE_FORMAT_CONTAINER_H_

#include <walle/format/format.h>

namespace walle {

namespace internal {

/**
  A "buffer" that appends data to a standard container (e.g. typically a
  ``std::vector`` or ``std::basic_string``).
 */
template <typename Container>
class ContainerBuffer : public Buffer<typename Container::value_type> {
 private:
  Container& container_;

 protected:
  virtual void grow(std::size_t size) FMT_OVERRIDE {
    container_.resize(size);
    this->ptr_ = &container_[0];
    this->capacity_ = size;
  }

 public:
  explicit ContainerBuffer(Container& container) : container_(container) {
    this->size_ = container_.size();
    if (this->size_ > 0) {
      this->ptr_ = &container_[0];
      this->capacity_ = this->size_;
    }
  }
};
}  // namespace internal

/**
  This class template provides operations for formatting and appending data
  to a standard *container* like ``std::vector`` or ``std::basic_string``.

  **Example**::

    void vecformat(std::vector<char>& dest, walle::BasicCStringRef<char> format,
                   walle::ArgList args) {
      walle::BasicContainerWriter<std::vector<char> > appender(dest);
      appender.write(format, args);
    }
    FMT_VARIADIC(void, vecformat, std::vector<char>&,
                 walle::BasicCStringRef<char>);
 */
template <class Container>
class BasicContainerWriter
  : public BasicWriter<typename Container::value_type> {
 private:
  internal::ContainerBuffer<Container> buffer_;

 public:
  /**
    Constructs a :class:`walle::BasicContainerWriter` object.
   */
  explicit BasicContainerWriter(Container& dest)
  : BasicWriter<typename Container::value_type>(buffer_), buffer_(dest) {}
};

} // namespace walle

#endif  // WALLE_FORMAT_CONTAINER_H_
