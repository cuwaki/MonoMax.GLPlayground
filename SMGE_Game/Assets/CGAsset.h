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
		여기 할 차례

		CGAsset 이 템플릿 클래스인데 virtual getReflection 이 존재하는 CGAssetBase 를 상속받으니까
		제대로 버철 함수의 호출 처리가 안된다 - 는 것 같다!! 계속 이상한 포인터를 접근하여 쓰레기값을 받아온다!

		CGAsset 은 CGObject 가 아니게 하고, CGInterf_Reflection 을 감싸서 버철 함수가 아닌 걸로 파일에 읽고 쓰기를 처리하는 걸로 하자
		즉 CGAsset 은 Reflection 처리를 하드코딩으로 해야겠다

		getReflection2 없애기
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
