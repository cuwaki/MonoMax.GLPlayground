#pragma once

#include "../GECommonIncludes.h"
#include "CInterfaceBase.h"
#include "../Objects/CObject.h"

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
			out_ = reflData;	// SGReflection �� decltype(out_) �� ĳ�������༭ ó���Ѵ�
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
			reflData = in_;		// SGReflection �� decltype(in_) �� operator= �� �޴´�
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
		CWString& getReflectionFilePath() { return reflectionFilePath_; }
		virtual const CWString& getClassName() = 0;

		virtual SGReflection& getReflection() = 0;
		virtual const SGReflection& getConstReflection() const
		{
			return const_cast<CInt_Reflection*>(this)->getReflection();
		}

		virtual void CopyFromTemplate(const CInt_Reflection& templateObj)
		{
			SGStringStreamIn in(templateObj.getConstReflection());
			in >> getReflection();
		}

		virtual void OnAfterDeserialized() {}

	protected:
		CWString reflectionFilePath_;
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

		SGReflection(CInt_Reflection& obj) : 
			pair_(obj), 
			className_(obj.getClassName()),
			reflectionFilePath_(obj.getReflectionFilePath()) // ##8A
		{
			if (isFast_ == false)
				buildVariablesMap();
		}

		virtual void OnBeforeSerialize() const {}
		virtual operator CWString() const;	// operator>> �� ������ �̰� �Ѵ�
		/* final */ SGReflection& operator=(const CWString& fullReflectedStr);	// operator<< �� ���� ������ �̰� �Ѵ�

		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& variableSplitted);	// DevReflection
		virtual SGReflection& operator=(CVector<CWString>& variableSplitted);	// ReflectionUtils

		const CWString& getClassName() const { return className_; }
		const CWString& getReflectionFilePath() const { return reflectionFilePath_; }

	protected:
		virtual void buildVariablesMap();

		CInt_Reflection& pair_;


		// ���� ���� - �̰� 2���� ������ ������ - ������ �ʿ��ϴ�
		// Reflection ��ü�� className �� ��Ÿ�����ϴ� ��찡 �ְ� - �������� ���Ǵ� SGRefl_Transform
		// pair_ �� className �� ��Ÿ�����ϴ� ��찡 �ִ�
		// ���� ���� - ��ø���� SGReflection ���� �Ǿ����� �� // ##8A �� ���� ���۷����� ����Ǿ������� ������ ������ ���������������� ������ �ִ� - ��)CActor
		const CWString& className_;	
		CWString& reflectionFilePath_;

		CWString reflectionName_;

		bool isFast_ = false;
		CHashMap<CWString, void*> variablesMap_;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	namespace ReflectionUtils
	{
		#define _TO_REFL(_vartype_, _varname_) ((isFast_ == false? CWString(L""#_varname_ + META_DELIM + L""#_vartype_ + META_DELIM) : CWString()) + ReflectionUtils::ToREFL(_varname_) + VARIABLE_DELIM)
		#define _FROM_REFL(_varname_, _in_vec_) ReflectionUtils::FromREFL_SOURCEVECTOR(_varname_, _in_vec_)
		#define _ADD_REFL_VARIABLE(_varname_) variablesMap_[L""#_varname_] = &_varname_
		#define _REF_REFL_VARIABLE(_varname_) (*SCast<decltype(&_varname_)>(variablesMap_[L""#_varname_]))

		// _TO_REFL �� ����� ����� �Լ� ����
		CWString _TO_REFL_Head(CWString _vartype_, CWString _varname_, bool isFast_);

		template<typename T>
		extern CWString ToREFL(const T& val)
		{
			// ���ڿ����� ���
			// �߰� �� �� - " �˻� �� �������͵� �׸��� \t�� "\\t"�� �ٲ���Ѵ�
			// �ǰ��� ��쿣 �� �ٲٳĸ� .asset ������ ���˿� \t���� �鿩���Ⱑ �� �� �ֱ� �����̴�

			if constexpr (std::is_same_v<T, CWString> || std::is_same_v<T, std::wstring>)
				return val.length() == 0 ? wtext("\"\"") : (L'"' + val + L'"');	// 
			else if constexpr (std::is_same_v<T, CString> || std::is_same_v<T, std::string>)
				return ToTCHAR(val.length() == 0 ? "\"\"" : ('"' + val + '"'));
			else if constexpr (std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>)
				return ToTCHAR(std::to_string(val));
			else if constexpr (std::is_member_function_pointer<decltype(&T::operator CWString)>::value) // for SGRefl_Actor ���� �ڵ� ó���� ���Ͽ�
				return val;

			return L"error - not support type!";
		}

		template<typename L>
		extern void FromREFL(L& left, const CWString& right)
		{
			// ���ڿ����� ���
			// �߰� �� �� - �������͵��� ������ ������ �����ؼ� �ֱ�, " ���� ����� ���ּ� �־��ֱ�
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
			rightVec.cursorNext();	// ��������!
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
			//varName = wtext("used - ") + varName;	// ���� ǥ��
#endif
			rightVec.cursorNext();	// ��������!
		}

		// �ϴ� �� ����
		// ���� �����̳ʸ� �ٷ� �� �ְ� �ؾ��Ѵ� L"CVector<CVector<SGRefl_Actor>>";
		// template ���� ���� runtime �� ���� ����... dimension �� �̿��Ͽ� runtime �� �ϴ� �� ���� �� ����
		// �׸��� �װ� ������ ��ũ�η� ��������Ѵ� ret += _TO_REFL_CONTAINER(CVector<SGRefl_Actor>, actorLayers_[1]);
		// �翬�� ���� �����̳� �д� �κе� �����Ǿ���Ѵ�
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
				// ���� �����̳�
				//for (const auto& child : cont)
				//{
				//	CWString childContTypeName = L"CVector<SGRefl_Actor>";	// �ڵ����� ��������
				//	CWString childContVarName = L"[i]";	// �ڵ����� ��������, �� ���� ��쿡�� ���� ó������߰ڴ�

				//	// ���⼭ �̷��� �ڽĵ鿡 ���ؼ� �� ���������Ѵ�
				//	//ret += ToCVector(child, childContTypeName, childContVarName, 0);
				//}
			}
			else
			{	// �������̴�
				size_t fB = contTypeName.find_first_of(L'<'), lB = contTypeName.find_first_of(L'>');
				CWString TName = contTypeName.substr(fB + 1, lB - fB - 1);

				for (const auto& it : cont)
				{
					if constexpr (std::is_base_of_v<SGReflection, CT::value_type>)	//(TName.find_first_of(L"SGRefl") != CWString::npos)
					{	// SGRefl_Actor ��
						ret += SCast<CWString>(it);
					}
					else
					{	// float, glm::vec3...
						ret += _TO_REFL_Head(TName, L"[x]", false);
						ret += ReflectionUtils::ToREFL(it);
						ret += SGReflection::VARIABLE_DELIM;
					}

					//if (*itStr.crbegin() == SGReflection::VARIABLE_DELIM_CHAR || *itStr.crbegin() == SGReflection::VALUE_DELIM_CHAR)
					//{	// SGRefl_Actor �̰ų� glm::mat4 �̰ų� ...
					//	ret += itStr;
					//}
					//else
					//{	// *it �� literal Ÿ���� ��� �ָ��� ������ ��쿡�� DELIM ���� ����
					//	//[0]$float$0.000000
					//	// _TO_REFL ó�� �۵��� ���Ѿ��Ѵ�, �ٵ� ���� �Լ� �����̶� �̰� �ȵȴ�, �ϴ� �ӽ÷� �ϵ��ڵ�!

					//	ret += wtext("[x]$");
					//	ret += TName + wtext("$");
					//	ret += itStr;
					//	ret += SGReflection::VARIABLE_DELIM;
					//}
				}
			}

			return ret;
		}

		// �ϴ� �� ����
		//// ���� �����̳� ���� �� 
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

			variableSplitted.cursorNext();	// [2] = (L"actorLayers_[1]", L"CVector<SGRefl_Actor>", L"2") �� �Ծ�ġ���

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

		template<typename REFLECTION, typename INSTANCE>
		CVector<REFLECTION>& clearAndEmplaceBackINST2REFL(CVector<REFLECTION>& refl, const CVector<INSTANCE>& instance)
		{
			refl.clear();
			refl.reserve(instance.size());

			for (int i = 0; i < instance.size(); ++i)
			{
				refl.emplace_back(instance[i]);
			}

			return refl;
		};
	};
};
