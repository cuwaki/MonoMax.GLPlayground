#pragma once

#include "GECommonIncludes.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <array>
#include <forward_list>
#include <set>

#if defined(_DEBUG) || defined(DEBUG)
	#define PHYSICS_DEBUG
#else
#endif

namespace SMGE
{
	namespace Physics
	{
		// 모든 계산은 오른손 법칙과 GL축 을 기준으로 한다

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		using PFloat = float;
		using PScalar = float;
		using PVec3 = glm::vec<3, PFloat>;
		using PVector = glm::vec<3, PFloat>;
		using PUniqueKey = uint32_t;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		PFloat ConvertRotation2Radian(PFloat rotation);	// Rotation == 회전 수
		PFloat ConvertRadian2Rotation(PFloat radian);
		PFloat ConvertRadian2Degree(PFloat radian);
		PFloat ConvertDegree2Radian(PFloat degree);

		template<typename VEC3>
		void GuranteeMore0(VEC3& outVec3)
		{
			outVec3.x = std::max<PFloat>(outVec3.x, 0.);
			outVec3.y = std::max<PFloat>(outVec3.y, 0.);
			outVec3.z = std::max<PFloat>(outVec3.z, 0.);
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		namespace Constants
		{
			constexpr PFloat _Second = 1000.;
			constexpr PFloat _Minute = _Second * 60.;

			constexpr PFloat _FramePerSecond = 60.;
			constexpr PFloat _AFrameTime = _Second / _FramePerSecond;

			constexpr PFloat _1KG = 1000.;
			constexpr PFloat _1M = 1.;								// GL 기본값 기준
			constexpr PFloat _DEFAULT_GRAVITY_Y_MS2 = -9.8;			// GL Y축 기준

			constexpr PFloat _InfiniteMass = std::numeric_limits<PFloat>::max();
		};

		enum EMaterial
		{
			NONE,
			IRON,
			// ...
			MAX,
		};

		namespace NewtonRules
		{
			// 정의 - td 동안 위치의 변화량
			PVec3 ComputeVelocity(const PVec3& Df, const PVec3& Di, PFloat td);	// D == displacement
			// 정의 - td 동안 속도의 변화량
			PVec3 ComputeAcceleration(const PVec3& Vf, const PVec3& Vi, PFloat td);

			// 정리 - 일 == 어떤 거리를 움직이게 하는 힘 또는 힘이 이동 방향에 작용한 량
			// W == F * displacement
			// W == dot( displacement, F )
			PScalar ComputeWork(const PVec3& F, const PVec3& displacement);	// 일의 단위 - 뉴턴미터 (Nm) 또는 줄 (J)

			// 정리 - 운동 에너지 == 물체가 운동하기 때문에 갖게 되는 에너지
			// KE == 1/2 * mass * |velocity| ^ 2
			PScalar ComputeKineticEnergy(PScalar mass, const PVec3& velocity);	// 운동에너지의 단위 - 일과 동일

			// 정리 - 일, 에너지 정리
			// W == delta(KE) == KEf - KEi
			PScalar ComputeWork(PScalar KEf, PScalar KEi);

			// 정리 - 중력 퍼텐셜 에너지, 단위는 줄
			// GPE == mass * gravityAccel * height
			PScalar ComputeGravitationalPotentialEnergy(PScalar mass, PScalar gravityY, PScalar height);
			PScalar ComputeGPE(PScalar m, PScalar g, PScalar h);

			// 정리 - 역학적 에너지 보존의 법칙 1 - 롤러코스터를 예로 들면 위아래로 움직이는 동안 운동에너지가 퍼텐션 에너지로 바뀌거나 반대거나 하게 되는 것이다
			// KEi + PEi == KEf + PEf

			// 정리 - 역학적 에너지 보존의 법칙 2 - 손실 E0 추가
			// KEi + PEi == KEf + PEf + E0

			// 정리 - 정규화된 반사 벡터 구하기
			// vP == dot(N, -Vi) * N
			// Vf == 2 * vP + Vi
			PVec3 ComputeReflectionVectorN(const PVec3& N, const PVec3& Vi);

			// 정리 - 운동량 == 모멘텀 momentum / 질량이 클수록 운동량이 크다
			// P = mass * velocity
			// 단위 kg * m/s
			PVec3 ComputeMomentum(PScalar mass, const PVec3& velocity);

			// 정리 - 충격량 == 임펄스 impluse
			// F == mass * accel == mass * (Vf - Vi) / td == (mass * Vf - mass * Vi) / td
			// F == (Pf - Pi) / td
			// 정리 - 충격량, 운동량 정리
			// I == F * td == (Pf - Pi)
			// 즉, 충격량은 td 가 없어도 계산이 가능하다
			PVec3 ComputeImpulse(PScalar mass, const PVec3& acc);
			PVec3 ComputeImpulse(PScalar mass, const PVec3& Vi, const PVec3& Vf);

			// 정리 - 뉴턴의 제 3법칙 - 작용, 반작용의 법칙
			// 한 물체가 다른 물체에 힘을 가하면 다른 물체가 한 물체에게 크기가 같고 방향이 반대인 힘을 행사한다
			// F1 == -F2
			// 그러므로 정리 - 운동량 보존의 법칙 - 운동량의 총합은 두 물체가 충돌할 때 항상 똑같이 유지된다, 즉 운동량은 여기서 저기로 옮겨갈 뿐이다
			// P1/td == -P2/td
			// m1 * Vi1 + m2 * Vi2 == m1 * Vf1 + m2 * Vf2
			// 고로 Pi1 + Pi2 == Pf1 + Pf2
			void ComputeConservedMomentum(PScalar mass1, const PVec3& Vi1, PScalar mass2, const PVec3& Vi2, PVec3& outVf1, PVec3& outVf2);

			// 정리 - 탄성계수 또는 반발계수
			// https://terms.naver.com/entry.naver?docId=4389969&ref=y&cid=60217&categoryId=60217
			PFloat ComputeRestitutionCoefficient(const PVec3& Vi1, const PVec3& Vi2, const PVec3& Vf1, const PVec3& Vf2);

			// 정리 - 선형 탄성 충돌 - 1일 때 완전탄성, 0일 때 완전비탄성, 그 중간일 때 비탄성충돌
			// 탄성계수 또는 반발계수 : e -> 0. <= e <= 1.
			// Vf2 - Vf1 == e * (Vi2 - Vi1)
			// 질량이 서로 다른 두 물체의 비탄성충돌 계산을 위한 공식이 정리된 곳
			// https://terms.naver.com/entry.naver?docId=3537078&cid=60217&categoryId=60217
			void ApplyRestitutionCoefficient(PFloat e, PScalar mass1, const PVec3& Vi1, PScalar mass2, const PVec3& Vi2, PVec3& outVf1, PVec3& outVf2);

			// 이하 회전 운동 관련
			// 단위 - 라디안
			PFloat ComputeAngularDisplacement(PFloat arcLength, PFloat radius);

			// 단위 - r/s == 라디안/초
			PScalar ComputeAngularVelocity(PFloat anglDispI, PFloat anglDispF, PFloat td);

			// 단위 - r/s^2 == 라디안/초의 제곱
			PScalar ComputeAngularAcceleration(PFloat anglVelI, PFloat anglVelF, PFloat td);

			PScalar ComputeTangentVelocity(PScalar anglVelInstantaneous, PFloat radius);
			PScalar ComputeTangentAcceleration(PScalar anglAccInstantaneous, PFloat radius);

			// 관성 모먼트 I
			// 다만 관성모먼트는 물체의 형상에 따라 달라진다. 이 경우는 모든 질량이 가장자리에 몰려있는 원통이나 고리에만 해당한다.
			PScalar ComputeInertialMoment(PScalar mass, PFloat radius);

			// 정리 - 토크 / 물체를 회전하게 만드는 힘
			// 각 운동에서 F = ma 에 해당한다, T = Ia
			// 단위 - Nm
			PScalar ComputeTorque(PScalar mass, PFloat radius, PScalar anglAcc);

			// 정리 - 회전 운동 에너지
			// 이제 에너지 보존의 법칙에 회전운동에너지를 포함해야한다.
			// KEti + PEi + KEri == KEtf + PEf + KErf + E0
			PScalar ComputeRotationalKineticEnergy(PScalar I, PScalar anglVel);

			// 정리 - 각 운동량
			PScalar ComputeAngluarMomentum(PScalar I, PScalar anglVel);

			// 정리 - 각 충격량
			PScalar ComputeAngularImpulse(PScalar mass, PScalar anglVelI, PScalar anglVelF);
		}

		struct SContactPlane
		{
			SContactPlane(PUniqueKey uk, PVec3 n, EMaterial m, class CRigidBody* rb);

			const PUniqueKey uniqueKey_;
			const class CRigidBody* from_;

			PVec3 normal_;
			EMaterial material_;

			bool operator<(const SContactPlane& other) const
			{
				return this->uniqueKey_ < other.uniqueKey_;
			}
			bool operator==(const SContactPlane& other) const
			{
				return this->uniqueKey_ == other.uniqueKey_;
			}
		};

		struct SForce
		{
			SForce(PVec3 f, class CRigidBody* rb);

			PFloat durationMS_;
			const PVec3 force_;
			const class CRigidBody* from_;

			bool IsExpired() const;
			bool IsImpact() const;
			bool IsThrust() const { return !IsImpact(); }
		};

		class CRigidBody
		{
		public:
			CRigidBody(const class CWorld& world);
			~CRigidBody();

			void Tick(float dt);

			void SetMass(PFloat m) { mass_ = m; }
			PFloat Mass() const { return mass_; }
			PVec3 Weight() const;
			EMaterial Material() const { return material_; }
			PVec3 NeedNormalForce(PVec3 contactPlaneNormal, PVec3* outPerpForce = nullptr) const;
			PVec3 CurrentFrictionalForce(EMaterial contactPlaneMat, PVec3 contactPlaneNormal, bool isKinetic) const;

			bool IsSliding(EMaterial contactPlaneMat, PVec3 contactPlaneNormal) const;
			PVec3 ComputeSlidingForce(EMaterial contactPlaneMat, PVec3 contactPlaneNormal) const;

			bool IsActive() const { return isActive_; }
			void SetActive(bool a);

			void SetOwnUniformVelocity(PVec3 ouv);
			PVec3 GetOwnUniformVelocity() const;

			void SetPhysicsPosition(PVec3 pos);
			PVec3 GetPhysicsPosition() const;
			PVec3 GetOldPhysicsPosition() const;

			void AddForce(SForce f);
			PVec3 GetTotalThrustForce() const;

			PVec3 ComputeAccelerationFromForce(PVec3 totalForce);

			const auto& GetContactPlanes() const;
			bool IsContacting(CRigidBody* other) const;
			auto FindContactPlane(PUniqueKey collUniKey) const;
			void OnContactStart(CRigidBody* other, PUniqueKey collUniKey, PVec3 normal, EMaterial mat);
			void OnContactEnd(CRigidBody* other, PUniqueKey collUniKey);

		protected:
			void OnChangedActive();

		protected:
			PScalar mass_ = 0.;
			PVec3 antiGravity_{ 0. };
			PVec3 normalForce_{ 0. };	// 필요없다 지워라
			EMaterial material_ = EMaterial::NONE;

			PVec3 position_{ 0. };
			PVec3 oldPosition_{ 0. };

			PVec3 ownUniformVelocity_{ 0. };	// 스스로 내는 등속도
			PVec3 velocity_{ 0. };	// 현재 속도

			bool isActive_ = false;

			std::set<SContactPlane> contactPlanes_;

			std::forward_list<SForce> thrustForces_;

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

			PScalar GravityY() const { return gravity_.y; }
			PVec3 Gravity() const;

			PFloat FrictionalForce(EMaterial going, EMaterial contacting, bool isKinetic) const;

			// runtime datas
		protected:
			mutable CVector<std::reference_wrapper<CRigidBody>> rigidBodies_;	// mutable for Register, Unregister

			// configurations
		protected:
			PVec3 gravity_{ 0. };
			std::array< std::array< std::array<PFloat, 2>, EMaterial::MAX>, EMaterial::MAX> frictionalForceTable_;
		};

		extern CWorld GWorld;
	}
};
