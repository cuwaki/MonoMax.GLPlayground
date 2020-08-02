#pragma once

#include "../Interfaces/CInt_Reflection.h"
#include "CActor.h"

namespace SMGE
{
	enum class EActorLayer : uint8
	{
		System = 0,	// 카메라, 매니저 ...
		Game,
		Max,
	};

	class CMap;

	template<typename T>
	using TActorLayers = CVector<CVector<T>>;

	struct SGRefl_Map : public SGReflection
	{
		using Super = SGReflection;

		SGRefl_Map(CMap& map);

		TActorLayers<SGRefl_Actor> actorLayersREFL_;

		virtual operator CWString() const;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& variableSplitted) override;

	protected:
		void linkINST2REFL();

	public :
		CMap& outerMap_;
	};

	class CMap : public CObject, public CInt_Reflection
	{
	public:
		using TReflectionStruct = SGRefl_Map;

		using Super = CObject;
		friend struct TReflectionStruct;

	public:
		CMap(CObject* outer);
		CMap(CObject* outer, const CMap& templateInst);

		virtual void Ctor() override;
		virtual void Tick(float);
		virtual void Render(float);

		CActor& SpawnDefaultActor(const CActor& templateActor, bool isDynamic);
		CActor& ArrangeActor(CActor& arrangedActor);
		CActor* FindActor(TActorKey ak);
		CSharPtr<CActor>&& RemoveActor(TActorKey ak);

		void StartToPlay();
		void FinishPlaying();
		bool IsStarted() { return isStarted_; }

	public:
		virtual CWString getClassName() override { return className_; }
		virtual SGReflection& getReflection() override;

	protected:
		CUniqPtr<TReflectionStruct> reflMap_;

	protected:
		// runtime
		TActorLayers<CSharPtr<CActor>> actorLayers_;
		bool isStarted_ = false;
	};
};