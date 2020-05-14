#pragma once

#include <cstdint>
#include <cinttypes>
#include <tuple>
#include <vector>
#include <string>
#include <list>
#include <queue>
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
        else if constexpr (std::is_same_v<T, CString> || std::is_same_v<T, CString>)
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

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

namespace MonoMaxGraphics
{
    namespace CuwakiDevUtils
    {
        template<typename IT, typename T_CF>
        auto PartialSortList(IT begin, IT end, size_t m, const T_CF& T_Comparer)
        {
            using T = typename IT::value_type;

            std::vector<T> ret;
            ret.reserve(m);

            auto pque = std::priority_queue<T, std::vector<T>, T_CF>{ T_Comparer };

            for (auto it = begin; it != end; ++it)
            {
                if (pque.size() < m)
                {
                    pque.push(*it);
                }
                else if (T_Comparer(*it, pque.top()))
                {
                    pque.pop();
                    pque.push(*it);
                }
            }

            while (pque.empty() == false)
            {
                ret.push_back(pque.top());
                pque.pop();
            }

            std::reverse(ret.begin(), ret.end());
            return ret;
        }

        template<typename LIST, typename T_CF>
        std::vector<typename LIST::value_type> SortList(const LIST& list, const T_CF& T_Comparer)
        {
            return PartialSortList(list.begin(), list.end(), list.size(), T_Comparer);
        }

        template<typename T>
        void ReplaceInline(T& source, const T& finding, const T& replacing)
        {
            const auto len = source.length();
            const auto fLen = finding.length(), rLen = replacing.length();

            size_t oldFound = 0;
            while (oldFound < len)
            {
                auto found = source.find(finding, oldFound);
                if (found < len)
                {
                    source.replace(found, fLen, replacing);
                    oldFound += rLen;
                }
                else
                    break;
            };
        }

        template<typename T>
        void ReplaceInline(T& source, const typename T::value_type* finding, const typename T::value_type* replacing)
        {
            ReplaceInline(source, T{ finding }, T{ replacing });
        }

        struct ScopedTimer
        {
            using self = ScopedTimer;
            using ClockType = std::chrono::steady_clock;

            const char* function_ = nullptr;
            const ClockType::time_point start_;

            ScopedTimer(const char* func) :
                function_{ func },
                start_(ClockType::now())
            {
            }

            ScopedTimer(const self&) = delete;
            self& operator=(const self&) = delete;

            ScopedTimer(self&&) = delete;
            self& operator=(self&&) = delete;

            ~ScopedTimer()
            {
                auto elapsed = ClockType::now() - start_;
                auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
                //std::cout << ns << " ns / " << ns / 1'000'000.0 << " ms " << function_ << "\n";
            }
        };

        template<typename T>
        size_t GetFileSize(const T& filePath)
        {
            auto in = std::ifstream(filePath, std::ios::in);
            if (in.good())
            {
                in.seekg(0, std::ios_base::end);

                size_t ret = in.tellg();
                in.close();
                return ret;
            }
            return 0;
        }

        template<typename T>
        T LoadFromTextFile(const T& filePath)
        {
            T ret;

            auto in = std::ifstream(filePath, std::ifstream::binary | std::ifstream::ate);
            if (in.good())
            {
                auto size = in.tellg();
                ret = std::move(T(size, '\0'));

                in.seekg(0);
                in.read(&ret[0], size);

                in.close();
                return ret;
            }

            return ret;
        }

        template<typename T>
        bool SaveToTextFile(const T& filePath, const T& text)
        {
            std::ofstream out(filePath);
            if (out.good())
            {
                out << text.c_str() << std::feof;
                out.close();
                return true;
            }

            return false;
        }
    }
}
