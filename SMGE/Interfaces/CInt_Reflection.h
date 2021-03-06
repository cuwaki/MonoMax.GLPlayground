#pragma once

#include "../GECommonIncludes.h"
#include "CInterfaceBase.h"
#include "../Objects/CObject.h"


/**
	20200929 RTTI 및 REFLECTION 시스템 특이사항 정리

	1. RTTIName 을 가져오는 함수는 비슷한 이름으로 총 4가지가 존재한다
		
		SGReflection::getClassRTTIName(
		CInt_Reflection::getClassRTTIName(

		클래스의 멤버함수인 GetClassRTTIName(
		전역 템플릿함수인 GetClassRTTIName(

		각 클래스별 ClassRTTIName 을 const 로 보호하고, 런타임 및 정적으로 rttiname 에 접근하기 위하여 종류가 여러가지인 것이다

	2. 실제로 RTTI 로써 사용되지 않는 클래스들에는 RTTI 매크로나 함수, 변수가 없을 수 있다 - 이들은 C++의 인터페이스 클래스와 같다고 생각하면 된다

	3. DEFINE_RTTI_CObject_DEFAULT 매크로 - 말그대로 인자가 outer 1개인 디폴트 생성용이다
	3. DEFINE_RTTI_CObject_VARIETY 매크로 - 인자가 여러개인 각 클래스용 전용 생성자들을 처리하기 위해서 존재한다

	4. 맵 로딩등을 할 때 문자열로 rtti 할 때는 버라이어티 생성자를 사용할 수 없다 - 컴파일 타임에 타입이 명시적으로 필요하기 때문이다
*/

namespace SMGE
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
	class CInt_Reflection : public CInterfaceBase
	{
	public:
		virtual const CWString& getReflectionFilePath() const;
		virtual const CString& getClassRTTIName() const = 0;

		virtual SGReflection& getReflection() = 0;
		virtual const SGReflection& getConstReflection() const
		{
			return const_cast<CInt_Reflection*>(this)->getReflection();
		}

		virtual void CopyFromTemplate(const CInt_Reflection* templateObj)
		{
			if (templateObj != nullptr)
			{
				SGStringStreamIn in(templateObj->getConstReflection());
				in >> getReflection();
			}
		}

		virtual void OnAfterDeserialized() {}
	};

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

		SGReflection(CInt_Reflection* obj) : pair_(obj)
		{
			if (isFast_ == false)
				buildVariablesMap();
		}
		SGReflection(CInt_Reflection& obj) : SGReflection(&obj)
		{
		}

		virtual void OnBeforeSerialize() const {}
		virtual operator CWString() const;	// operator>> 의 역할을 이게 한다
		/* final */ SGReflection& operator=(const CWString& fullReflectedStr);	// operator<< 의 최종 역할을 이게 한다

		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& variableSplitted);	// DevReflection
		virtual SGReflection& operator=(CVector<CWString>& variableSplitted);	// ReflectionUtils

		const CString& getClassRTTIName() const { return classRTTIName_; }
		const CWString& getReflectionFilePath() const { return reflectionFilePath_; }

	protected:
		virtual void buildVariablesMap();

		CInt_Reflection* pair_;

		CString classRTTIName_;
		CWString reflectionFilePath_;

		bool isFast_ = false;
		CHashMap<CWString, void*> variablesMap_;
	};

	// 말그대로 .asset 의 헤더 부분만 딱 읽어오기 위한 클래스이다 - 애셋 로드할 때 실제 그 애셋 안에 뭐가 든지 모를 때 사용한다.
	class CHeaderOnlyReflection : public CInt_Reflection
	{
	public:
		CHeaderOnlyReflection(void *dummy) {}
		virtual const CString& getClassRTTIName() const { return dummyClassRTTIName_; }
		virtual SGReflection& getReflection() { return dummy_; }

	protected:
		CString dummyClassRTTIName_ = "CHeaderOnlyReflection";
		SGReflection dummy_{ *this };
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	namespace ReflectionUtils
	{
		#define _TO_REFL(_vartype_, _varname_) ((isFast_ == false? CWString(L""#_varname_ + META_DELIM + L""#_vartype_ + META_DELIM) : CWString()) + ReflectionUtils::ToREFL(_varname_) + VARIABLE_DELIM)
		#define _FROM_REFL(_varname_, _in_vec_) ReflectionUtils::FromREFL_SOURCEVECTOR(_varname_, _in_vec_)
		#define _ADD_REFL_VARIABLE(_varname_) variablesMap_[L""#_varname_] = &_varname_
		#define _REF_REFL_VARIABLE(_varname_) (*static_cast<decltype(&_varname_)>(variablesMap_[L""#_varname_]))

		CString GetClassRTTIName(CVector<TupleVarName_VarType_Value>& metaSplitted);
		CWString GetReflectionFilePath(CVector<TupleVarName_VarType_Value>& metaSplitted);

		// _TO_REFL 의 헤더만 만드는 함수 버전
		CWString _TO_REFL_Head(CWString _vartype_, CWString _varname_, bool isFast_);

		template<typename T>
		extern CWString ToREFL(const T& val)
		{
			// 문자열들의 경우
			// 추가 할 일 - " 검사 및 딜리미터들 그리고 \t을 "\\t"로 바꿔야한다
			// 탭같은 경우엔 왜 바꾸냐면 .asset 파일의 포맷에 \t으로 들여쓰기가 들어갈 수 있기 때문이다

			if constexpr (std::is_same_v<T, bool>)
				return val ? wtext("true") : wtext("false");
			else if constexpr (std::is_same_v<T, CWString> || std::is_same_v<T, std::wstring>)
				return val.length() == 0 ? wtext("\"\"") : (L'"' + val + L'"');	// 
			else if constexpr (std::is_same_v<T, CString> || std::is_same_v<T, std::string>)
				return ToTCHAR(val.length() == 0 ? "\"\"" : ('"' + val + '"'));
			else if constexpr (std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>)
				return ToTCHAR(std::to_string(val));
			// deprecated 201226 {
			//else if constexpr (std::is_member_function_pointer<decltype(&T::operator CWString)>::value) // for SGRefl_Actor 등의 자동 처리를 위하여
			//	return static_cast<CWString>(val); }

			return L"error - not support type!";
		}

		template<typename T>
		extern void FromREFL(T& left, const CWString& right)
		{
			// 문자열들의 경우
			// 추가 할 일 - 딜리미터들을 원래의 값으로 변경해서 넣기, " 들을 제대로 없애서 넣어주기
			if constexpr (std::is_same_v<T, bool>)
				left = (right == wtext("true"));
			else if constexpr (std::numeric_limits<T>::is_integer)
				left = static_cast<T>(std::atoll(ToASCII(right).c_str()));
			else if constexpr (std::is_floating_point_v<T>)
				left = static_cast<T>(std::atof(ToASCII(right).c_str()));
			else if constexpr (std::is_same_v<T, CWString> || std::is_same_v<T, std::wstring>)
				left = right.substr(1, right.length() - 2);
			else if constexpr (std::is_same_v<T, CString> || std::is_same_v<T, std::string>)
				left = ToASCII(right.substr(1, right.length() - 2));
			else
			{	// glm::mat ...
				FromREFL(left, right);
			}
		}

		// { glm:: section
		template<>
		extern CWString ToREFL(const glm::mat4& gv);
		template<>
		extern void FromREFL(glm::mat4& gv, const CWString& str);
		template<>
		extern CWString ToREFL(const glm::vec3& gv);
		template<>
		extern void FromREFL(glm::vec3& gv, const CWString& str);
		template<>
		extern CWString ToREFL(const glm::vec2& gv);
		template<>
		extern void FromREFL(glm::vec2& gv, const CWString& str);
		// } glm::section

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
#if DEBUG || _DEBUG
			//varName = wtext("used - ") + varName;	// 사용됨 표시
#endif
			rightVec.cursorNext();	// 다음으로!
		}

		// 하던 일 정리
		// 다중 컨테이너를 다룰 수 있게 해야한다 L"CVector<CVector<SGRefl_Actor>>";
		// template 으로 할지 runtime 에 할지 흠흠... dimension 을 이용하여 runtime 에 하는 게 나을 것 같다
		// 그리고 그게 끝나맨 매크로로 묶어줘야한다 ret += _TO_REFL_CONTAINER(CVector<SGRefl_Actor>, mapActorsW_);
		// 당연히 다중 컨테이너 읽는 부분도 수정되어야한다
		template<typename ContT, typename KeyT>
		CWString ToCVector(const ContT& cont, const CWString& contTypeName, const CWString& contVarName, std::optional<KeyT> contKeyForParent)
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
				size_t fB = contTypeName.find_first_of(L'<'), lB = contTypeName.find_first_of(L'>');
				CWString TName = contTypeName.substr(fB + 1, lB - fB - 1);

				for (const auto& it : cont)
				{
					//if constexpr (std::is_base_of_v<SGReflection, ContT::value_type>)	//(TName.find_first_of(L"SGRefl") != CWString::npos)
					if constexpr (std::is_same_v<std::reference_wrapper<SGReflection>, ContT::value_type>)	//(TName.find_first_of(L"SGRefl") != CWString::npos)
					{	// SGRefl_Actor 등
						ret += static_cast<CWString>(it.get());
					}
					else
					{	// float, glm::vec3...
						ret += _TO_REFL_Head(TName, L"[x]", false);
						ret += ReflectionUtils::ToREFL(it);
						ret += SGReflection::VARIABLE_DELIM;
					}

					//if (*itStr.crbegin() == SGReflection::VARIABLE_DELIM_CHAR || *itStr.crbegin() == SGReflection::VALUE_DELIM_CHAR)
					//{	// SGRefl_Actor 이거나 glm::mat4 이거나 ...
					//	ret += itStr;
					//}
					//else
					//{	// *it 가 literal 타입일 경우 주르륵 써지는 경우에는 DELIM 들이 없다
					//	//[0]$float$0.000000
					//	// _TO_REFL 처럼 작동을 시켜야한다, 근데 여긴 함수 안쪽이라서 이게 안된다, 일단 임시로 하드코딩!

					//	ret += wtext("[x]$");
					//	ret += TName + wtext("$");
					//	ret += itStr;
					//	ret += SGReflection::VARIABLE_DELIM;
					//}
				}
			}

			return ret;
		}

		// 하던 일 정리
		//// 다중 컨테이너 읽을 때 
		//// mapActorsW_.resize
		//	// mapActorsW_
		//		// mapActorsW_.resize
		//			// CGActors
		////_FROM_REFL(actorKey_, variableSplitted);
		template<typename CT, typename VS, typename FUNC, typename... FUNCArgs>
		void FromCVector(CT& cont, VS& variableSplitted, FUNC& func, FUNCArgs&&... funcargs)
		{
			size_t contSize = 0;
			CWString& size_str = std::get<Tuple_Value>(*variableSplitted.cursor());
			ReflectionUtils::FromREFL(contSize, size_str);

			variableSplitted.cursorNext();	// [2] = (L"mapActorsW_", L"CVector<SGRefl_Actor>", L"2") 를 먹어치운다

			if (contSize == 0)
				return;

			cont.clear();
			cont.reserve(contSize);

			for (size_t i = 0; i < contSize; i++)
			{
				func(variableSplitted, i, std::forward<FUNCArgs>(funcargs)...);
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

		template<typename SGRefl_REF, typename COBJ_UniqPtr>
		CVector<SGRefl_REF>& clearAndEmplaceBackINST2REFL(CVector<SGRefl_REF>& refl, const CVector<COBJ_UniqPtr>& instance)
		{
			refl.clear();
			refl.reserve(instance.size());

			for (int i = 0; i < instance.size(); ++i)
			{
				refl.emplace_back(instance[i]->getReflection());
			}

			return refl;
		};

		template<typename TargetClass, typename OuterClass>
		auto FuncLoadClass(OuterClass* outer, CVector<TupleVarName_VarType_Value>& variableSplitted)
		{
			auto rttiName = ReflectionUtils::GetClassRTTIName(variableSplitted);

			auto newObj = dynamic_cast<TargetClass*>(RTTI_CObject::NewDefault(rttiName, outer));
			newObj->getReflection() = variableSplitted;

			return newObj;
		};
	};
};
