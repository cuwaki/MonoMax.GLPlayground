#include "CGInterf_Reflection.h"

namespace MonoMaxGraphics
{
	const CWString SGReflection::META_DELIM = L"$";
	const CWString SGReflection::VARIABLE_DELIM = L"\n";
	const CWString SGReflection::VALUE_DELIM = L"\t";

	namespace ReflectionUtils
	{
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
			auto spl = GlobalUtils::SplitStringToVector(valuesStr, SGReflection::VALUE_DELIM);
			for (int r = 0; r < 4; ++r)
			{
				for (int c = 0; c < 4; ++c)
				{
					FromREFL(gv[r][c], spl[r * 4 + c]);
				}
			}
		}
	};

	void SGReflection::buildVariablesMap()
	{
		//#define _TO_REFL(_vartype_, _varname_) ((isFast_ == false ? CWString(L""#_varname_ + META_DELIM + ToTCHAR(CString(typeid(_varname_).name())) + META_DELIM) : CWString()) + ReflectionUtils::ToREFL(_varname_) + VARIABLE_DELIM)

		/*	variant 로 하는 방법
		using TAllReflTypeVariant = std::variant<TActorKey*, CWString*, glm::mat4*>;

		CMap<CWString, TAllReflTypeVariant> variables;
		variables[L"className_"] = &className_;
		variables[L"actorKey_"] = &actorKey_;
		variables[L"worldTransform_"] = &worldTransform_;

		worldTransform_ = *std::get<decltype(&worldTransform_)>(variables[L"worldTransform_"]);*/

		_ADD_REFL_VARIABLE(className_);
	}

	SGReflection::operator CWString() const
	{
		return _TO_REFL(CWString, className_);
	}

	SGReflection& SGReflection::operator=(const CWString& fullReflectedStr)
	{
		CVector<CWString> variableSplitted = GlobalUtils::SplitStringToVector(fullReflectedStr, VARIABLE_DELIM);

		if (isFast_ == false)
		{
			CVector<TupleVarName_VarType_Value> metaSplitted;
			metaSplitted.reserve(variableSplitted.size());

			CVector<CWString> temp;
			for (int i = 0; i < variableSplitted.size(); ++i)
			{
				temp = GlobalUtils::SplitStringToVector(variableSplitted[i], META_DELIM);
				metaSplitted.emplace_back(std::tie(temp[Tuple_VarName], temp[Tuple_VarType], temp[Tuple_Value]));
			}	

			metaSplitted.cursorBegin();
			return operator=(metaSplitted);
		}
		else
		{
			variableSplitted.cursorBegin();
			return operator=(variableSplitted);
		}
	}

	SGReflection& SGReflection::operator=(CVector<CWString>& variableSplitted)
	{
		// from fast
		_FROM_REFL(className_, variableSplitted);
		return *this;
	}

	SGReflection& SGReflection::operator=(CVector<TupleVarName_VarType_Value>& metaSplitted)
	{
		_FROM_REFL(className_, metaSplitted);
		return *this;
	}
};
