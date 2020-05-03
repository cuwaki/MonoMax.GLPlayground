#pragma once

#include "../GCommonIncludes.h"
#include "../Interfaces/CGInterf_Reflection.h"
#include "CGActor.h"

namespace MonoMaxGraphics
{
	enum class EActorLayer : uint8
	{
		System = 0,
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
		virtual void MakeDefault() override;

		void ArrangeActor(const CGActor& templateActor);
		void OverrideActor(CGActor& arrangedActor);

	public:
		virtual SGReflection& getReflection();

	protected:
		CUniqPtr<ReflectionStruct> reflMap_;
		TActorLayers<CSharPtr<CGActor>> actorLayers_;
	};
};