#pragma once

#include <string>
#include <ostream>
#include <memory>

namespace json
{
    class array;
    class object;

    enum class value_type
    {
        Invalid,
        Null,
        Boolean,
        String,
        Number,
        Array,
        Object,
        NUM_T
    };

    class value
    {
    public:
        value() = default;
        value(const value &rhs) = default;
        value(value &&rhs) = default;
        value(const array &arr);
        value(array &&arr);
        value(const object &obj);
        value(object &&obj);

        ~value() = default;

        bool valid() const noexcept;
        bool empty() const noexcept;
        value_type type() const noexcept;
        bool as_boolean() const;
        int as_integer() const;
        double as_double() const;
        std::string as_string() const;
        array as_array() const;
        object as_object() const;

        std::string to_string() const;

        void set_raw_basic_data(value_type type, const std::string &basic_data);
        void set_raw_basic_data(value_type type, std::string &&basic_data);

        value &operator=(const value &) = default;
        value &operator=(value &&) = default;

        static value null();
        static value boolean(bool b);
        static value number(int num);
        static value number(double num);
        static value string(const char *str);
        static value string(const std::string &str);
        static value string(std::string &&str);

    private:
        value_type _type = value_type::Null;
        std::string _basic_type_data = "null";
        std::shared_ptr<object> _object_ptr = nullptr;
        std::shared_ptr<array> _array_ptr = nullptr;
    };

    std::ostream &operator<<(std::ostream &out, const value &val);

} // namespace json