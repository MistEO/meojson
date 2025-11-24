#include <iostream>

#include "array_test.h"
#include "include_test.h"
#include "jsonc_test.h"
#include "object_test.h"
#include "parse_test.h"
#include "precision_test.h"
#include "serializing_test.h"
#include "unicode_test.h"
#include "value_test.h"
#include "enum_test.h"

int main()
{
    std::cout << "\n****** Testing ******\n" << std::endl;

    std::cout << "\n*** parsing ***\n" << std::endl;
    bool success = parsing();

    std::cout << "\n*** serializing ***\n" << std::endl;
    success &= serializing();

    std::cout << "\n*** jsonizing ***\n" << std::endl;
    success &= jsonizing();

    std::cout << "\n*** include_test ***\n" << std::endl;
    success &= include_test();

    std::cout << "\n*** precision_test ***\n" << std::endl;
    success &= precision_test();

    std::cout << "\n*** unicode_test ***\n" << std::endl;
    success &= unicode_test();

    std::cout << "\n*** value_test ***\n" << std::endl;
    success &= value_test();

    std::cout << "\n*** enum_test ***\n" << std::endl;
    success &= enum_test();

    std::cout << "\n*** object_test ***\n" << std::endl;
    success &= object_test();

    std::cout << "\n*** array_test ***\n" << std::endl;
    success &= array_test();

    std::cout << "\n*** jsonc_test ***\n" << std::endl;
    success &= jsonc_test();

    if (!success) {
        std::cout << "\n****** Test failed ******\n" << std::endl;
        return -1;
    }

    std::cout << "\n****** Test passed ******\n" << std::endl;
    return 0;
}
