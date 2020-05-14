#pragma once

#include "../GCommonIncludes.h"
#include "../Interfaces/CGInterf_Reflection.h"
#include "CGActor.h"

namespace MonoMaxGraphics
{
	enum class EActorLayer : uint8
	{
		System = 0,	// 카메라, 매니저 ...
		Game,
		Max,
	};

	class CGMap;

	template<typename T>
	using TActorLayers = CVector<CVector<T>>;

	struct SGRefl_Map : public SGReflection
	{
		SGRefl_Map(CGMap& map);

		TActorLayers<SGRefl_Actor> actorLayers_;
	};

	class CGMap : public CGObject, public CGInterf_Reflection
	{
	public:
		using ReflectionStruct = SGRefl_Map;

		using Super = CGObject;
		friend struct ReflectionStruct;

	public:
		CGMap();

		virtual void CGCtor() override;
		virtual void CopyFromTemplate(const CGObject& templateObj) override;

		void Activate();

		CGActor& ArrangeActor(const CGActor& templateActor);
		CGActor& OverrideActor(CGActor& arrangedActor);

	public:
		virtual SGReflection& getReflection() override;
		SGReflection& getReflection2();

	protected:
		CUniqPtr<ReflectionStruct> reflMap_;
		TActorLayers<CSharPtr<CGActor>> actorLayers_;
	};
};