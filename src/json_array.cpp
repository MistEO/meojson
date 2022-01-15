#include "json_array.h"

#include "json_value.h"

json::array::array(const raw_array &arr) : _array_data(arr) { ; }

json::array::array(raw_array &&arr) noexcept : _array_data(std::move(arr)) { ; }

json::array::array(std::initializer_list<raw_array::value_type> init_list)
    : _array_data(init_list) {
  ;
}

template <typename ArrayType> json::array::array(ArrayType arr) {
  static_assert(
      std::is_constructible<json::value, typename ArrayType::value_type>::value,
      "Parameter can't be used to construct a json::value");
  for (auto &&ele : arr) {
    _array_data.emplace_back(std::move(ele));
  }
}

const json::value &json::array::at(size_t pos) const {
  return _array_data.at(pos);
}

void json::array::clear() noexcept { _array_data.clear(); }

const std::string json::array::to_string() const {
  std::string str = "[";
  for (const json::value &val : _array_data) {
    str += val.to_string() + ",";
  }
  if (str.back() == ',') {
    str.pop_back();
  }
  str += "]";
  return str;
}

const std::string json::array::format(std::string shift_str,
                                      size_t basic_shift_count) const {
  std::string shift;
  for (size_t i = 0; i != basic_shift_count + 1; ++i) {
    shift += shift_str;
  }

  std::string str = "[";
  for (const json::value &val : _array_data) {
    str += "\n" + shift + val.format(shift_str, basic_shift_count + 1) + ",";
  }
  if (str.back() == ',') {
    str.pop_back(); // pop last ','
  }

  str += '\n';
  for (size_t i = 0; i != basic_shift_count; ++i) {
    str += shift_str;
  }
  str += ']';
  return str;
}

const bool json::array::get(size_t pos, bool default_value) const {
  if (exist(pos)) {
    json::value value = _array_data.at(pos);
    if (value.is_boolean()) {
      return value.as_boolean();
    } else {
      return default_value;
    }
  } else {
    return default_value;
  }
}

const int json::array::get(size_t pos, int default_value) const {
  if (exist(pos)) {
    json::value value = _array_data.at(pos);
    if (value.is_number()) {
      return value.as_integer();
    } else {
      return default_value;
    }
  } else {
    return default_value;
  }
}

const long json::array::get(size_t pos, long default_value) const {
  if (exist(pos)) {
    json::value value = _array_data.at(pos);
    if (value.is_number()) {
      return value.as_long();
    } else {
      return default_value;
    }
  } else {
    return default_value;
  }
}

const unsigned long json::array::get(size_t pos, unsigned default_value) const {
  if (exist(pos)) {
    json::value value = _array_data.at(pos);
    if (value.is_number()) {
      return value.as_unsigned_long();
    } else {
      return default_value;
    }
  } else {
    return default_value;
  }
}

const long long json::array::get(size_t pos, long long default_value) const {
  if (exist(pos)) {
    json::value value = _array_data.at(pos);
    if (value.is_number()) {
      return value.as_long_long();
    } else {
      return default_value;
    }
  } else {
    return default_value;
  }
}

const unsigned long long
json::array::get(size_t pos, unsigned long long default_value) const {
  if (exist(pos)) {
    json::value value = _array_data.at(pos);
    if (value.is_number()) {
      return value.as_unsigned_long_long();
    } else {
      return default_value;
    }
  } else {
    return default_value;
  }
}

const float json::array::get(size_t pos, float default_value) const {
  if (exist(pos)) {
    json::value value = _array_data.at(pos);
    if (value.is_number()) {
      return value.as_float();
    } else {
      return default_value;
    }
  } else {
    return default_value;
  }
}

const double json::array::get(size_t pos, double default_value) const {
  if (exist(pos)) {
    json::value value = _array_data.at(pos);
    if (value.is_number()) {
      return value.as_double();
    } else {
      return default_value;
    }
  } else {
    return default_value;
  }
}

const long double json::array::get(size_t pos,
                                   long double default_value) const {
  if (exist(pos)) {
    json::value value = _array_data.at(pos);
    if (value.is_number()) {
      return value.as_long_double();
    } else {
      return default_value;
    }
  } else {
    return default_value;
  }
}

const std::string json::array::get(size_t pos,
                                   std::string default_value) const {
  if (exist(pos)) {
    json::value value = _array_data.at(pos);
    if (value.is_string()) {
      return value.as_string();
    } else {
      return default_value;
    }
  } else {
    return default_value;
  }
}

const std::string json::array::get(size_t pos,
                                   const char *default_value) const {
  if (exist(pos)) {
    json::value value = _array_data.at(pos);
    if (value.is_string()) {
      return value.as_string();
    } else {
      return default_value;
    }
  } else {
    return default_value;
  }
}

json::array::iterator json::array::begin() noexcept {
  return _array_data.begin();
}

json::array::iterator json::array::end() noexcept { return _array_data.end(); }

json::array::const_iterator json::array::begin() const noexcept {
  return _array_data.begin();
}

json::array::const_iterator json::array::end() const noexcept {
  return _array_data.end();
}

json::array::const_iterator json::array::cbegin() const noexcept {
  return _array_data.cbegin();
}

json::array::const_iterator json::array::cend() const noexcept {
  return _array_data.cend();
}

json::array::reverse_iterator json::array::rbegin() noexcept {
  return _array_data.rbegin();
}
json::array::reverse_iterator json::array::rend() noexcept {
  return _array_data.rend();
}

json::array::const_reverse_iterator json::array::rbegin() const noexcept {
  return _array_data.rbegin();
}
json::array::const_reverse_iterator json::array::rend() const noexcept {
  return _array_data.rend();
}

json::array::const_reverse_iterator json::array::crbegin() const noexcept {
  return _array_data.crbegin();
}

json::array::const_reverse_iterator json::array::crend() const noexcept {
  return _array_data.crend();
}

json::value &json::array::operator[](size_t pos) { return _array_data[pos]; }

const json::value &json::array::operator[](size_t pos) const {
  return _array_data[pos];
}

// const raw_array &json::array::raw_data() const
// {
//     return _array_data;
// }

std::ostream &operator<<(std::ostream &out, const json::array &arr) {
  // TODO: format output

  out << arr.to_string();
  return out;
}