#include "CInt_Reflection.h"

namespace SMGE
{
	const CWString SGReflection::META_DELIM = L"$";
	const CWString SGReflection::VARIABLE_DELIM = L"\n";
	const CWString SGReflection::VALUE_DELIM = L"\t";
	const CWString::value_type SGReflection::VARIABLE_DELIM_CHAR = L'\n';
	const CWString::value_type SGReflection::VALUE_DELIM_CHAR = L'\t';

	namespace ReflectionUtils
	{
		CWString _TO_REFL_Head(CWString _vartype_, CWString _varname_, bool isFast_)
		{
			return (isFast_ == false ? CWString(_varname_ + SGReflection::META_DELIM + _vartype_ + SGReflection::META_DELIM) : CWString());
		}

		// { glm::section
		template<>
		extern CWString ToREFL(const glm::mat4& gv)
		{
			CWString ret;
			for (int r = 0; r < 4; ++r)
			{
				for (int c = 0; c < 4; ++c)
				{
					ret += ToTCHAR(gv[r][c]);
					ret += SGReflection::VALUE_DELIM;
				}
			}
			return ret;
		}

		template<>
		extern void FromREFL(glm::mat4& gv, const CWString& valuesStr)
		{
			auto spl = SplitStringToVector(valuesStr, SGReflection::VALUE_DELIM);
			for (int r = 0; r < 4; ++r)
			{
				for (int c = 0; c < 4; ++c)
				{
					FromREFL(gv[r][c], spl[r * 4 + c]);
				}
			}
		}

		template<>
		extern CWString ToREFL(const glm::vec3& gv)
		{
			CWString ret;
			for (int c = 0; c < 3; ++c)
			{
				ret += ToTCHAR(gv[c]);
				ret += SGReflection::VALUE_DELIM;
			}
			return ret;
		}

		template<>
		extern void FromREFL(glm::vec3& gv, const CWString& valuesStr)
		{
			auto spl = SplitStringToVector(valuesStr, SGReflection::VALUE_DELIM);
			for (int c = 0; c < 3; ++c)
			{
				FromREFL(gv[c], spl[c]);
			}
		}

		template<>
		extern CWString ToREFL(const glm::vec2& gv)
		{
			CWString ret;
			for (int c = 0; c < 2; ++c)
			{
				ret += ToTCHAR(gv[c]);
				ret += SGReflection::VALUE_DELIM;
			}
			return ret;
		}

		template<>
		extern void FromREFL(glm::vec2& gv, const CWString& valuesStr)
		{
			auto spl = SplitStringToVector(valuesStr, SGReflection::VALUE_DELIM);
			for (int c = 0; c < 2; ++c)
			{
				FromREFL(gv[c], spl[c]);
			}
		}
		// } glm::section

		CString GetClassRTTIName(const CVector<TupleVarName_VarType_Value>& in)
		{
			in.pushCursor();
			CString rttiName;
			_FROM_REFL(rttiName, in);
			in.popCursor();
			return rttiName;
		}

		CWString GetReflectionFilePath(const CVector<TupleVarName_VarType_Value>& in)
		{
			in.pushCursor();
			CString rttiName;
			_FROM_REFL(rttiName, in);
			CWString reflPath;
			_FROM_REFL(reflPath, in);
			in.popCursor();
			return reflPath;
		}
	};

	const CWString& CInt_Reflection::getSourceFilePath() const
	{
		return getReflection().getSourceFilePath();
	}

	void SGReflection::buildVariablesMap()
	{
		//#define _TO_REFL(_vartype_, _varname_) ((isFast_ == false ? CWString(L""#_varname_ + META_DELIM + ToTCHAR(CString(typeid(_varname_).name())) + META_DELIM) : CWString()) + ReflectionUtils::ToREFL(_varname_) + VARIABLE_DELIM)

		/*	variant 로 하는 방법
		using TAllReflTypeVariant = std::variant<TActorKey*, CWString*, glm::mat4*>;

		CHashMap<CWString, TAllReflTypeVariant> variables;
		variables[L"className_"] = &className_;
		variables[L"actorKey_"] = &actorKey_;
		variables[L"worldTransform_"] = &worldTransform_;

		worldTransform_ = *std::get<decltype(&worldTransform_)>(variables[L"worldTransform_"]);*/

		// 아래 변수들은 SGReflection 입장에서는 const 이므로 등록 안하는 게 맞겠다 - 20200809
		//_ADD_REFL_VARIABLE(className_);
		//_ADD_REFL_VARIABLE(reflectionFilePath_);
	}

	const SGReflection& SGReflection::operator>>(CWString& out) const
	{
		out += _TO_REFL(CString, classRTTIName_);
		out += _TO_REFL(CWString, reflectionFilePath_);

		return *this;
	}

	struct SWhiteSpaceOrCommentForAssetString
	{
		bool isStartedMultiLineComments_ = false;

		void operator()(CWString& str)
		{
			if (isStartedMultiLineComments_)
			{	// 다중 주석 처리 중
				for (auto it = str.begin(); it != str.end(); it++)
				{
					auto c = *it;
					if (c == L'*' && (it + 1) != str.end() && *(it + 1) == L'/')
					{	// 여러 줄 주석 - 끝
						isStartedMultiLineComments_ = false;
						str.erase(str.begin(), (it + 2));	// */ 까지 끊고 
						break;
					}
				}

				if (isStartedMultiLineComments_ == false)
				{	// 주석이 끝났다 - 나머지 처리하기
				}
				else
				{	// 싹다 날림
					str.clear();
					return;
				}
			}

			// 맨 앞, 시작 부분에 있는 공백들을 제거한다, 탭이 스페이스로 대체된 에디터를 쓰는 경우 이럴 수 있다
			for (auto it = str.begin(); it != str.end();)
			{
				auto c = *it;

				if (c == L'\t' || c == L' ' || c == L'\r' || c == L'\n')
				{	// 들여쓰기
					str.erase(it);
				}
				else if (c == L'/' && (it + 1) != str.end() && *(it + 1) == L'/')
				{	// 한줄 주석
					str.erase(it, str.end());
				}
				else if (c == L'/' && (it + 1) != str.end() && *(it + 1) == L'*')
				{	// 여러 줄 주석 - 시작
					isStartedMultiLineComments_ = true;
					str.erase(it, str.end());
				}
				else
					break;
			}
		}
	};

	SGReflection& SGReflection::operator<<(const CWString& inFullReflectedStr)
	{
		CVector<CWString> in = SplitStringToVector(inFullReflectedStr, VARIABLE_DELIM);

		if (isFast_ == false)
		{
			CVector<TupleVarName_VarType_Value> metaSplitted;	// ##CVector<float> 리플렉션 데이터 최적화
			metaSplitted.reserve(in.size());

			SWhiteSpaceOrCommentForAssetString whiteOrCommentRemover;
			CVector<CWString> temp;
			for (int i = 0; i < in.size(); ++i)
			{
				whiteOrCommentRemover(in[i]);

				if (in[i].length() == 0)
					continue;

				temp = SplitStringToVector(in[i], META_DELIM);
				metaSplitted.emplace_back(std::tie(temp[Tuple_VarName], temp[Tuple_VarType], temp[Tuple_Value]));
			}	

			metaSplitted.cursorBegin();
			operator<<(metaSplitted);
			//if(pairInstance_ != nullptr)	// 여기서 부르면 asset 계층에 따라서 여러번 불리우게 된다! 그래서 막아둠 - 지금은 뭔가 이상한 vftbl 관련 버그가 있어서 못쓰고 있다
			//	pairInstance_->OnAfterDeserialized();
		}
		else
		{
			in.cursorBegin();

			operator<<(in);
			//if(pairInstance_ != nullptr)	// 여기서 부르면 asset 계층에 따라서 여러번 불리우게 된다! 그래서 막아둠 - 지금은 뭔가 이상한 vftbl 관련 버그가 있어서 못쓰고 있다
			//	pairInstance_->OnAfterDeserialized();
		}

		return *this;
	}

	// _isFast == true
	SGReflection& SGReflection::operator<<(const CVector<CWString>& in)
	{
		_FROM_REFL(classRTTIName_, in);
		_FROM_REFL(reflectionFilePath_, in);

		return *this;
	}

	// _isFast == false
	SGReflection& SGReflection::operator<<(const CVector<TupleVarName_VarType_Value>& in)
	{
		_FROM_REFL(classRTTIName_, in);
		_FROM_REFL(reflectionFilePath_, in);

		return *this;
	}
};
