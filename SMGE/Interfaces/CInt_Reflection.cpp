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
			auto spl = GlobalUtils::SplitStringToVector(valuesStr, SGReflection::VALUE_DELIM);
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
			auto spl = GlobalUtils::SplitStringToVector(valuesStr, SGReflection::VALUE_DELIM);
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
			auto spl = GlobalUtils::SplitStringToVector(valuesStr, SGReflection::VALUE_DELIM);
			for (int c = 0; c < 2; ++c)
			{
				FromREFL(gv[c], spl[c]);
			}
		}
		// } glm::section
	};

	void SGReflection::buildVariablesMap()
	{
		//#define _TO_REFL(_vartype_, _varname_) ((isFast_ == false ? CWString(L""#_varname_ + META_DELIM + ToTCHAR(CString(typeid(_varname_).name())) + META_DELIM) : CWString()) + ReflectionUtils::ToREFL(_varname_) + VARIABLE_DELIM)

		/*	variant �� �ϴ� ���
		using TAllReflTypeVariant = std::variant<TActorKey*, CWString*, glm::mat4*>;

		CHashMap<CWString, TAllReflTypeVariant> variables;
		variables[L"className_"] = &className_;
		variables[L"actorKey_"] = &actorKey_;
		variables[L"worldTransform_"] = &worldTransform_;

		worldTransform_ = *std::get<decltype(&worldTransform_)>(variables[L"worldTransform_"]);*/

		// �Ʒ� �������� SGReflection ���忡���� const �̹Ƿ� ��� ���ϴ� �� �°ڴ� - 20200809
		//_ADD_REFL_VARIABLE(className_);
		//_ADD_REFL_VARIABLE(reflectionFilePath_);
	}

	SGReflection::operator CWString() const
	{
		OnBeforeSerialize();

		CWString ret;

		ret += _TO_REFL(CString, classRTTIName_);
		ret += _TO_REFL(CWString, reflectionFilePath_);

		return ret;
	}

	void RemoveOrCommentForAssetFiles(CWString& str)
	{
		// �� ��, ���� �κп� �ִ� ������� �����Ѵ�, ���� �����̽��� ��ü�� �����͸� ���� ��� �̷� �� �ִ�
		for (auto it = str.begin(); it != str.end();)
		{
			auto c = *it;
			if (c == L'\t' || c == L' ' || c == L'\r' || c == L'\n')
			{	// �鿩����
				str.erase(it);
			}
			else if (c == L'/' && (it + 1) != str.end() && *(it + 1) == L'/')
			{	// �ּ�
				str.erase(it, str.end());
			}
			else
				break;
		}
	}

	SGReflection& SGReflection::operator=(const CWString& fullReflectedStr)
	{
		CVector<CWString> variableSplitted = GlobalUtils::SplitStringToVector(fullReflectedStr, VARIABLE_DELIM);

		if (isFast_ == false)
		{
			CVector<TupleVarName_VarType_Value> metaSplitted;	// ##CVector<float> ���÷��� ������ ����ȭ
			metaSplitted.reserve(variableSplitted.size());

			CVector<CWString> temp;
			for (int i = 0; i < variableSplitted.size(); ++i)
			{
				RemoveOrCommentForAssetFiles(variableSplitted[i]);

				if (variableSplitted[i].length() == 0)
					continue;

				temp = GlobalUtils::SplitStringToVector(variableSplitted[i], META_DELIM);
				metaSplitted.emplace_back(std::tie(temp[Tuple_VarName], temp[Tuple_VarType], temp[Tuple_Value]));
			}	

			metaSplitted.cursorBegin();

			auto& ret = operator=(metaSplitted);
			pair_.OnAfterDeserialized();
			return ret;
		}
		else
		{
			variableSplitted.cursorBegin();

			auto& ret = operator=(variableSplitted);
			pair_.OnAfterDeserialized();
			return ret;
		}
	}

	SGReflection& SGReflection::operator=(CVector<CWString>& variableSplitted)
	{
		// from fast
		CString dummy;
		_FROM_REFL(dummy, variableSplitted);	// classRTTIName_ ����

		_FROM_REFL(reflectionFilePath_, variableSplitted);

		return *this;
	}

	SGReflection& SGReflection::operator=(CVector<TupleVarName_VarType_Value>& metaSplitted)
	{
		CString dummy;
		_FROM_REFL(dummy, metaSplitted);	// classRTTIName_ ����

		CWString reflPath;
		_FROM_REFL(reflPath, metaSplitted);

		if (reflectionFilePath_.length() == 0)
		{	// ���� ���� - ##8A �� �ӽ� ����
			reflectionFilePath_ = std::move(reflPath);
		}

		return *this;
	}

	CString SGReflection::GetClassRTTIName(CVector<TupleVarName_VarType_Value>& metaSplitted)
	{
		metaSplitted.pushCursor();
		CString rttiName;
		_FROM_REFL(rttiName, metaSplitted);	// classRTTIName_ ����
		metaSplitted.popCursor();
		return rttiName;
	}
};
