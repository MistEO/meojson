<div align="center">

# meojson

现代化的全平台 Json/Json5 解析/生成器，Header-only，并且使用了魔法！

A modern all-platform Json/Json5 parser/serializer, which is header-only and used magic!

</div>

[English](./README_en.md)

## 使用说明

- 在您的项目中包含头文件即可使用  

```c++
#include "json.hpp"
```

- 若您需要解析 Json5, 则请包含 `json5.hpp` 头文件

```c++
#include "json5.hpp"
```

- **meojson** 仅依赖 STL, 但需要 c++17 标准
- 若使用 MSVC，请在项目中添加 `/Zc:preprocessor`
- 若使用 AppleClang，请在项目中添加 `-Wno-gnu-zero-variadic-macro-arguments`

## 序列化

以下是一些基本特性：

```c++
json::value j;
j["pi"] = 3.14;
j["happy"] = true;
j["answer"]["everything"] = 42;
j["object"] = { {"currency", "USD"}, {"value", 42.99} };
```

以及一些有趣的特性：

```c++
std::set<int> set { 1, 2, 3 };
j["set"] = set;

// 什么鬼类型！
std::unordered_map<std::string, std::list<std::map<std::string, std::deque<int>>>> map {
    { "key_1", { { { "inner_key_1", { 7, 8, 9 } } }, { { "inner_key_2", { 10 } } } } },
};
j["map"] = map;

// output:
// {"answer":{"everything":42},"happy":true,"map":{"key_1":[{"inner_key_1":[7,8,9]},{"inner_key_2":[10]}]},"object":{"currency":"USD","value":42.990000},"pi":3.140000,"set":[1,2,3]}
std::cout << j << std::endl;
```

别眨眼，我们又转回来了！

```c++
double pi = (double)j["pi"];
int answer = (int)j["answer"]["everything"];

std::set<int> new_set = (std::set<int>)j["set"];
// 又是这个鬼类型
auto new_map = (std::unordered_map<std::string, std::list<std::map<std::string, std::deque<int>>>>)j["map"];
```

然而对于运行时的 JSON，最好先检查它是否可以转换。

```c++
if (j["happy"].is<std::vector<int>>()) {
    std::vector<int> vec = (std::vector<int>)j["happy"];
}
else {
    std::cout << "天啊, j[\"happy\"] 不是一个数组！" << std::endl;
    std::cout << "还好我检查了，不然就炸了！" << std::endl;
}
```

我猜你已经明白了，是的，**meojson** 不仅仅是一个 JSON 库，还是一个序列化库！

```c++
struct MyStruct
{
    int x = 0;
    std::vector<double> vec;
    // 怎么总是你！
    std::unordered_map<std::string, std::list<std::map<std::string, std::deque<int>>>> map;

    // 让我们加点魔法
    MEO_JSONIZATION(x, vec, map);
};

MyStruct mine;
mine.vec.emplace_back(0.5);
mine.map = { { "key_1", { { { "inner_key_1", { 7, 8, 9 } } }, { { "inner_key_2", { 10 } } } } } };

// 是的，它是那么直观和流畅！
json::value j_mine = mine;

// output: {"map":{"key_1":[{"inner_key_1":[7,8,9]},{"inner_key_2":[10]}]},"vec":[0.500000],"x":0}
std::cout << j_mine << std::endl;

// 恰恰，我们也可以把它转回来！
MyStruct new_mine = (MyStruct)j_mine;
```

嵌套调用也是易如反掌！

```c++
struct Outter
{
    int outter_a = 10;
    std::vector<MyStruct> my_vec;

    MEO_JSONIZATION(outter_a, my_vec);
};

Outter outter;
outter.my_vec.emplace_back(mine);

json::value j_outter = outter;

// output:
// {"my_vec":[{"map":{"key_1":[{"inner_key_1":[7,8,9]},{"inner_key_2":[10]}]},"vec":[0.500000],"x":0}],"outter_a":10}
std::cout << j_outter.to_string() << std::endl;

// 同样的反序列化
Outter new_o = (Outter)j_outter;
```

对于可选字段，我们可以在其中添加 `MEO_OPT`，这样在转换时，如果此字段在 JSON 中不存在，它将被跳过。

```c++
struct OptionalFields
{
    int a = 0;
    double b = 0;
    std::vector<int> c;

    MEO_JSONIZATION(a, MEO_OPT b, MEO_OPT c);
};

json::value ja = {
    { "a", 100 },
};
if (ja.is<OptionalFields>()) {
    OptionalFields var = (OptionalFields)ja;
    // output: 100
    std::cout << var.a << std::endl;
}
```

对于第三方不可侵入的类型，则需要实现 `to_json`, `check_json`, `from_json`

```c++
struct ThirdPartyStruct
{
    int a = 100;
};

namespace json
{
template <>
class serialization<ThirdPartyStruct>
{
public:
    json::value to_json(const ThirdPartyStruct& t) const { return t.a; }
    bool check_json(const json::value& j) const { return j.is_number(); }
    bool from_json(const json::value& j, ThirdPartyStruct& out) const
    {
        out.a = j.as_integer();
        return true;
    }
};
} // namespace json

// 然后可以将其用作 JSON
ThirdPartyStruct third;
json::value jthird = third;
ThirdPartyStruct new_third = (ThirdPartyStruct)jthird;

// 或者添加到结构中
struct Outter2
{
    int outter_a = 10;
    ThirdPartyStruct third;

    MEO_JSONIZATION(outter_a, my_vec, third);
};
```

如果你不喜欢愚蠢的侵入式函数，也可以使用 `json::serialize` 和 `json::deserialize` 进行更优雅的转换:

```c++
struct Serializer
{
    json::value operator()(const ThirdPartyStruct& t) const { return t.a; }
};
struct Deserializer
{
    bool operator()(const json::value& j, ThirdPartyStruct& t) const
    {
        if (!j.is_number()) return false;
        t.a = j.as_integer();
        return true;
    }
};

std::map<std::string, ThirdPartyStruct> third;
third["key"] = { 100 };
json::value jthird = json::serialize(third, Serializer {});

std::cout << jthird << std::endl;

std::map<std::string, ThirdPartyStruct> new_third;
bool ret = json::deserialize(jthird, new_third, Deserializer {});
```

还有一些琐碎的特性：

```c++
// 通过 `emplace` 向数组或对象添加元素
j["set"].emplace(10);
j["object"].emplace("key3", "value3");

// 合并两个数组
j["set"] += json::array { 11, 12 };

// 合并两个对象
j["object"] |= {
    { "key4", 4 },
    { "key5", false },
};

// 转为字符串
std::string oneline = j.dumps();
std::string format = j.dumps(4);

// 保存到文件
std::ofstream ofs("meo.json");
ofs << j;
ofs.close();

```

## 解析

现在让我们谈谈解析

```c++
std::string content = R"(
{
    "repo": "meojson",
    "author": {
        "MistEO": "https://github.com/MistEO",
        "ChingCdesu": "https://github.com/ChingCdesu"
    },
    "list": [ 1, 2, 3 ],
    "str": "abc\n123",
    "num": 3.1416,
    "A_obj": {
        "B_arr": [
            { "C_str": "i am a distraction" },
            { "C_str": "you found me!" }
        ]
    }
})";

// 它是一个 std::optional<json::value>
auto ret = json::parse(content);

if (!ret) {
    std::cerr << "解析失败" << std::endl;
    return;
}
json::value& value = *ret;

// Output: meojson
std::cout << (std::string)value["repo"] << std::endl;

/* Output:
    ChingCdesu's homepage: https://github.com/ChingCdesu
    MistEO's homepage: https://github.com/MistEO
*/
for (auto&& [name, homepage] : (json::object)value["author"]) {
    std::cout << name << "'s homepage: " << (std::string)homepage << std::endl;
}
// num = 3.141600
double num = (double)value["num"];

// get_value = "default_value"
std::string get_value = value.get("maybe_exists", "default_value");
std::cout << get_value << std::endl;
```

和大多数解析库一样，很无聊，你肯定不想看这个。  
所以让我给你看点有趣的东西：

```c++
// 多么神奇的 `get`，你可以连续传参 key 或 pos！
// nested_get = you found me!
std::string nested_get = value.get("A_obj", "B_arr", 1, "C_str", "default_value");

// `find` 可以帮助你找到并检查类型是否正确
// 如果没有 `num`，则 opt_n 将为 std::nullopt
auto opt_n = value.find<double>("num");
if (opt_n) {
    // 输出: 3.141600
    std::cout << *opt_n << std::endl;
}

```

还有一些你在序列化中已经见过的技巧

```c++
bool is_vec = value["list"].is<std::vector<int>>();

std::vector<int> to_vec = value["list"].as_collection<int>();
to_vec = (std::vector<int>)value["list"];       // 与上面相同
to_vec = value["list"].as<std::vector<int>>();  // 与上面相同

// 输出: 1, 2, 3
for (auto&& i : to_vec) {
    std::cout << i << std::endl;
}

std::list<int> to_list = value["list"].as_collection<int, std::list>();
to_list = (std::list<int>)value["list"];        // 与上面相同
to_list = value["list"].as<std::list<int>>();   // 与上面相同

std::set<int> to_set = value["list"].as_collection<int, std::set>();
to_set = (std::set<int>)value["list"];          // 与上面相同
to_set = value["list"].as<std::set<int>>();     // 与上面相同

bool is_map = value["author"].is<std::map<std::string, std::string>>();

std::map<std::string, std::string> to_map = value["author"].as_map<std::string>();
to_map = (std::map<std::string, std::string>)value["author"];       // 与上面相同
to_map = value["author"].as<std::map<std::string, std::string>>();  // 与上面相同

auto to_hashmap = value["author"].as_map<std::string, std::unordered_map>();
to_hashmap = (std::unordered_map<std::string, std::string>)value["author"];     // 与上面相同
to_hashmap = value["author"].as<std::unordered_map<std::string, std::string>>();// 与上面相同
```

以及不知道有啥用的字面语法

```c++
// Output: "literals"
using namespace json::literals;
auto val = "{\"hi\":\"literals\"}"_json;
std::cout << val["hi"] << std::endl;
```

但好消息是，我们也可以解析 JSON5！

```c++
std::string_view content5 = R"(
// 这是一个 Json5 内容
{
  名字: "MistEO",                  /* 键的引号可以省略 */
  😊: '😄',                       // 表情符可以用作键
  thanks: 'ありがとう',             /* 单引号也可以用作字符串 */
  \u006Bey: ['value',],            // 正常字符和转义可以混合使用
  inf: +Infinity, nan: NaN,        // 数字可以以 '+' 开头
  fractional: .3, integer: 42.,    // 允许以小数点开头或结尾
  byte_max: 0xff,                  // 支持十六进制数
  light_speed: +3e8,               // 以及科学计数法
})";

auto ret = json::parse5(content5);
if (!ret) {
    std::cerr << "解析失败" << std::endl;
    return;
}
json::value& value = *ret;

// Output: MistEO
std::cout << value["名字"] << std::endl;
// str = "value"
std::string str = (std::string)value["key"][0];
```
