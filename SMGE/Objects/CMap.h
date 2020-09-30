#pragma once

#include "../Interfaces/CInt_Reflection.h"
#include "CActor.h"
#include "../RTTI.hpp"

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
		DECLARE_RTTI_CObject(CMapComponent)

	public:
		using TReflectionStruct = SGRefl_Map;

		using Super = CObject;
		friend struct TReflectionStruct;

	public:
		CMap(CObject* outer);

		void Ctor();

		virtual void Tick(float);
		virtual void Render(float);

		virtual void ProcessPendingKills();

		//CActor& SpawnDefaultActor(const CActor& templateActor, bool isDynamic);
		CActor& FinishSpawnActor(CActor& arrangedActor);
		CActor* FindActor(TActorKey ak);
		CSharPtr<CActor>&& RemoveActor(TActorKey ak);
		const CVector<CSharPtr<CActor>>& GetActors(EActorLayer layer) const;

		void StartToPlay();
		void FinishPlaying();
		bool IsStarted() { return isStarted_; }

	protected:
		CActor& SpawnActorINTERNAL(CObject* newObj, bool isDynamic)
		{
			CSharPtr<CActor> newActor(DCast<CActor*>(newObj));

			if (isDynamic == true)
			{	// DynamicActorKey
				newActor->actorKey_ = DynamicActorKey++;
			}

			auto rb = actorLayers_[EActorLayer::Game].emplace_back(std::move(newActor));
			rb->OnSpawnStarted(this, isDynamic);

			return *std::static_pointer_cast<CActor>(rb);
		}

	public:
		virtual const CString& getClassRTTIName() const override { return GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		CUniqPtr<TReflectionStruct> reflMap_;

	protected:
		// runtime
		TActorLayers<CSharPtr<CActor>> actorLayers_;
		bool isStarted_ = false;

		static TActorKey DynamicActorKey;	// 테스트 코드

	public:
		// 애셋등에서 리플렉션으로 액터를 생성할 때 사용
		template<typename... Args>
		CActor& SpawnActorDEFAULT(const std::string& classRTTIName, bool isDynamic, Args&&... args)
		{
			auto newObj = RTTI_CObject::NewDefault(classRTTIName, std::forward<Args>(args)...);
			return static_cast<CActor&>(SpawnActorINTERNAL(newObj, isDynamic));
		}

		// 코드에서 하드코딩으로 액터를 스폰할 때 사용
		template<typename ActorT, typename... Args>
		ActorT& SpawnActorVARIETY(bool isDynamic, Args&&... args)
		{
			auto newObj = RTTI_CObject::NewVariety<ActorT>(SMGE::GetClassRTTIName<ActorT>(), std::forward<Args>(args)...);
			return static_cast<ActorT&>(SpawnActorINTERNAL(newObj, isDynamic));
		}
	};

	namespace Globals
	{
		extern CMap* GCurrentMap;
	}
};