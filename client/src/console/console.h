#pragma once

#include <iostream>
#include <cstdint>

namespace io
{
    template <typename T> void print(T object)
    {
        std::cout << object;
    }

    template <typename T> void println(T line)
    {
        std::cout << line << std::endl;
    }

    uint32_t SafeScanUint32(std::string_view prompt);
}