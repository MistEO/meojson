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
    constexpr constexpr_variant() = default;

    constexpr constexpr_variant(const constexpr_variant& v) : _data(v._data), _index(v._index) {}

    constexpr constexpr_variant(constexpr_variant&& v) : _data(std::move(v._data)), _index(v._index) {}

    constexpr constexpr_variant& operator=(const constexpr_variant& v)
    {
        _index = v._index;
        _data = v._data;
        return *this;
    }

    constexpr constexpr_variant& operator=(constexpr_variant&& v)
    {
        _index = v._index;
        _data = std::move(v._data);
        return *this;
    }

    constexpr ~constexpr_variant() = default;

    template <typename elem_t>
    requires contains_type<elem_t, elems_t...>
    constexpr constexpr_variant(elem_t&& elem)
    {
        constexpr size_t index = index_of<elem_t>();
        std::get<index>(_data) = std::forward<elem_t&&>(elem);
        _index = index;
    }

    template <typename elem_t>
    requires contains_type<elem_t, elems_t...>
    constexpr constexpr_variant& operator=(elem_t&& elem)
    {
        constexpr size_t index = index_of<elem_t>();
        std::get<index>(_data) = std::forward<elem_t&&>(elem);
        _index = index;
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
        if (_index == index) {
            return &std::get<index>(_data);
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
        if (_index == index) {
            return std::get<index>(_data);
        }
        else {
            throw std::bad_variant_access();
        }
    }

private:
    std::tuple<elems_t...> _data;
    size_t _index = 0;
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
