#pragma once

#include <string>
#include <unordered_map>
#include <initializer_list>

#include "json_value.h"

namespace json
{
    class object
    {
    public:
        using raw_object = std::unordered_map<std::string, value>;
        using iterator = raw_object::iterator;
        using const_iterator = raw_object::const_iterator;

        object() = default;
        object(const object &rhs) = default;
        object(object &&rhs) = default;
        object(const raw_object &raw_obj);
        object(raw_object &&raw_obj);
        object(std::initializer_list<raw_object::value_type> init_list);

        ~object() = default;

        bool empty() const noexcept { return _object_data.empty(); }
        size_t size() const noexcept { return _object_data.size(); }
        const value &at(const std::string &key) const;
        std::string to_string() const;
        std::string format(std::string shift_str = "    ", size_t basic_shift_count = 0) const;

        void clear() noexcept;
        bool insert(const std::string &key, const value &val);
        bool insert(std::string &&key, value &&val);
        bool emplace(const std::string &key, const value &val);
        bool emplace(std::string &&key, value &&val);
        bool earse(const std::string &key);

        iterator begin() noexcept;
        iterator end() noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        value &operator[](const std::string &key);
        value &operator[](std::string &&key);

        object &operator=(const object &) = default;
        object &operator=(object &&) = default;

        // const raw_object &raw_data() const;

    private:
        raw_object _object_data;
    };

    std::ostream &operator<<(std::ostream &out, const json::object &obj);

} // namespace json