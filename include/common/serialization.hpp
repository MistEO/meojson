#pragma once

#include <type_traits>

#include "types.hpp"
#include "utils.hpp"

namespace json
{
namespace _serialization_helper
{
    template <typename in_t, typename serializer_t>
    class is_serializable
    {
        template <typename U>
        static auto test(int) -> decltype(std::declval<serializer_t>()(std::declval<in_t>()), std::true_type());

        template <typename U>
        static std::false_type test(...);

    public:
        static constexpr bool value = decltype(test<in_t>(0))::value;
    };

    struct empty_serializer
    {
        // sample:
        // json::value operator()(const type_1&) const { return ...; }
        // json::value operator()(const type_2&) const { return ...; }
        // json::value operator()(const type_3&) const { return ...; }
    };

    template <typename T>
    void unable_to_serialize()
    {
        static_assert(!sizeof(T), "Unable to serialize T. "
#ifdef _MSC_VER
                                  "See T below: " __FUNCSIG__
#else
        // "See T below: " __PRETTY_FUNCTION__

#endif
        );
    }
}

namespace _serialization_helper
{
    template <typename out_t, typename deserializer_t, typename string_t = default_string_t>
    class is_deserializable
    {
        template <typename U>
        static auto test(int)
            -> decltype(std::declval<deserializer_t>()(std::declval<basic_value<string_t>>(), std::declval<out_t&>()),
                        std::true_type());

        template <typename U>
        static std::false_type test(...);

    public:
        static constexpr bool value = decltype(test<out_t>(0))::value;
    };

    struct empty_deserializer
    {
        // sample:
        // bool operator()(const json::value&, type_1&) const { return ...; }
        // bool operator()(const json::value&, type_2&) const { return ...; }
        // bool operator()(const json::value&, type_3&) const { return ...; }
    };

    template <typename T>
    void unable_to_deserialize()
    {
        static_assert(!sizeof(T), "Unable to deserialize T. "
#ifdef _MSC_VER
                                  "See T below: " __FUNCSIG__
#else
        // "See T below: " __PRETTY_FUNCTION__

#endif
        );
    }
}

template <typename in_t, typename serializer_t = _serialization_helper::empty_serializer,
          typename string_t = default_string_t>
basic_value<string_t> serialize(in_t&& in, const serializer_t& serializer)
{
    if constexpr (_serialization_helper::is_serializable<in_t, serializer_t>::value) {
        return serializer(std::forward<in_t>(in));
    }
    else if constexpr (std::is_constructible_v<basic_value<string_t>, in_t>) {
        return basic_value<string_t>(std::forward<in_t>(in));
    }
    else if constexpr (_utils::is_collection<std::decay_t<in_t>>) {
        basic_array<string_t> arr;
        for (auto&& elem : in) {
            using elem_t = std::decay_t<decltype(elem)>;

            auto j_elem = serialize<string_t>(std::forward<elem_t>(elem), serializer);
            arr.emplace_back(std::move(j_elem));
        }
        return arr;
    }
    else if constexpr (_utils::is_map<std::decay_t<in_t>>) {
        basic_object<string_t> obj;
        for (auto&& [key, elem] : in) {
            using key_t = std::decay_t<decltype(key)>;
            using elem_t = std::decay_t<decltype(elem)>;

            auto j_elem = serialize<string_t>(std::forward<elem_t>(elem), serializer);
            obj.emplace(std::forward<key_t>(key), std::move(j_elem));
        }
        return obj;
    }
    else {
        _serialization_helper::unable_to_serialize<in_t>();
    }
}

} // namespace json
