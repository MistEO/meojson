#pragma once

#include <tuple>

namespace json
{

template <typename... elems_t>
class constexpr_variant : std::tuple<elems_t...>
{
public:
    constexpr constexpr_variant() = default;
    
    // template <typename = std::enable_if_t<(std::is_copy_constructible_v<elems_t> && ...)>>
    constexpr constexpr_variant(const constexpr_variant& v) : std::tuple<elems_t...>(v)
    {
        __index = v.__index;
    }
    
    constexpr constexpr_variant(constexpr_variant&& v) : std::tuple<elems_t...>(std::move(v)) { __index = v.__index; }

    // template <typename = std::enable_if_t<(std::is_copy_constructible_v<elems_t> && ...)>>
    constexpr constexpr_variant& operator=(const constexpr_variant& v)
    {
        __index = v.__index;
        this->std::tuple<elems_t...>::operator=(v);
        return *this;
    }

    constexpr constexpr_variant& operator=(constexpr_variant&& v)
    {
        __index = v.__index;
        this->std::tuple<elems_t...>::operator=(std::move(v));
        return *this;
    }
    
    constexpr ~constexpr_variant() = default;

    template <typename elem_t>
    constexpr constexpr_variant(elem_t elem)
    {
        constexpr size_t index = index_of<elem_t>();
        std::get<index>(*this) = std::move(elem);
        __index = index;
    }

    template <typename elem_t>
    constexpr constexpr_variant& operator=(elem_t elem)
    {
        constexpr size_t index = index_of<elem_t>();
        std::get<index>(*this) = std::move(elem);
        __index = index;
        return *this;
    }

    template <typename elem_t>
    constexpr static size_t index_of()
    {
        constexpr size_t N = sizeof...(elems_t);
        constexpr bool found[] = { std::is_same_v<elem_t, elems_t>... };
        size_t index = N;
        for (size_t i = 0; i < N; ++i) {
            if (found[i]) {
                index = i;
                break;
            }
        }
        return index;
    }

    template <typename elem_t>
    constexpr const elem_t* get_if() const
    {
        constexpr size_t index = index_of<elem_t>();
        if (__index == index) {
            return &std::get<index>(*this);
        }
        else {
            return nullptr;
        }
    }

    template <typename elem_t>
    constexpr const elem_t& get() const
    {
        constexpr size_t index = index_of<elem_t>();
        if (__index == index) {
            return std::get<index>(*this);
        }
        else {
            throw std::bad_variant_access();
        }
    }

private:
    size_t __index;
};

} // namespace json

namespace std
{

template <typename elem_t, typename... elems_t>
constexpr const elem_t* get_if(const json::constexpr_variant<elems_t...>* v)
{
    return v->template get_if<elem_t>();
}

template <typename elem_t, typename... elems_t>
constexpr const elem_t& get(const json::constexpr_variant<elems_t...>& v)
{
    return v.template get<elem_t>();
}

}
