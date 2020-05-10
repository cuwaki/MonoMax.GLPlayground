#pragma once

#include "../GCommonIncludes.h"
#include "CGInterface.h"
#include "../Objects/CGObject.h"

namespace MonoMaxGraphics
{
	struct SGReflection;

	template<typename T>
	struct SGReflectionStream
	{
	};

	template<typename T>
	struct SGReflectionStreamOut : public SGReflectionStream<T>
	{
		SGReflectionStreamOut& operator<<(const SGReflection& reflData)
		{
			out_ = reflData;	// SGReflection 을 decltype(out_) 로 캐스팅해줘서 처리한다
			return *this;
		}

		T out_;
	};

	template<typename T>
	struct SGReflectionStreamIn : public SGReflectionStream<T>
	{
		SGReflectionStreamIn& operator>>(SGReflection& reflData)
		{
			reflData = in_;		// SGReflection 이 decltype(in_) 을 operator= 로 받는다
			return *this;
		}

		T in_;
	};

	using SGStringStreamOut = SGReflectionStreamOut<CWString>;
	using SGStringStreamIn = SGReflectionStreamIn<CWString>;

	using TupleVarName_VarType_Value = std::tuple<CWString, CWString, CWString>;
	const int32 Tuple_VarName = 0;
	const int32 Tuple_VarType = 1;
	const int32 Tuple_Value = 2;

#define _TO_REFL(_vartype_, _varname_) ((isFast_ == false? CWString(L""#_varname_ + META_DELIM + L""#_vartype_ + META_DELIM) : CWString()) + ReflectionUtils::ToREFL(_varname_) + VARIABLE_DELIM)
#define _FROM_REFL(_varname_, _in_vec_) ReflectionUtils::FromREFL_VEC(_varname_, _in_vec_)
#define _ADD_REFL_VARIABLE(_varname_) variablesMap_[L""#_varname_] = &_varname_
#define _REF_REFL_VARIABLE(_varname_) (*SCast<decltype(&_varname_)>(variablesMap_[L""#_varname_]))

	struct SGReflection
	{
		const CWString META_DELIM = L"$";
		const CWString VARIABLE_DELIM = L"\n";
		const CWString VALUE_DELIM = L"\t";

		SGReflection(CGObject& obj) :
			reflClassName_(obj.reflClassName_)
		{
			if (isFast_ == false)
				buildVariablesMap();
		}

		CWString& reflClassName_;

		virtual void buildVariablesMap();

		virtual operator CWString() const;	// operator>> 의 역할을 이게 한다
		virtual SGReflection& operator=(const CWString& fullReflectedStr);	// operator<< 의 최종 역할을 이게 한다

	protected:
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& variableSplitted);	// DevReflection
		virtual SGReflection& operator=(CVector<CWString>& variableSplitted);	// ReflectionUtils

		bool isFast_ = false;
		CMap<CWString, void*> variablesMap_;
	};

	namespace ReflectionUtils
	{
		template<typename T>
		extern CWString ToREFL(const T& val)
		{
			if constexpr (std::is_same_v<T, CWString> || std::is_same_v<T, std::wstring>)
				return val;	// 추가 할 일 - 딜리미터들을 다른 걸로 바꿔줘야한다
			else if constexpr (std::is_same_v<T, CString> || std::is_same_v<T, std::string>)
				return ToTCHAR(val);	// 추가 할 일 - 딜리미터들을 다른 걸로 바꿔줘야한다
			else if constexpr (std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>)
				return ToTCHAR(std::to_string(val));

			return L"error - not support type!";
		}

		template<>
		extern CWString ToREFL(const glm::mat4& gv);

		template<typename L>
		extern void FromREFL(L& left, const CWString& right)
		{
			if constexpr (std::numeric_limits<L>::is_integer)
				left = SCast<L>(std::atoll(ToASCII(right).c_str()));
			else if constexpr (std::is_floating_point_v<L>)
				left = SCast<L>(std::atof(ToASCII(right).c_str()));
			else if constexpr (std::is_same_v<L, CWString> || std::is_same_v<L, std::wstring>)
				left = right;	// 추가 할 일 - 딜리미터들을 원래의 값으로 변경해서 넣기
			else if constexpr (std::is_same_v<L, CString> || std::is_same_v<L, std::string>)
				left = ToASCII(right);	// 추가 할 일 - 딜리미터들을 원래의 값으로 변경해서 넣기
			else
			{	// glm::mat ...
				FromREFL(left, right);
			}
		}

		template<>
		extern void FromREFL(glm::mat4& gv, const CWString& str);

		template<typename L>
		extern void FromREFL_VEC(L& left, CVector<CWString>& rightVec)
		{
			FromREFL(left, *rightVec.cursor());
			rightVec.cursorNext();	// 다음으로!
		}

		template<typename L>
		extern void FromREFL_VEC(L& left, CVector<TupleVarName_VarType_Value>& rightVec)
		{
			auto tup = rightVec.cursor();

			CWString& varName = std::get<Tuple_VarName>(*tup);
			CWString& varType = std::get<Tuple_VarType>(*tup);
			CWString& value = std::get<Tuple_Value>(*tup);

			FromREFL(left, value);
#if _DEBUG
			varName = wtext("used - ") + varName;	// 사용됨 표시
#endif
			rightVec.cursorNext();	// 다음으로!
		}
	};

	class CGInterf_Reflection : public CGInterface
	{
	public:
		virtual SGReflection& getReflection() = 0;
		const SGReflection& getConstReflection() const
		{
			return const_cast<CGInterf_Reflection *>(this)->getReflection();
		}
	};
};
