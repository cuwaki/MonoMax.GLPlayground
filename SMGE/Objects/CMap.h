#pragma once

#include "../Interfaces/CInt_Reflection.h"
#include "CActor.h"
#include "../Components/CBoundComponent.h"
#include "../RTTI.hpp"
#include "../CQuadTree.h"
#include <set>

namespace SMGE
{
	namespace MapConst
	{
		constexpr float OctreeLeafWidth = 50.f;	// 50미터
		constexpr int32 OctreeDepth = 6;

		constexpr float MaxX = OctreeLeafWidth * 64.f;	// 64 == std::pow(2, OctreeDepth)
		constexpr float MaxY = MaxX;
		constexpr float MaxZ = MaxX;
	}

	using ActorOcTree = COcTree<std::set<CActor *>, float>;

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

		TActorLayers<std::reference_wrapper<SGReflection>> actorLayersREFL_;

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

		using This = CMap;
		using Super = CObject;
		friend struct TReflectionStruct;

	public:
		CMap(CObject* outer);

		void Ctor();

		virtual void Tick(float);

		virtual void ProcessPendingKills();

		//CActor& SpawnDefaultActor(const CActor& templateActor, bool isDynamic);
		CActor& FinishSpawnActor(CActor& arrangedActor);
		CActor* FindActor(TActorKey ak);
		CUniqPtr<CActor>&& RemoveActor(TActorKey ak);
		const CVector<CUniqPtr<CActor>>& GetActors(EActorLayer layer) const;
		CVector<CActor*> QueryActors(const SAABB& aabb) const;

		void BeginPlay();
		void FinishPlaying();
		bool IsBeganPlay() const		{ return isBeganPlay_ == true && isBeginningPlay_ == false; }
		bool IsBeginningPlay() const	{ return isBeginningPlay_ == true && isBeganPlay_ == false; }

		class CCameraActor* GetCurrentCamera() const;

	public:
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		CActor& SpawnActorINTERNAL(EActorLayer layer, CObject* newObj, bool isDynamic);
		void OnPostBeginPlay();

		void changeCurrentCamera(class CCameraActor* camA);
		void cameraFrustumCulling();

	protected:
		CUniqPtr<TReflectionStruct> reflMap_;

	protected:
		// runtime
		ActorOcTree actorOctree_;
		TActorLayers<CUniqPtr<CActor>> actorLayers_;
		bool isBeganPlay_ = false;
		bool isBeginningPlay_ = false;
		CVector<CActor*> oldActorsInFrustum_;

		class CCameraActor* currentCamera_;
		bool isFrustumCulling_ = true;
#ifdef IS_EDITOR
		class CCameraActor* cullingCamera_;
#endif
		static TActorKey DynamicActorKey;

	public:
		// 애셋등에서 리플렉션으로 액터를 생성할 때 사용
		template<typename... Args>
		CActor& SpawnActorDEFAULT(EActorLayer layer, const std::string& classRTTIName, bool isDynamic, Args&&... args)
		{
			auto newObj = RTTI_CObject::NewDefault(classRTTIName, std::forward<Args>(args)...);
			return static_cast<CActor&>(SpawnActorINTERNAL(layer, newObj, isDynamic));
		}

		// 코드에서 하드코딩으로 액터를 스폰할 때 사용
		template<typename ActorT, typename... Args>
		ActorT& SpawnActorVARIETY(EActorLayer layer, bool isDynamic, Args&&... args)
		{
			auto newObj = RTTI_CObject::NewVariety<ActorT>(std::forward<Args>(args)...);
			return static_cast<ActorT&>(SpawnActorINTERNAL(layer, newObj, isDynamic));
		}
	};

	namespace Globals
	{
		extern CMap* GCurrentMap;
	}
};