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
#include <type_traits>
#include <typeinfo>
#include <variant>
#include <any>

#include "glfw/glfw3.h"
#include "../packages/glm.0.9.9.800/build/native/include/glm/gtx/string_cast.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/glm.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/ext.hpp"

#define etoi(_E_) SCast<int32>(_E_)
#define etos(_E_) SCast<size_t>(_E_)

#define text(_STR_) _STR_
#define wtext(_STR_) L##_STR_

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
}

#include "CGEContainers.h"

namespace MonoMaxGraphics
{
    template<typename T>
    extern T& ToLower(T& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), std::tolower);
        return str;
    }

    template<typename T>
    extern T& ToUpper(T& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), std::toupper);
        return str;
    }

    template<typename T>
    extern CWString ToTCHAR(const T& val)
    {
        if constexpr (std::is_same_v<T, CWString> || std::is_same_v<T, std::wstring>)
            return val;
        else if constexpr (std::is_same_v<T, CString> || std::is_same_v<T, std::string>)
            return ToTCHAR(val);
        else if constexpr (std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>)
            return ToTCHAR(std::to_string(val));
        else
        {	// glm::vec or mat ...
            return ToTCHAR(glm::to_string(val));
        }

        return L"error - not support type!";
    }

    CWString ToTCHAR(const CString& astr);
    CString ToASCII(const CWString& wstr);
}
