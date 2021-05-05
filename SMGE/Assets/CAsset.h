#pragma once

#include "../GECommonIncludes.h"
#include "../Interfaces/CInt_Reflection.h"
#include "../RTTI.hpp"
#include "../Objects/CObject.h"

namespace SMGE
{
	class CAssetBase
	{
	public:
		CAssetBase() {}

		// template<> 으로 쓰일 거라서 virtual 있으면 안된다!!

	protected:
		CWString assetFilePath_;
		bool isReadOnly_ = false;
		CObject* contentClass_ = nullptr;
	};

	template<typename TBaseClass>
	using TContentClassNewFunc = std::function<TBaseClass*(void)>;

	using TContentClassDeleteFunc = std::function<void(void)>;

	template<typename TBaseClass>
	class CAsset : public CAssetBase
	{
	public:
		CAsset(TBaseClass* contentClass) : CAssetBase()
		{	// 쓰기를 위하여 외부에서 받음
			contentClass_ = contentClass;
			isReadOnly_ = false;
		}

		template<typename ...Args>
		CAsset(CWString filePath, Args&&... contentClassCtorArgs) : CAssetBase()
		{	// 읽기를 위하여 내부에서 만듦
			assetFilePath_ = filePath;
			isReadOnly_ = true;
			LoadAssetRecursive(filePath, std::forward<Args>(contentClassCtorArgs)...);
		}
		
		template<typename NewF, typename DeleteF, typename ...Args>
		CAsset(CWString filePath, NewF&& nf, DeleteF&& df, Args&&... contentClassCtorArgs) : CAssetBase()
		{	// 읽기를 위하여 내부에서 만듦
			newFunc_ = std::forward<NewF>(nf);
			deleteFunc_ = std::forward<DeleteF>(df);

			assetFilePath_ = filePath;
			isReadOnly_ = true;
			LoadAssetRecursive(filePath, std::forward<Args>(contentClassCtorArgs)...);
		}

		~CAsset()
		{
			if (isReadOnly_ && contentClass_)
			{
				if(deleteFunc_)	// 지정된 파괴 방법
					deleteFunc_();
				else
					delete contentClass_;

				contentClass_ = nullptr;
			}
		}

		TBaseClass* getContentClass() const
		{
			return static_cast<TBaseClass*>(contentClass_);
		}

	protected:
		bool IsTemplateAssetPath(CWString filePath) const
		{
			return SMGE::IsStartsWith(filePath, wtext("/templates/")) || SMGE::IsStartsWith(filePath, wtext("templates/"));
		}
		bool IsEmptyAssetPath(CWString filePath) const
		{
			return filePath.length() == 0;
		}

		template<typename ...Args>
		void LoadAssetRecursive(CWString filePath, Args&&... contentClassCtorArgs)
		{
			assert(isReadOnly_ == true && "for read only");

			SGStringStreamIn strIn(LoadFromTextFile(filePath));
			if (strIn.IsValid())
			{
				// 부모 클래스로 계속 올라간다 - 예) slime.asset -> CAMonster -> CAPawn -> CActor
				SGReflection currentRefl(nullptr);
				strIn >> currentRefl;

				// top일 때 RTTI 클래스를 생성해야한다
				if (contentClass_ == nullptr)
				{
					if (newFunc_)
					{	// 지정된 생성 방법
						contentClass_ = newFunc_();	// 지금은 contentClassCtorArgs 를 무시할 수 밖에 없는 구조이다 - TContentClassNewFunc 에 Args... 을 지정할수가 없어서임
					}
					else
					{	// 디폴트 생성 방법
						if constexpr (sizeof...(contentClassCtorArgs) == 0)
							contentClass_ = RTTI_CObject::NewDefault(currentRefl.getClassRTTIName(), nullptr);
						else
							contentClass_ = RTTI_CObject::NewVariety<TBaseClass>(nullptr, std::forward<Args>(contentClassCtorArgs)...);
					}
				}

				auto parentAssetPath = currentRefl.getSourceFilePath();
				if (IsTemplateAssetPath(parentAssetPath) == true)
				{	// 더이상 부모가 없다 - 이게 탑 == 템플릿이다, 이제부터 실제로 로드를 시작한다 - 예) CActor
				}
				else
				{	// 부모 클래스의 템플릿을 로드한다
					LoadAssetRecursive(CAssetManager::GetTemplateAssetPath(ToTCHAR(currentRefl.getClassRTTIName())));
				}

				// 자식 클래스의 값으로 계속 덮어씌워져나갈 것이다
				strIn >> getContentClass()->getReflection();
			}
			else
			{
				throw SMGEException(wtext("LoadAssetRecursive : bad filepath - ") + filePath);
			}

			// 마지막에 한번만 부르도록 - 지금은 뭔가 이상한 vftbl 관련 버그가 있어서 못쓰고 있다
			//getContentClass()->OnAfterDeserialized();
		}

	protected:
		TContentClassNewFunc<TBaseClass> newFunc_;
		TContentClassDeleteFunc deleteFunc_;
	};
}