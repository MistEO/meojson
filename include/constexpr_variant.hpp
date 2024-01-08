#pragma once

#include <tuple>

namespace json
{

template <typename T, typename... Types>
concept contains_type = (std::same_as<T, Types> || ...);

template <typename... elems_t>
class constexpr_variant
{
public:
    constexpr constexpr_variant() { __index = 0; }

    constexpr constexpr_variant(const constexpr_variant& v) : __data(v.__data) { __index = v.__index; }

    constexpr constexpr_variant(constexpr_variant&& v) : __data(std::move(v.__data)) { __index = v.__index; }

    constexpr constexpr_variant& operator=(const constexpr_variant& v)
    {
        __index = v.__index;
        __data = v.__data;
        return *this;
    }

    constexpr constexpr_variant& operator=(constexpr_variant&& v)
    {
        __index = v.__index;
        __data = std::move(v.__data);
        return *this;
    }

    constexpr ~constexpr_variant() = default;

    template <typename elem_t>
    requires contains_type<elem_t, elems_t...>
    constexpr constexpr_variant(elem_t&& elem)
    {
        constexpr size_t index = index_of<elem_t>();
        std::get<index>(__data) = std::move(elem);
        __index = index;
    }

    template <typename elem_t>
    requires contains_type<elem_t, elems_t...>
    constexpr constexpr_variant& operator=(elem_t&& elem)
    {
        constexpr size_t index = index_of<elem_t>();
        std::get<index>(__data) = std::move(elem);
        __index = index;
        return *this;
    }

    template <typename elem_t>
    requires contains_type<elem_t, elems_t...>
    constexpr static size_t index_of()
    {
        constexpr size_t N = sizeof...(elems_t);
        constexpr bool found[] = { std::is_same_v<elem_t, elems_t>... };
        constexpr size_t index = ([&] {
            size_t index = N;
            for (size_t i = 0; i < N; ++i) {
                if (found[i]) {
                    index = i;
                    break;
                }
            }
            return index;
        })();
        static_assert(index != N, "error!");
        return index;
    }

    template <typename elem_t>
    requires contains_type<elem_t, elems_t...>
    constexpr const elem_t* get_if() const
    {
        constexpr size_t index = index_of<elem_t>();
        if (__index == index) {
            return &std::get<index>(__data);
        }
        else {
            return nullptr;
        }
    }

    template <typename elem_t>
    requires contains_type<elem_t, elems_t...>
    constexpr const elem_t& get() const
    {
        constexpr size_t index = index_of<elem_t>();
        if (__index == index) {
            return std::get<index>(__data);
        }
        else {
            throw std::bad_variant_access();
        }
    }

private:
    std::tuple<elems_t...> __data;
    size_t __index;
};

} // namespace json

namespace std
{

template <typename elem_t, typename... elems_t>
requires json::contains_type<elem_t, elems_t...>
constexpr const elem_t* get_if(const json::constexpr_variant<elems_t...>* v)
{
    return v->template get_if<elem_t>();
}

template <typename elem_t, typename... elems_t>
requires json::contains_type<elem_t, elems_t...>
constexpr const elem_t& get(const json::constexpr_variant<elems_t...>& v)
{
    return v.template get<elem_t>();
}

}
