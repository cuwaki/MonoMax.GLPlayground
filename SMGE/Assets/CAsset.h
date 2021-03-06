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
		void* contentClass_ = nullptr;
	};

	// TBaseClass - 로드할 애셋의 베이스 클래스를 넣어야한다 - CStaticMeshActor 일 경우 CActor, CMeshComponent 일 경우 CComponent, 모르겠으면 CInt_Reflection 등등
	template<typename TBaseClass>
	class CAsset : public CAssetBase
	{
	public:
		CAsset(TBaseClass* contentClass) :
			CAssetBase()
		{	// 쓰기를 위하여 외부에서 받음
			contentClass_ = static_cast<void*>(contentClass);
			isReadOnly_ = false;
		}

		CAsset(CWString filePath) : CAssetBase()
		{	// 읽기를 위하여 내부에서 만듦
			isReadOnly_ = true;
		
			assetFilePath_ = filePath;

			LoadAssetRecursive(filePath);
		}

		~CAsset()
		{
			if (isReadOnly_ && contentClass_)
			{
				//auto delTarget = dynamic_cast<TBaseClass*>(reinterpret_cast<CInt_Reflection*>(contentClass_));
				auto delTarget = static_cast<TBaseClass*>(contentClass_);
				delete delTarget;
				contentClass_ = nullptr;
			}
		}

		TBaseClass* getContentClass() const
		{
			//return dynamic_cast<TBaseClass*>(reinterpret_cast<CInt_Reflection*>(contentClass_));
			return static_cast<TBaseClass*>(contentClass_);
		}

	protected:
		bool IsTemplateAssetPath(CWString filePath)
		{
			return SMGE::IsStartsWith(filePath, wtext("/templates/")) || SMGE::IsStartsWith(filePath, wtext("templates/"));
		}
		bool IsEmptyAssetPath(CWString filePath)
		{
			return filePath.length() == 0;
		}

		void LoadAssetRecursive(CWString filePath)
		{
			assert(isReadOnly_ == true && "for read only");

			SGStringStreamIn strIn(LoadFromTextFile(filePath));
			if (strIn.IsValid())
			{
				// 부모 클래스로 계속 올라간다 - 예) slime.asset -> CAMonster -> CAPawn -> CActor
				SGReflection currentRefl(nullptr);
				strIn >> currentRefl;

				if (contentClass_ == nullptr)
				{	// top일 때 RTTI 클래스를 생성해야한다
					contentClass_ = RTTI_CObject::NewDefault(currentRefl.getClassRTTIName(), nullptr);
				}

				auto parentAssetPath = currentRefl.getReflectionFilePath();
				if (IsTemplateAssetPath(parentAssetPath) == true)
				{	// 더이상 부모가 없다 - 이게 탑 == 템플릿이다, 이제부터 실제로 로드를 시작한다 - 예) CActor
				}
				else
				{	// 부모 클래스의 템플릿을 로드한다
					LoadAssetRecursive(CAssetManager::GetTemplateAssetPath(ToTCHAR(currentRefl.getClassRTTIName())));
				}

				// 자식 클래스의 값으로 계속 덮어씌워져나갈 것이다
				CInt_Reflection* contentClass = dynamic_cast<CInt_Reflection*>(getContentClass());
				strIn >> contentClass->getReflection();
			}
			else
			{
				throw SMGEException(wtext("LoadAssetRecursive : bad filepath - ") + filePath);
			}
		}
	};
};
