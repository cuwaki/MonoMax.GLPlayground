#pragma once

#include "../GECommonIncludes.h"
#include "../Interfaces/CInt_Reflection.h"

namespace SMGE
{
	class CAssetBase
	{
	public:
		using TContentClass = void;

		CAssetBase() {}

		// template<> 으로 쓰일 거라서 virtual 있으면 안된다!!

	protected:
		CWString assetFilePath_;
	};

	template<typename C>
	class CAsset : public CAssetBase
	{
	public:
		using TContentClass = C;
		using TReflectionStruct = typename C::TReflectionStruct;

	public:
		CAsset(TContentClass* contentClass) :
			contentClass_(contentClass),
			CAssetBase()
		{	// 쓰기를 위하여 외부에서 받음
			isReadOnly_ = false;
		}

		CAsset(CWString filePath) : CAssetBase()
		{	// 읽기를 위하여 내부에서 만듦
			isReadOnly_ = true;
		
			contentClass_ = new TContentClass(nullptr);
			assetFilePath_ = filePath;

			LoadContentClass(filePath);
		}

		~CAsset()
		{
			if (isReadOnly_ && contentClass_)
				delete contentClass_;
		}

		TContentClass* getContentClass() const { return contentClass_; }

	protected:
		bool IsTemplateAssetPath(CWString filePath)
		{
			return SMGE::GlobalUtils::IsStartsWith(filePath, wtext("/templates/"));
		}

		void LoadContentClass(CWString filePath)
		{
			SGStringStreamIn strIn(CuwakiDevUtils::LoadFromTextFile(filePath));
			if (strIn.IsValid())
			{
				// 부모 클래스로 계속 올라간다 - 예) slime.asset -> CAMonster -> CAPawn -> CActor
				TContentClass temp(nullptr);
				SGReflection parentRefl(static_cast<CInt_Reflection&>(temp));	// 이유를 모르겠는데 여기서 SCast 쓰면 컴파일 오류난다
				strIn >> parentRefl;

				auto parentAssetPath = parentRefl.getReflectionFilePath();
				if (IsTemplateAssetPath(parentAssetPath) == true)
				{	// 더이상 부모가 없다 - 이게 탑 == 템플릿이다, 이제부터 실제로 로드를 시작한다 - 예) CActor
				}
				else
				{	// 부모 클래스의 템플릿을 로드한다
					LoadContentClass(CAssetManager::GetTemplateAssetPath(ToTCHAR(parentRefl.getClassRTTIName())));
				}

				// 자식 클래스의 값으로 계속 덮어씌워져나갈 것이다
				strIn >> getContentClass()->getReflection();
			}
			else
			{
				throw SMGEException(wtext("LoadContentClass : bad filepath - ") + filePath);
			}
		}

	protected:
		bool isReadOnly_ = false;
		TContentClass* contentClass_;
	};
};
