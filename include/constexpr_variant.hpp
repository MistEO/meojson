#pragma once

#include <tuple>

namespace json
{

template <typename T, typename... elems_t>
concept contains_type = (std::same_as<T, elems_t> || ...);

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

    constexpr size_t index() const { return _index; }

    constexpr const std::tuple<elems_t...>& data() const { return _data; }

private:
    std::tuple<elems_t...> _data;
    size_t _index = 0;
};

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
#endif

template <typename tuple_t, std::size_t... Is>
constexpr bool compare_at_indexs(const tuple_t& t1, const tuple_t& t2, std::size_t index, std::index_sequence<Is...>)
{
    bool result = false;
    (..., (Is == index ? (result = (std::get<Is>(t1) == std::get<Is>(t2)), true) : false));
    return result;
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

template <typename... elems_t>
constexpr bool compare_at_index(const std::tuple<elems_t...>& t1, const std::tuple<elems_t...>& t2, std::size_t index)
{
    return compare_at_indexs(t1, t2, index, std::index_sequence_for<elems_t...> {});
}

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

template <typename... elems_t>
constexpr bool operator==(const json::constexpr_variant<elems_t...>& x, const json::constexpr_variant<elems_t...>& y)
{
    if (x.index() != y.index()) {
        return false;
    }
    return compare_at_index(x.data(), y.data(), x.index());
}

template <typename... elems_t>
constexpr bool operator!=(const json::constexpr_variant<elems_t...>& x, const json::constexpr_variant<elems_t...>& y)
{
    return !(x == y);
}
