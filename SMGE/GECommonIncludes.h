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

template<typename T>
using CVector = std::vector<T>;

template<typename T>
using CFList = std::forward_list<T>;

template<typename T>
using CList = std::list<T>;

template<typename K, typename T>
using CMap = std::unordered_map<K, T>;

using CString = std::string;
using CWString = std::wstring;

template<typename T>
using CUniqPtr = std::unique_ptr<T>;

template <class _Ty, class... _Types, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
_NODISCARD CUniqPtr<_Ty> MakeUniqPtr(_Types&&... _Args) { // make a unique_ptr
    return CUniqPtr<_Ty>(new _Ty(_STD forward<_Types>(_Args)...));
}

template<typename T>
using CSharPtr = std::shared_ptr<T>;

template <class _Ty, class... _Types>
_NODISCARD CSharPtr<_Ty> MakeSharPtr(_Types&&... _Args) { // make a shared_ptr
    const auto _Rx = new std::_Ref_count_obj2<_Ty>(_STD forward<_Types>(_Args)...);
    CSharPtr<_Ty> _Ret;
    _Ret._Set_ptr_rep_and_enable_shared(_STD addressof(_Rx->_Storage._Value), _Rx);
    return _Ret;
}

template<typename T>
using CWeakPtr = std::weak_ptr<T>;

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
void ToLower(T& str)
{
    std::transform(str.begin(), str.end(), str.begin(), std::tolower);
}

template<typename T>
void ToUpper(T& str)
{
    std::transform(str.begin(), str.end(), str.begin(), std::toupper);
}
