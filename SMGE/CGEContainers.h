#pragma once

namespace MonoMaxGraphics
{
    template<typename T>
    class CVector : public std::vector<T>
    {
        using ParentVectorT = std::vector<T>;

    public:
        CVector() : std::vector<T>() {};
        CVector(std::initializer_list<T>&& il) : std::vector<T>(std::move(il)) {}

        template<typename U>
        T& operator[](const U& index)   // enum class 들을 [] 로 바로 받을 수 있게 하기 위함
        {
            return ParentVectorT::operator[](etoi(index));
        }
        template<typename U>
        const T& operator[](const U& index) const
        {
            return ParentVectorT::operator[](etoi(index));
        }

        typename ParentVectorT::iterator cursorBegin() { cursor_ = this->begin(); return cursor();   }
        typename ParentVectorT::iterator cursorNext() { cursor_++; return cursor(); }
        typename ParentVectorT::iterator cursor() {  return cursor_; }
        void setCursor(typename ParentVectorT::iterator newCursor) { cursor_ = newCursor; }

        bool isCursorEnd() { return cursor_ == this->end(); }

    protected:
        typename ParentVectorT::iterator cursor_;
    };

    template<typename T>
    using CFList = std::forward_list<T>;

    template<typename T>
    using CList = std::list<T>;

    template<typename K, typename T>
    using CMap = std::unordered_map<K, T>;

    using CString = std::string;
    using CWString = std::wstring;

#define CUniqPtr std::unique_ptr
#define MakeUniqPtr std::make_unique

#define CSharPtr std::shared_ptr
#define MakeSharPtr std::make_shared
#define SPtrCast std::static_pointer_cast

    /* 아래와 같이 하면 상속된 클래스의 스마트포인터를 베이스 클래스의 스마트 포인터에 넣을 때 컴파일 오류가 난다 - 어려워서 그냥 매크로로 처리한다
    // std::unique_ptr wapper
    template<typename T>
    using CUniqPtr = std::unique_ptr<T>;

    template <class _Ty, class... _Types, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
    _NODISCARD CUniqPtr<_Ty> MakeUniqPtr(_Types&&... _Args) { // make a unique_ptr
        return CUniqPtr<_Ty>(new _Ty(_STD forward<_Types>(_Args)...));
    };

    // std::shared_ptr wapper
    template<typename T>
    using CSharPtr = std::shared_ptr<T>;

    template <class _Ty, class... _Types>
    _NODISCARD CSharPtr<_Ty> MakeSharPtr(_Types&&... _Args) { // make a shared_ptr
        const auto _Rx = new std::_Ref_count_obj2<_Ty>(_STD forward<_Types>(_Args)...);
        CSharPtr<_Ty> _Ret;
        _Ret._Set_ptr_rep_and_enable_shared(_STD addressof(_Rx->_Storage._Value), _Rx);
        return _Ret;
    };
    */

    template<typename T>
    using CWeakPtr = std::weak_ptr<T>;

    // 전역 유틸리티 함수들
    namespace GlobalUtils
    {
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
    }
};
