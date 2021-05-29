#pragma once

#include "GECommonIncludes.h"
#include <array>

namespace SMGE
{
	namespace Physics
	{
		using PFloat = float;
		using PVec3 = glm::vec<3, PFloat>;

//#define kg
//#define newton
//#define ms
		enum EMaterial
		{
			NONE,
			IRON,
			// ...
			MAX,
		};

		namespace Constants
		{
			constexpr PFloat _1KG = 1000.;
			constexpr PFloat _1M = 1.;								// GL 기본값 기준
			constexpr PFloat _DEFAULT_GRAVITY_Y_MS2 = -9.8 * _1M;	// GL Y축 기준
			constexpr PFloat _1N = _1KG * _DEFAULT_GRAVITY_Y_MS2;	// 1뉴턴
		};

		class CRigidBody
		{
		public:
			CRigidBody(const class CWorld& world);
			~CRigidBody();

			PVec3 Weight() const;
			PVec3 NeedNormalForce(PVec3 contactPlaneNormal, PVec3* outPerpForce = nullptr) const;
			PVec3 CurrentFrictionalForce(EMaterial contactPlaneMat, PVec3 contactPlaneNormal, bool isKinetic) const;

			bool IsSliding(EMaterial contactPlaneMat, PVec3 contactPlaneNormal) const;
			PVec3 ComputeSlidingAccel(EMaterial contactPlaneMat, PVec3 contactPlaneNormal) const;

			bool IsActive() const { return isActive_; }
			void SetActive(bool a);

		protected:
			void OnChangedActive();

		protected:
			PFloat mass_ = 0.;
			PVec3 antiGravity_{ 0. };
			PVec3 normalForce_{ 0. };
			EMaterial material_ = EMaterial::NONE;

			PVec3 ownForce_;

			bool isActive_ = false;

		protected:
			const class CWorld& world_;
		};

		class CWorld
		{
		public:
			CWorld();
			void LoadConfigurations();

		public:
			void Tick(float td);

			void Register(CRigidBody& rb) const;
			void Unregister(CRigidBody& rb) const;

			PFloat GravityY() const { return gravity_.y; }
			PVec3 Gravity() const;

			PFloat FrictionalForce(EMaterial going, EMaterial contacting, bool isKinetic) const;

			// runtime datas
		protected:
			mutable CVector<std::reference_wrapper<CRigidBody>> rigidBodies_;

			// configurations
		protected:
			PVec3 gravity_{ 0. };
			std::array< std::array< std::array<PFloat, 2>, EMaterial::MAX>, EMaterial::MAX> frictionalForceTable_;
		};

		extern CWorld GWorld;
	}
};
