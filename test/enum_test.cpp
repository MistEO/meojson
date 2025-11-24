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
    // We need an enum value that is within range but not named, OR assume we rely on reflection failure.
    // Since our reflection covers -128 to 128, picking a value in that range that isn't defined:
    // Color has 1, 2, 4. 3 is not defined.
    // Note: is_valid<E, V> checks if __PRETTY_FUNCTION__ contains the name.
    // For (Color)3, it might output "(Color)3" or similar, which our parser should reject (needs to start with letter/underscore).
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
    // If we construct value from int, can we cast to enum?
    {
        json::value v = 2; // matches Color::Green
        if (!v.is<Color>()) {
            // is<Color> checks string OR number.
            std::cout << "Error: 2 should be identified as Color (via number)" << std::endl;
            success = false;
        }
        Color c = v.as<Color>();
        if (c != Color::Green) {
            std::cout << "Error: 2 failed to cast to Color::Green" << std::endl;
            success = false;
        }
    }

    return success;
}
