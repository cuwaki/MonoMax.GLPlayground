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
		bool IsTopTemplateAssetPath(CWString filePath)
		{
			return filePath;
		}

		bool IsTemplateAssetPath(CWString filePath)
		{
			return SMGE::GlobalUtils::IsContains(filePath, wtext("/templates/"));
		}

		void LoadContentClass(CWString filePath)
		{
			SGStringStreamIn strIn(CuwakiDevUtils::LoadFromTextFile(filePath));
			if (strIn.IsValid())
			{
				// �θ� Ŭ������ ��� �ö󰣴� - ��) slime.asset -> CAMonster -> CAPawn -> CActor
				TContentClass temp(nullptr);
				SGReflection parentRefl(static_cast<CInt_Reflection&>(temp));
				strIn >> parentRefl;

				auto parentAssetPath = parentRefl.getReflectionFilePath();
				if (parentAssetPath.length() == 0)
				{	// ���̻� �θ� ���� - �̰� ž�̴�, �������� ������ �ε带 �����Ѵ� - ��) CActor
				}
				else
				{	// �θ� Ŭ������ ���ø��� �ε��Ѵ�
					LoadContentClass(CAssetManager::GetTemplateAssetPath(parentRefl.getClassName()));
				}

				// �ڽ� Ŭ������ ������ ��� ����������� ���̴�
				strIn >> getContentClass()->getReflection();
			}
		}

	protected:
		bool isReadOnly_ = false;
		TContentClass* contentClass_;
	};
};
