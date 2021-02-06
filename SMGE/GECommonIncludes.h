#pragma once

#include <Windows.h>
#undef min	// windows.h 의 이것때문에 CBoundCheck 쪽에서 컴파일 오류남
#undef max	// windows.h 의 이것때문에 CBoundCheck 쪽에서 컴파일 오류남

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include <cstdint>
#include <cinttypes>
#include <tuple>
#include <vector>
#include <string>
#include <list>
#include <queue>
#include <forward_list>
#include <map>
#include <set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <typeinfo>
#include <variant>
#include <any>
#include <optional>
#include <functional>
#include <numeric>

#include "gl/glew.h"
#include "glfw/glfw3.h"
#include "../packages/glm.0.9.9.800/build/native/include/glm/gtx/string_cast.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/glm.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/ext.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/gtx/quaternion.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/gtc/quaternion.hpp"

#define etoi(_E_) SCast<int32>(_E_)
#define etos(_E_) SCast<size_t>(_E_)

#define text(_STR_) _STR_
#define wtext(_STR_) L##_STR_

#define ARRAY_COUNT(_a_) (sizeof(_a_) / sizeof(_a_[0]))

namespace SMGE
{
    using int8 = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;

    using uint8 = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;

    using wchar = wchar_t;

    using TActorKey = uint32;
    const TActorKey InvalidActorKey = 0;

    inline bool IsValidActorKey(const TActorKey& ak)
    {
        return ak > 0;
    }

    template<typename T, typename U>
    inline T SCast(const U& u)
    {
        return static_cast<T>(u);
    }

    template<typename T, typename U>
    inline T DCast(const U& u)
    {
        return dynamic_cast<T>(u);
    }
}

#include "GEContainers.h"

#define IsA(__cobjectInst__, classRTTIName) (__cobjectInst__ && __cobjectInst__->getClassRTTIName() == wtext(#classRTTIName))

namespace SMGE
{
    template<typename T>
    extern T& ToLowerInline(T& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), std::tolower);
        return str;
    }

    template<typename T>
    extern T& ToUpperInline(T& str)
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

        // 아... 어쩔 수 없이 이렇게 처리한다 - 아래의 이유로 인하여 하나하나 지정해줘야할 듯... 이거 방법이 있을 것 같은데 아직 내 C++ 실력이 부족하구나
        else if constexpr (std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec2> || std::is_same_v<T, glm::mat4>)
            return ToTCHAR(glm::to_string(val));    // glm::vec or mat ...

        // deprecated 201226 {
		//// 이 코드가 위로 올라가면 지원이 안되는 클래스가 T 로 들어오는 경우 컴파일 오류가 나게 된다, 이거 SFINAE 로 컴파일 오류 안나고 넘어가게 할 수 없나?
		//else if constexpr (std::is_member_function_pointer<decltype(&T::operator CWString)>::value) // for SGRefl_Actor 등의 자동 처리를 위하여
		//	return val; }

        else
            return L"error - not support type!";
    }

    CWString ToTCHAR(const CString& astr);
    CString ToASCII(const CWString& wstr);
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

namespace SMGE
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
        T Replace(T source, const T& finding, const T& replacing)
        {
            ReplaceInline(source, finding, replacing);
            return source;
        }
        template<typename T>
        T Replace(T source, const typename T::value_type* finding, const typename T::value_type* replacing)
        {
            ReplaceInline(source, T{ finding }, T{ replacing });
            return source;
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
            static_assert(std::is_same_v<T, CWString>, "only support CWString!!!");

            T ret;
            auto in = std::wifstream(filePath, std::ifstream::binary | std::ifstream::ate);
            if (in.good())
            {
                auto size = in.tellg();
                ret = std::move(T(size, '\0'));

                in.seekg(0);
                in.read(&ret[0], size);

                in.close();

                // 로드할 때 \r\n 로 읽어진다 - Windows 에서...
                CuwakiDevUtils::ReplaceInline(ret, L"\r\n", L"\n"); // 그러므로 여기서 정규화한다! 아오....
                return ret;
            }

            return ret;
        }

        template<typename T>
        bool SaveToTextFile(const T& filePath, const T& text)
        {
            static_assert(std::is_same_v<T, CWString>, "only support CWString!!!");

            std::wofstream out(filePath);
            if (out.good())
            {
                out << text.c_str();
                out.close();

                // 세이브할 때 \n 으로 쓰더라도
                return true;
            }

            return false;
        }

        void Quat2Direction(glm::quat& q, float& pitch, float& yaw, float& roll);
    }

    namespace Path
    {
        CWString GetNormalizedPath(const CWString& path);
        bool IsValidPath(const CWString& path);
        CWString GetDirectoryFullPath(const CWString& dir);
        CWString GetDirectoryCurrent();
    }

    struct SMGEException : public std::exception
    {
        SMGEException(const CWString& exceptionMsg) : exceptionMsg_(exceptionMsg) {}
        CWString exceptionMsg_;
    };
}
