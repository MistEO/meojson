#include "enum_test.h"
#include "json.hpp"
#include <iostream>

enum class Color
{
    Red = 1,
    Green = 2,
    Blue = 4
};

enum class Direction
{
    Up,
    Down,
    Left,
    Right
};

// Enum with values outside default range (-128 to 128) - reflection might fail if not specialized
// But for now we just test supported range
enum class SmallEnum : char
{
    A = 0,
    B = 1
};

// Enum using MEOJSON_ENUM_RANGE macro to specify custom range
enum class Status
{
    Idle = 100,
    Running = 101,
    Paused = 102,
    Stopped = 103,
    MEOJSON_ENUM_RANGE(Idle, Stopped)
};

bool enum_test()
{
    bool success = true;
    std::cout << "Testing Enum Reflection..." << std::endl;

    // 1. Serialization (Enum -> String)
    {
        json::value v = Color::Red;
        if (!v.is_string()) {
            std::cout << "Error: Color::Red should be serialized as string" << std::endl;
            success = false;
        }
        else if (v.as_string() != "Red") {
            std::cout << "Error: Color::Red serialized as " << v.as_string() << ", expected 'Red'" << std::endl;
            success = false;
        }
    }

    {
        json::value v = Direction::Left;
        if (v.as_string() != "Left") {
            std::cout << "Error: Direction::Left serialized as " << v.as_string() << ", expected 'Left'" << std::endl;
            success = false;
        }
    }

    // 2. Deserialization (String -> Enum)
    {
        json::value v = "Green";
        if (!v.is<Color>()) {
            std::cout << "Error: 'Green' should be identified as Color" << std::endl;
            success = false;
        }
        else {
            Color c = v.as<Color>();
            if (c != Color::Green) {
                std::cout << "Error: 'Green' parsed as wrong enum value" << std::endl;
                success = false;
            }
        }
    }

    // 3. Case Insensitive Deserialization
    {
        json::value v = "blue";
        if (!v.is<Color>()) {
            std::cout << "Error: 'blue' (lowercase) should be identified as Color" << std::endl;
            success = false;
        }
        else {
            Color c = v.as<Color>();
            if (c != Color::Blue) {
                std::cout << "Error: 'blue' parsed as wrong enum value" << std::endl;
                success = false;
            }
        }
    }

    // 4. Invalid String
    {
        json::value v = "Purple";
        if (v.is<Color>()) {
            std::cout << "Error: 'Purple' should NOT be identified as Color" << std::endl;
            success = false;
        }
        // Trying to cast should throw (via fallback to int conversion which throws on string)
        try {
            Color c = v.as<Color>();
            (void)c;
            std::cout << "Error: v.as<Color>() for 'Purple' should throw" << std::endl;
            success = false;
        }
        catch (...) {
            // Expected
        }
    }

    // 5. Constructor Throw on Unknown Enum
    {
        try {
            json::value v = (Color)3;
            std::cout << "Error: Constructing json::value from (Color)3 should throw" << std::endl;
            std::cout << "       Value became: " << v.to_string() << std::endl;
            success = false;
        }
        catch (const json::exception& e) {
            // Expected
            std::cout << "Caught expected exception: " << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Caught unknown exception" << std::endl;
        }
    }

    // 6. Operator Enum with Number (Legacy support check)
    {
        json::value v = 2; // matches Color::Green
        if (!v.is<Color>()) {
            std::cout << "Error: 2 should be identified as Color (via number)" << std::endl;
            success = false;
        }
        Color c = v.as<Color>();
        if (c != Color::Green) {
            std::cout << "Error: 2 failed to cast to Color::Green" << std::endl;
            success = false;
        }
    }

    // 7. Test MEOJSON_ENUM_RANGE macro with custom range
    std::cout << "Testing MEOJSON_ENUM_RANGE macro..." << std::endl;
    
    // 7.1 Serialization
    {
        json::value v = Status::Running;
        if (!v.is_string()) {
            std::cout << "Error: Status::Running should be serialized as string" << std::endl;
            success = false;
        }
        else if (v.as_string() != "Running") {
            std::cout << "Error: Status::Running serialized as " << v.as_string() << ", expected 'Running'" << std::endl;
            success = false;
        }
    }

    // 7.2 Deserialization
    {
        json::value v = "Paused";
        if (!v.is<Status>()) {
            std::cout << "Error: 'Paused' should be identified as Status" << std::endl;
            success = false;
        }
        else {
            Status s = v.as<Status>();
            if (s != Status::Paused) {
                std::cout << "Error: 'Paused' parsed as wrong enum value" << std::endl;
                success = false;
            }
        }
    }

    // 7.3 Case insensitive
    {
        json::value v = "stopped";
        if (!v.is<Status>()) {
            std::cout << "Error: 'stopped' (lowercase) should be identified as Status" << std::endl;
            success = false;
        }
        else {
            Status s = v.as<Status>();
            if (s != Status::Stopped) {
                std::cout << "Error: 'stopped' parsed as wrong enum value" << std::endl;
                success = false;
            }
        }
    }

    // 7.4 Number conversion
    {
        json::value v = 103; // matches Status::Stopped
        if (!v.is<Status>()) {
            std::cout << "Error: 103 should be identified as Status (via number)" << std::endl;
            success = false;
        }
        Status s = v.as<Status>();
        if (s != Status::Stopped) {
            std::cout << "Error: 103 failed to cast to Status::Stopped" << std::endl;
            success = false;
        }
    }

    // 7.5 All enum values
    {
        json::value idle = Status::Idle;
        if (idle.as_string() != "Idle") {
            std::cout << "Error: Status::Idle not serialized correctly" << std::endl;
            success = false;
        }

        json::value stopped = Status::Stopped;
        if (stopped.as_string() != "Stopped") {
            std::cout << "Error: Status::Stopped not serialized correctly" << std::endl;
            success = false;
        }
    }

    return success;
}
