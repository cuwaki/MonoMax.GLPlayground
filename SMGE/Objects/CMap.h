#pragma once

#include "../Interfaces/CInt_Reflection.h"
#include "../Components/CBoundComponent.h"
#include "../CQuadtree.h"
#include <set>

namespace SMGE
{
	namespace MapConst
	{
		constexpr float OctreeLeafWidth = 5;	// 5미터
		constexpr int32 OctreeDepth = 8;

		constexpr float MaxX = OctreeLeafWidth * 256;	// 256 == std::pow(2, OctreeDepth) - 최종 옥트리 전체 한변의 크기는 OctreeLeafWidth * MaxX 가 된다
		constexpr float MaxY = MaxX;
		constexpr float MaxZ = MaxX;
	}

	using MapOcTree = COctree<std::set<CActor *>>;

	// deprecated
	//enum class EActorLayer : uint8
	//{
	//	System = 0,	// 카메라, 매니저 ...
	//	Game,
	//	Max,
	//};

	class CMap;

	struct SGRefl_Map : public SGReflection
	{
		using Super = SGReflection;

		SGRefl_Map(CMap& map);

		CVector<std::reference_wrapper<SGReflection>> mapActorsRefl_;

		virtual const SGReflection& operator>>(CWString& out) const override;
		virtual SGReflection& operator<<(const CVector<TupleVarName_VarType_Value>& in) override;

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

		virtual void ProcessPendingKill(CActor* actor);
		virtual void AddMapActor(CActor* actor);

		CActor* FindActor(TActorKey ak);
		const CVector<CActor*>& GetMapActors() const;
		CVector<CActor*> QueryActors(const SAABB& aabb) const;

		void BeginPlay();
		void FinishPlaying();
		bool IsBeganPlay() const		{ return isBeganPlay_ == true && isBeginningPlay_ == false; }
		bool IsBeginningPlay() const	{ return isBeginningPlay_ == true && isBeganPlay_ == false; }

		class CCameraActor* GetCurrentCamera() const;

	public:
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

		bool IsTemplate() const;

	protected:
		void OnPostBeginPlay();

		void changeCurrentCamera(class CCameraActor* camA);
		void cameraFrustumCulling();

		void RemoveActorFromOctree(CActor* actor);

	protected:
		UPtr<TReflectionStruct> reflMap_;

	protected:
		// runtime
		MapOcTree mapOctree_;
		CVector<CActor*> mapActorsW_;
		bool isBeganPlay_ = false;
		bool isBeginningPlay_ = false;
		CVector<CActor*> actorsAroundFrustum_;

		class CCameraActor* currentCamera_;	// 물체를 비추는 카메라
		class CCameraActor* cullingCamera_;	// mapOctree_ 와 협동하여 렌더링을 위해 컬링하는 카메라
		const bool isFrustumCulling_;
	};

	namespace Globals
	{
		extern CMap* GCurrentMap;
	}
};