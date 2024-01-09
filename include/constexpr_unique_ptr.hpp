#pragma once

#include <utility>

namespace json
{

template <typename elem_t>
class constexpr_unique_ptr
{
public:
    constexpr void reset()
    {
        delete _data;
        _data = nullptr;
    }

    constexpr constexpr_unique_ptr() = default;
    constexpr constexpr_unique_ptr(elem_t* p) : _data(p) {}

    constexpr constexpr_unique_ptr(const constexpr_unique_ptr&) = delete;
    constexpr constexpr_unique_ptr(constexpr_unique_ptr&& another) : _data(std::exchange(another._data, nullptr)) {}

    constexpr constexpr_unique_ptr& operator=(const constexpr_unique_ptr&) = delete;
    constexpr constexpr_unique_ptr& operator=(constexpr_unique_ptr&& another)
    {
        if (this == &another) {
            return *this;
        }
        reset();
        _data = std::exchange(another._data, nullptr);
        return *this;
    }

    constexpr ~constexpr_unique_ptr() { reset(); }

    constexpr elem_t* operator->() const { return _data; }
    constexpr elem_t& operator*() const { return *_data; }
    constexpr elem_t* get() const { return _data; }

private:
    elem_t* _data = nullptr;
};

}