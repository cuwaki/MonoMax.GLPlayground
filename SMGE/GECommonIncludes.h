#pragma once

#include <cstdint>
#include <cinttypes>
#include <tuple>
#include <vector>
#include <string>
#include <list>
#include <forward_list>
#include <unordered_map>
#include <memory>
#include <algorithm>

#include "glfw/glfw3.h"
#include "../packages/glm.0.9.9.800/build/native/include/glm/glm.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/ext.hpp"

namespace MonoMaxGraphics
{
    using int8 = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;

    using uint8 = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;

    template<typename T, typename U>
    inline T SCast(U u)
    {
        return static_cast<T>(u);
    }

    template<typename T, typename U>
    inline T DCast(U u)
    {
        return dynamic_cast<T>(u);
    }

    template<typename T>
    T& ToLower(T& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), std::tolower);
        return str;
    }

    template<typename T>
    T& ToUpper(T& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), std::toupper);
        return str;
    }
}

#define etoi(_E_) SCast<int32>(_E_)
#define etos(_E_) SCast<size_t>(_E_)

#define text(_STR_) _STR_
#define wtext(_STR_) L##_STR_

#include "CGEContainers.h"
