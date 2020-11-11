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
        typedef std::map<std::string, value> json_object;
        typedef json_object::iterator iterator;
        typedef json_object::const_iterator const_iterator;

        object() = default;
        object(const object &rhs) = default;
        object(object &&rhs) = default;
        object(const json_object &map);
        object(json_object &&map);

        ~object() = default;

        bool empty() const;
        const value at(const std::string &key) const;
        size_t size() const;
        std::string to_string() const;

        void clear();
        bool insert(const std::string &key, const value &value);
        bool insert(std::string &&key, value &&value);
        bool earse(const std::string &key);

        iterator begin();
        iterator end();
        const_iterator cbegin() const;
        const_iterator cend() const;

        value &operator[](const std::string &key);
        const value &operator[](const std::string &key) const;
        object &operator=(const object &) = default;
        object &operator=(object &&) = default;

        // const json_object &raw_data() const;

    private:
        json_object _object_data;
        bool _valid = false;
    };

    std::ostream &operator<<(std::ostream &out, const json::object &object);

} // namespace json