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
#include <stack>
#include <forward_list>
#include <map>
#include <set>
#include <unordered_set>
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
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include "gl/glew.h"
#include "glfw/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw/glfw3native.h"
#include "../packages/glm.0.9.9.800/build/native/include/glm/gtx/string_cast.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/glm.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/ext.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/gtx/quaternion.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/gtc/quaternion.hpp"

#define etoi(_E_) static_cast<int32>(_E_)
#define etos(_E_) static_cast<size_t>(_E_)

#define text(_STR_) _STR_
#define wtext(_STR_) L##_STR_

#define ARRAY_COUNT(_a_) (sizeof(_a_) / sizeof(_a_[0]))

// types
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

    template<typename T, class _Alloc = std::allocator<T>>
    class CVector : public std::vector<T>
    {
        using ParentVectorT = std::vector<T>;

    public:
        CVector() : std::vector<T>() {};
        CVector(std::initializer_list<T>&& il) : std::vector<T>(std::forward<std::initializer_list<T>>(il)) {}
        CVector(const size_t size, const _Alloc& _Al = _Alloc()) : std::vector<T>(size, _Al) {};    // #cvcvuptr_resize - unique_ptr 을 CVector<CVector< 의 T 로 넣을 경우 CVector< 의 resize 가 컴파일 오류가 나서 일단 이렇게 한다

        template<typename U>
		T& operator[](const U& index)   // enum class 들을 [] 로 바로 받을 수 있게 하기 위함
		{
			if constexpr (std::is_enum_v < U>)
				return ParentVectorT::operator[](etoi(index));
			else
				return ParentVectorT::operator[](index);
		}
		template<typename U>
		const T& operator[](const U& index) const
		{
			if constexpr (std::is_enum_v < U>)
				return ParentVectorT::operator[](etoi(index));
			else
                return ParentVectorT::operator[](index);
		}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // { 특수 용도의 cursor 관리 영역
        typename ParentVectorT::const_iterator cursorBegin() const { cursor_ = this->begin(); return cursor(); }
        typename ParentVectorT::const_iterator cursorNext() const { cursor_++; return cursor(); }
        typename ParentVectorT::const_iterator cursor() const { return cursor_; }
        void setCursor(typename ParentVectorT::const_iterator newCursor) const { cursor_ = newCursor; }
        void pushCursor() const { stack_.push(cursor_); }
        void popCursor() const { cursor_ = stack_.top(); stack_.pop(); }
        bool isCursorEnd() const { return cursor_ == this->end(); }

    protected:
        mutable typename ParentVectorT::const_iterator cursor_;
        mutable std::stack<typename ParentVectorT::const_iterator> stack_;
    // }
    };

    template<typename T>
    using CForwardList = std::forward_list<T>;

    template<typename T>
    using CList = std::list<T>;

    template<typename K, typename T>
    using CHashMap = std::unordered_map<K, T>;

    using CString = std::string;
    using CWString = std::wstring;

    struct SMGEException : public std::exception
    {
        SMGEException(const CWString& exceptionMsg) : exceptionMsg_(exceptionMsg) {}
        CWString exceptionMsg_;
    };

    struct SScopedTimer
    {
        using self = SScopedTimer;
        using ClockType = std::chrono::steady_clock;

        const char* function_ = nullptr;
        const ClockType::time_point start_;

        SScopedTimer(const char* func) :
            function_{ func },
            start_(ClockType::now())
        {
        }

        SScopedTimer(const self&) = delete;
        self& operator=(const self&) = delete;

        SScopedTimer(self&&) = delete;
        self& operator=(self&&) = delete;

        ~SScopedTimer()
        {
            auto elapsed = ClockType::now() - start_;
            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
            //std::cout << ns << " ns / " << ns / 1'000'000.0 << " ms " << function_ << "\n";
        }
    };
}

// constants
namespace SMGE
{
    namespace Configs
    {
        constexpr float BoundEpsilon = 0.001f;			// 점이나 선, 면 등을 사용할 때 어떤 두께가 필요한 경우가 있어서 정의된 상수, OpenGL 단위 기준 1 을 1미터로 놓고 1000으로 나눴다 - 즉 1미리미터임
        constexpr float BoundCheckEpsilon = 0.0001f;
    }
}

// functions
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
            ReplaceInline(ret, L"\r\n", L"\n"); // 그러므로 여기서 정규화한다! 아오....
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

    template<typename Container, typename T>
    auto FindIt(const Container& container, const T& val)
    {
        return std::find(container.begin(), container.end(), val);
    }

    template<typename Container, typename Iter>
    bool IsFound(const Container& container, const Iter& it)
    {
        return it != container.end();
    }

    template<typename Container, typename T>
    bool Find(const Container& container, const T& val)
    {
        return IsFound(std::find(container.begin(), container.end(), val));
    }

    template<typename T>
    CVector<T> SplitStringToVector(const T& str, const T& delim)
    {
        CVector<T> tokens;
        size_t prev = 0, pos = 0;
        do
        {
            pos = str.find(delim, prev);
            if (pos == T::npos) pos = str.length();
            T token = str.substr(prev, pos - prev);
            if (!token.empty()) tokens.push_back(token);
            prev = pos + delim.length();
        } while (pos < str.length() && prev < str.length());
        return tokens;
    }

    template<typename T, typename U>
    bool IsContains(const T& str, const U& finding)
    {
        return str.find(finding) != T::npos;
    }
    template<typename T, typename U>
    bool IsStartsWith(const T& str, const U& finding)
    {
        return str.find(finding) == 0;
    }

    inline bool IsNearlyEqual(float l, float r, float epsilon = Configs::BoundCheckEpsilon)	// 여기 - 통합 필요
    {
        return std::fabsf(l - r) < epsilon;
    }

    inline bool IsInRange(float rangeL, float rangeR, float value)
    {
        if (rangeL < rangeR)
            return value >= rangeL && value < rangeR;
        return value >= rangeR && value < rangeL;
    }

    inline float GetDistanceSquared(const glm::vec3& l, const glm::vec3& r)
    {
        const auto x = (l.x - r.x), y = (l.y - r.y), z = (l.z - r.z);
        return x * x + y * y + z * z;
    }

    template<typename VecT>
    void FindMinMax(const std::initializer_list<VecT>& points, VecT& outMin, VecT& outMax)
    {
        std::for_each(points.begin(), points.end(), [&outMin, &outMax](const auto& point)
            {
                if (point.x < outMin.x)
                    outMin.x = point.x;
                if (point.y < outMin.y)
                    outMin.y = point.y;
                if (point.z < outMin.z)
                    outMin.z = point.z;

                if (point.x > outMax.x)
                    outMax.x = point.x;
                if (point.y > outMax.y)
                    outMax.y = point.y;
                if (point.z > outMax.z)
                    outMax.z = point.z;
            });
    }

    namespace Path
    {
        CWString GetNormalizedPath(const CWString& path);
        bool IsValidPath(const CWString& path);
        CWString GetDirectoryFullPath(const CWString& dir);
        CWString GetDirectoryCurrent();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// c++ helpers
//
#define DELETE_COPY_CTOR(classTypeName)\
		classTypeName(const classTypeName& other) = delete;\
		classTypeName& operator=(const classTypeName& other) = delete;

#define DELETE_MOVE_CTOR(classTypeName)\
		classTypeName(classTypeName&& other) = delete;\
		classTypeName& operator=(classTypeName&& other) = delete;

#define DEFAULT_COPY_CTOR(classTypeName)\
		classTypeName(const classTypeName& other) = default;\
		classTypeName& operator=(const classTypeName& other) = default;

#define DEFAULT_MOVE_CTOR(classTypeName)\
		classTypeName(classTypeName&& other) = default;\
		classTypeName& operator=(classTypeName&& other) = default;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// stack allocator - https://howardhinnant.github.io/stack_alloc.html
#include "short_alloc.h"

template <class T, std::size_t BufSize>
using StackVector = std::vector<T, short_alloc<T, BufSize, alignof(T)>>;

template <class T, std::size_t BufSize>
using StackSet = std::unordered_set<T, std::hash<T>, std::equal_to<T>, short_alloc<T, BufSize, alignof(T)>>;
