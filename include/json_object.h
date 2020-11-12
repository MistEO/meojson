#pragma once

#include <string>
#include <map>

namespace json
{
    class value;
    class object
    {
        friend class value;

    public:
        using json_object = std::map<std::string, value>;
        using iterator = json_object::iterator;
        using const_iterator = json_object::const_iterator;

        object() = default;
        object(const object &rhs) = default;
        object(object &&rhs) = default;
        object(const json_object &obj);
        object(json_object &&obj);

        ~object() = default;

        bool empty() const noexcept;
        const value at(const std::string &key) const;
        size_t size() const noexcept;
        std::string to_string() const;

        void clear() noexcept;
        bool insert(const std::string &key, const value &val);
        bool insert(std::string &&key, value &&val);
        bool earse(const std::string &key);

        iterator begin() noexcept;
        iterator end() noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        value &operator[](const std::string &key);
        const value &operator[](const std::string &key) const;
        object &operator=(const object &) = default;
        object &operator=(object &&) = default;

        // const json_object &raw_data() const;

    private:
        json_object _object_data;
        bool _valid = false;
    };

    std::ostream &operator<<(std::ostream &out, const json::object &obj);

} // namespace json