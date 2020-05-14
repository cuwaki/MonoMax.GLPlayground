#pragma once

#include "../../SMGE/GECommonIncludes.h"
#include "../Objects/CGObject.h"
#include "../Interfaces/CGInterf_Reflection.h"

namespace MonoMaxGraphics
{
	template<typename C> class CGAsset;

	struct SGRefl_AssetBase : public SGReflection
	{
		SGRefl_AssetBase(CGObject& obj) :
			SGReflection(obj)
		{
		}
	};

	template<typename C>
	struct SGRefl_Asset : public SGRefl_AssetBase
	{
		using CR = typename C::ReflectionStruct;

		SGRefl_Asset(CGAsset<C>& asset) :
			childRefl_(SCast<CR&>(asset.getContentClass()->getReflection2())),
			SGRefl_AssetBase(asset)
		{
		}

		virtual operator CWString() const override
		{
			CWString ret = SGRefl_AssetBase::operator CWString();
			ret += childRefl_;
			return ret;
		}

		CR& childRefl_;
	};

	class CGAssetBase : public CGObject, public CGInterf_Reflection
	{
	public:
		CGAssetBase() : CGObject() {}
	};

	/*
		���� �� ����

		CGAsset �� ���ø� Ŭ�����ε� virtual getReflection �� �����ϴ� CGAssetBase �� ��ӹ����ϱ�
		����� ��ö �Լ��� ȣ�� ó���� �ȵȴ� - �� �� ����!! ��� �̻��� �����͸� �����Ͽ� �����Ⱚ�� �޾ƿ´�!

		CGAsset �� CGObject �� �ƴϰ� �ϰ�, CGInterf_Reflection �� ���μ� ��ö �Լ��� �ƴ� �ɷ� ���Ͽ� �а� ���⸦ ó���ϴ� �ɷ� ����
		�� CGAsset �� Reflection ó���� �ϵ��ڵ����� �ؾ߰ڴ�

		getReflection2 ���ֱ�
	*/

	template<typename C>
	class CGAsset : public CGAssetBase
	{
	public:
		using ReflectionStruct = SGRefl_Asset<C>;

		friend struct ReflectionStruct;

		using CR = typename C::ReflectionStruct;
		friend CR;

	public:
		CGAsset(C* contentClass = nullptr) :
			contentClass_(contentClass),
			CGAssetBase()
		{
			reflClassName_ = wtext("SMGE_Game::CGAsset<") + (getContentClass() ? getContentClass()->getReflClassName() : wtext("nullptr")) + wtext(">");
		}

		C* getContentClass() const { return contentClass_; }

		virtual SGReflection& getReflection() override
		{
			if (reflAsset_.get() == nullptr)
				reflAsset_ = MakeUniqPtr<ReflectionStruct>(*this);
			return *reflAsset_.get();
		}
		SGReflection& getReflection2()
		{
			if (reflAsset_.get() == nullptr)
				reflAsset_ = MakeUniqPtr<ReflectionStruct>(*this);
			return *reflAsset_.get();
		}

	protected:
		C* contentClass_;
		CWString filePath_;

		CUniqPtr<ReflectionStruct> reflAsset_;
	};
};
