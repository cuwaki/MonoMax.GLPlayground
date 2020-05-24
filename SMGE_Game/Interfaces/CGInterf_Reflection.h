#pragma once

#include "../GCommonIncludes.h"
#include "CGInterface.h"
#include "../Objects/CGObject.h"

namespace MonoMaxGraphics
{
	struct SGReflection;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct SGReflectionStream
	{
		SGReflectionStream() = default;
		bool IsValid() { return false; }
	};

	template<typename T>
	struct SGReflectionStreamOut : public SGReflectionStream<T>
	{
		SGReflectionStreamOut() = default;
		SGReflectionStreamOut(const SGReflection& sourceRefl)
		{
			*this << sourceRefl;
		}

		SGReflectionStreamOut& operator<<(const SGReflection& reflData)
		{
			out_ = reflData;	// SGReflection 을 decltype(out_) 로 캐스팅해줘서 처리한다
			return *this;
		}

		bool IsValid() { return out_.length() > 0; }

		T out_;
	};

	template<typename T>
	struct SGReflectionStreamIn : public SGReflectionStream<T>
	{
		SGReflectionStreamIn() = default;
		SGReflectionStreamIn(CWString&& source)
		{
			in_ = std::move(source);
		}
		SGReflectionStreamIn(const SGReflection& sourceRefl)
		{
			in_ = sourceRefl;
		}

		SGReflectionStreamIn& operator>>(SGReflection& reflData)
		{
			reflData = in_;		// SGReflection 이 decltype(in_) 을 operator= 로 받는다
			return *this;
		}

		bool IsValid() { return in_.length() > 0; }

		T in_;
	};

	using SGStringStreamOut = SGReflectionStreamOut<CWString>;
	using SGStringStreamIn = SGReflectionStreamIn<CWString>;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	using TupleVarName_VarType_Value = std::tuple<CWString, CWString, CWString>;
	const int32 Tuple_VarName = 0;
	const int32 Tuple_VarType = 1;
	const int32 Tuple_Value = 2;

	struct SGReflection
	{
		static const CWString META_DELIM;
		static const CWString VARIABLE_DELIM;
		static const CWString VALUE_DELIM;
		static const CWString::value_type VARIABLE_DELIM_CHAR;
		static const CWString::value_type VALUE_DELIM_CHAR;

		SGReflection(CGObject& obj) : className_(obj.className_)
		{
			if (isFast_ == false)
				buildVariablesMap();
		}

		virtual operator CWString() const;	// operator>> 의 역할을 이게 한다
		/* final */ SGReflection& operator=(const CWString& fullReflectedStr);	// operator<< 의 최종 역할을 이게 한다

		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& variableSplitted);	// DevReflection
		virtual SGReflection& operator=(CVector<CWString>& variableSplitted);	// ReflectionUtils

	protected:
		virtual void buildVariablesMap();

		CWString& className_;
		bool isFast_ = false;
		CMap<CWString, void*> variablesMap_;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	namespace ReflectionUtils
	{
		#define _TO_REFL(_vartype_, _varname_) ((isFast_ == false? CWString(L""#_varname_ + META_DELIM + L""#_vartype_ + META_DELIM) : CWString()) + ReflectionUtils::ToREFL(_varname_) + VARIABLE_DELIM)
		#define _FROM_REFL(_varname_, _in_vec_) ReflectionUtils::FromREFL_SOURCEVECTOR(_varname_, _in_vec_)
		#define _ADD_REFL_VARIABLE(_varname_) variablesMap_[L""#_varname_] = &_varname_
		#define _REF_REFL_VARIABLE(_varname_) (*SCast<decltype(&_varname_)>(variablesMap_[L""#_varname_]))

		template<typename T>
		extern CWString ToREFL(const T& val)
		{
			// 문자열들의 경우
			// 추가 할 일 - " 검사 및 딜리미터들을 다른 걸로 바꿔줘야한다

			if constexpr (std::is_same_v<T, CWString> || std::is_same_v<T, std::wstring>)
				return val.length() == 0 ? wtext("\"\"") : (L'"' + val + L'"');	// 
			else if constexpr (std::is_same_v<T, CString> || std::is_same_v<T, std::string>)
				return ToTCHAR(val.length() == 0 ? "\"\"" : ('"' + val + '"'));
			else if constexpr (std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>)
				return ToTCHAR(std::to_string(val));

			return L"error - not support type!";
		}

		template<>
		extern CWString ToREFL(const glm::mat4& gv);

		template<typename L>
		extern void FromREFL(L& left, const CWString& right)
		{
			// 문자열들의 경우
			// 추가 할 일 - 딜리미터들을 원래의 값으로 변경해서 넣기, " 들을 제대로 없애서 넣어주기
			if constexpr (std::numeric_limits<L>::is_integer)
				left = SCast<L>(std::atoll(ToASCII(right).c_str()));
			else if constexpr (std::is_floating_point_v<L>)
				left = SCast<L>(std::atof(ToASCII(right).c_str()));
			else if constexpr (std::is_same_v<L, CWString> || std::is_same_v<L, std::wstring>)
				left = right.substr(1, right.length() - 2);
			else if constexpr (std::is_same_v<L, CString> || std::is_same_v<L, std::string>)
				left = ToASCII(right.substr(1, right.length() - 2));
			else
			{	// glm::mat ...
				FromREFL(left, right);
			}
		}

		template<>
		extern void FromREFL(glm::mat4& gv, const CWString& str);

		template<typename L>
		extern void FromREFL_SOURCEVECTOR(L& left, CVector<CWString>& rightVec)
		{
			FromREFL(left, *rightVec.cursor());
			rightVec.cursorNext();	// 다음으로!
		}

		template<typename L>
		extern void FromREFL_SOURCEVECTOR(L& left, CVector<TupleVarName_VarType_Value>& rightVec)
		{
			auto tup = rightVec.cursor();

			CWString& varName = std::get<Tuple_VarName>(*tup);
			CWString& varType = std::get<Tuple_VarType>(*tup);
			CWString& value = std::get<Tuple_Value>(*tup);

			FromREFL(left, value);
#if _DEBUG
			//varName = wtext("used - ") + varName;	// 사용됨 표시
#endif
			rightVec.cursorNext();	// 다음으로!
		}

		// 하던 일 정리
		// 다중 컨테이너를 다룰 수 있게 해야한다 L"CVector<CVector<SGRefl_Actor>>";
		// template 으로 할지 runtime 에 할지 흠흠... dimension 을 이용하여 runtime 에 하는 게 나을 것 같다
		// 그리고 그게 끝나맨 매크로로 묶어줘야한다 ret += _TO_REFL_CONTAINER(CVector<SGRefl_Actor>, actorLayers_[1]);
		// 당연히 다중 컨테이너 읽는 부분도 수정되어야한다
		template<typename CT, typename KT>
		CWString ToCVector(const CT& cont, const CWString& contTypeName, const CWString& contVarName, std::optional<KT> contKeyForParent)
		{
			CWString ret;
			ret += contVarName;
			if (contKeyForParent.has_value())
				ret += L"[" + ToTCHAR(*contKeyForParent) + L"]";
			ret += SGReflection::META_DELIM;

			ret += contTypeName;
			ret += SGReflection::META_DELIM;

			ret += ToTCHAR(cont.size());
			ret += SGReflection::VARIABLE_DELIM;

			size_t dimension = std::count(contTypeName.begin(), contTypeName.end(), '<');

			if (dimension > 1)
			{
				// 다중 컨테이너
				//for (const auto& child : cont)
				//{
				//	CWString childContTypeName = L"CVector<SGRefl_Actor>";	// 자동으로 만들어야함
				//	CWString childContVarName = L"[i]";	// 자동으로 만들어야함, 맵 같은 경우에는 따로 처리해줘야겠다

				//	// 여기서 이렇게 자식들에 대해서 또 내려가야한다
				//	//ret += ToCVector(child, childContTypeName, childContVarName, 0);
				//}
			}
			else
			{	// 마지막이다
				for (const auto& it : cont)
				{
					CWString itStr = ToTCHAR(it);

					if (*itStr.crbegin() == SGReflection::VARIABLE_DELIM_CHAR || *itStr.crbegin() == SGReflection::VALUE_DELIM_CHAR)
					{	// SGRefl_Actor 이거나 glm::mat4 이거나 ...
						ret += itStr;
					}
					else
					{	// *it 가 literal 타입일 경우 주르륵 써지는 경우에는 DELIM 들이 없다
						//[0]$float$0.000000
						// _TO_REFL 처럼 작동을 시켜야한다, 근데 여긴 함수 안쪽이라서 이게 안된다, 일단 임시로 하드코딩!
						// 테스트 코드
						ret += wtext("[x]$float$");
						ret += itStr;
						ret += SGReflection::VARIABLE_DELIM;
					}
				}
			}

			return ret;
		}

		// 하던 일 정리
		//// 다중 컨테이너 읽을 때 
		//// actorLayers_.resize
		//	// actorLayers_[0
		//		// actorLayers_[0.resize
		//			// CGActors
		//	// actorLayers_[1
		//		// actorLayers_[0.resize
		//			// CGActors
		//
		////_FROM_REFL(actorKey_, variableSplitted);
		template<typename CT, typename VS, typename FUNC>
		void FromCVector(CT& cont, VS& variableSplitted, FUNC& func)
		{
			size_t contSize = 0;
			CWString& size_str = std::get<Tuple_Value>(*variableSplitted.cursor());
			ReflectionUtils::FromREFL(contSize, size_str);

			variableSplitted.cursorNext();	// [2] = (L"actorLayers_[1]", L"CVector<SGRefl_Actor>", L"2") 를 먹어치운다

			if (contSize == 0)
				return;

			cont.clear();
			cont.reserve(contSize);

			for (size_t i = 0; i < contSize; i++)
			{
				func(variableSplitted, i);
			}
		}

		template<typename CT>
		auto FromRefl_PushBack(CT& cont)
		{
			return [&cont](auto& variableSplitted, size_t i)
			{
				typename CT::value_type temp;
				_FROM_REFL(temp, variableSplitted);
				cont.push_back(temp);
			};
		};
	};

	class CGInterf_Reflection : public CGInterface
	{
	public:
		virtual SGReflection& getReflection() = 0;
		virtual const SGReflection& getConstReflection() const
		{
			return const_cast<CGInterf_Reflection*>(this)->getReflection();
		}
		
		virtual void CopyFromTemplate(const CGInterf_Reflection& templateObj)
		{
			SGStringStreamIn in(templateObj.getConstReflection());
			in >> getReflection();
		}
	};
};
