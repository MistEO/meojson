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
        delete data;
        data = nullptr;
    }

    constexpr constexpr_unique_ptr() = default;
    constexpr constexpr_unique_ptr(elem_t* p) : data(p) {}

    constexpr constexpr_unique_ptr(const constexpr_unique_ptr&) = delete;
    constexpr constexpr_unique_ptr(constexpr_unique_ptr&& another) : data(std::exchange(another.data, nullptr)) {}

    constexpr constexpr_unique_ptr& operator=(const constexpr_unique_ptr&) = delete;
    constexpr constexpr_unique_ptr& operator=(constexpr_unique_ptr&& another)
    {
        if (this == &another) {
            return *this;
        }
        reset();
        data = std::exchange(another.data, nullptr);
        return *this;
    }

    constexpr ~constexpr_unique_ptr() { reset(); }

    constexpr elem_t* operator->() const { return data; }
    constexpr elem_t& operator*() const { return *data; }
    constexpr elem_t* get() const { return data; }

private:
    elem_t* data;
};

}