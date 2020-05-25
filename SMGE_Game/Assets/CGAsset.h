#pragma once

#include "../../SMGE/GECommonIncludes.h"
#include "../Interfaces/CGInterf_Reflection.h"

namespace SMGE
{
	class CGAssetBase
	{
	public:
		using TContentClass = void;

		CGAssetBase() {}

		// template<> ���� ���� �Ŷ� virtual ������ �ȵȴ�!!

	protected:
		CWString assetFilePath_;
		CWString assetFileName_;
	};

	template<typename C>
	class CGAsset : public CGAssetBase
	{
	public:
		using TContentClass = C;
		using TReflectionStruct = typename C::TReflectionStruct;

	public:
		CGAsset(TContentClass* contentClass) :
			contentClass_(contentClass),
			CGAssetBase()
		{	// ���⸦ ���Ͽ� �ܺο��� ����
			isReadOnly_ = false;
		}

		CGAsset(CWString filePath) : CGAssetBase()
		{	// �б⸦ ���Ͽ� ���ο��� ����
			isReadOnly_ = true;
			
			assetFilePath_ = filePath;
			LoadDefaultContentClass();
		}

		~CGAsset()
		{
			if (isReadOnly_)
				delete contentClass_;
		}

		TContentClass* getContentClass() const { return contentClass_; }

	protected:
		void LoadDefaultContentClass()
		{
			contentClass_ = new TContentClass();

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
