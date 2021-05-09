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

	template<typename TContentClass, typename ...Args>
	using TContentClassNewFunc = std::function<TContentClass*(Args&&...)>;
	using TContentClassDeleteFunc = std::function<void(void*)>;

	template<typename TContentClass>
	class CAsset : public CAssetBase
	{
	public:
		CAsset(TContentClass* contentClass) : CAssetBase()
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
		
		template<typename NewContentF, typename DeleteContentF, typename ...Args>
		CAsset(CWString filePath, NewContentF&& nf, DeleteContentF&& df, Args&&... contentClassCtorArgs) : CAssetBase()
		{	// 읽기를 위하여 내부에서 만듦
			newContentFunc_ = std::forward<NewContentF>(nf);
			deleteContentFunc_ = std::forward<DeleteContentF>(df);

			assetFilePath_ = filePath;
			isReadOnly_ = true;
			LoadAssetRecursive(filePath, std::forward<Args>(contentClassCtorArgs)...);
		}

		~CAsset()
		{
			if (isReadOnly_ && contentClass_)
			{
				if(deleteContentFunc_)	// 지정된 파괴 방법
					deleteContentFunc_(contentClass_);
				else
					delete contentClass_;

				contentClass_ = nullptr;
			}
		}

		TContentClass* getContentClass() const
		{
			return static_cast<TContentClass*>(contentClass_);
		}

	protected:
		bool IsTemplate(CWString filePath) const
		{
			return SMGE::IsStartsWith(filePath, wtext("/templates/")) || SMGE::IsStartsWith(filePath, wtext("templates/"));
		}

		template<typename ...Args>
		void LoadAssetRecursive(CWString filePath, Args&&... contentClassCtorArgs)
		{
			assert(isReadOnly_ == true && "for read only");

			SGReflectionStringIn strIn(LoadFromTextFile(filePath));
			if (strIn.IsValid())
			{
				// 부모 클래스로 계속 올라간다 - 예) slime.asset -> CAMonster -> CAPawn -> CActor
				SGReflection currentRefl(nullptr);
				strIn >> currentRefl;	// 헤더만 읽음

				// 최하위일 때 RTTI 클래스를 생성해야한다, 예) CAMonster 일 때 contentClass_가 생성되어야한다
				if (contentClass_ == nullptr)
				{
					if (newContentFunc_)
					{	// 지정된 생성 방법
						contentClass_ = newContentFunc_(std::forward<Args>(contentClassCtorArgs)...);
					}
					else
					{	// 디폴트 생성 방법
						if constexpr (sizeof...(contentClassCtorArgs) == 0)
							contentClass_ = RTTI_CObject::NewDefault(currentRefl.getClassRTTIName(), nullptr);
						else
							contentClass_ = RTTI_CObject::NewVariadic<TContentClass>(nullptr, std::forward<Args>(contentClassCtorArgs)...);
					}
				}

				auto parentAssetPath = currentRefl.getSourceFilePath();
				if (IsTemplate(parentAssetPath) == true)
				{	// 더이상 부모가 없다 - 이게 탑 == 템플릿이다, 이제부터 실제로 로드를 시작한다 - 예) CActor
				}
				else
				{	// 부모 클래스의 템플릿을 로드한다
					LoadAssetRecursive(CAssetManager::GetTemplateAssetPath(ToTCHAR(currentRefl.getClassRTTIName())));
				}

				// 후위로 재귀 호출 되므로 CActor -> CAPawn -> CAMonster 순서로 적용되며, 하위의 값으로 계속 덮어씌워져나갈 것이다
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
		TContentClassNewFunc<TContentClass> newContentFunc_;
		TContentClassDeleteFunc deleteContentFunc_;
	};
}