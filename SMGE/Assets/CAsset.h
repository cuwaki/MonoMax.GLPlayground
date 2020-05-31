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

		// template<> ���� ���� �Ŷ� virtual ������ �ȵȴ�!!

	protected:
		CWString assetFilePath_;
		CWString assetFileName_;
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
		{	// ���⸦ ���Ͽ� �ܺο��� ����
			isReadOnly_ = false;
		}

		CAsset(CWString filePath) : CAssetBase()
		{	// �б⸦ ���Ͽ� ���ο��� ����
			isReadOnly_ = true;
			
			assetFilePath_ = filePath;
			LoadDefaultContentClass();
		}

		~CAsset()
		{
			if (isReadOnly_)
				delete contentClass_;
		}

		TContentClass* getContentClass() const { return contentClass_; }

	protected:
		void LoadDefaultContentClass()
		{
			contentClass_ = new TContentClass(nullptr);

			SGStringStreamIn strIn(CuwakiDevUtils::LoadFromTextFile(assetFilePath_));
			if (strIn.IsValid())
			{
				strIn >> getContentClass()->getReflection();
			}
			else
			{
				// error
			}
		}

	protected:
		bool isReadOnly_ = false;
		TContentClass* contentClass_;
	};
};
