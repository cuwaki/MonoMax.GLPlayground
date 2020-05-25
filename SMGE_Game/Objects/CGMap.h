#pragma once

#include "../GCommonIncludes.h"
#include "../Interfaces/CGInterf_Reflection.h"
#include "CGActor.h"

namespace SMGE
{
	struct SMGEException : public std::exception
	{
		SMGEException(const CWString& exceptionMsg) : exceptionMsg_(exceptionMsg) {}
		CWString exceptionMsg_;
	};

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
		using Super = SGReflection;

		SGRefl_Map(CGMap& map);

		TActorLayers<SGRefl_Actor> actorReflLayers_;

		virtual operator CWString() const;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& variableSplitted) override;

	protected:
		void linkActorReferences();

	public :
		CGMap* outerMap_;
	};

	class CGMap : public CGObject, public CGInterf_Reflection
	{
	public:
		using TReflectionStruct = SGRefl_Map;

		using Super = CGObject;
		friend struct TReflectionStruct;

	public:
		CGMap();
		CGMap(const CGMap& templateInst);

		virtual void CGCtor() override;
		virtual void Tick(float);
		virtual void Render(float);

		CGActor& SpawnDefaultActor(const CGActor& templateActor, bool isDynamic);
		CGActor& ArrangeActor(CGActor& arrangedActor);

		void StartToPlay();
		bool IsStarted() { return isStarted_; }

	public:
		virtual SGReflection& getReflection() override;

	protected:
		CUniqPtr<TReflectionStruct> reflMap_;

	protected:
		// runtime
		TActorLayers<CSharPtr<CGActor>> actorLayers_;
		bool isStarted_ = false;
	};
};