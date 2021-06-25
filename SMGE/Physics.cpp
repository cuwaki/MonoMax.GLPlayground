#pragma once

#include "GECommonIncludes.h"
#include "Physics.h"

namespace SMGE
{
	namespace Physics
	{
		CWorld GWorld;

		PFloat ConvertRotation2Radian(PFloat rotation)
		{
			return rotation * 2. * M_PI;
		}
		PFloat ConvertRadian2Rotation(PFloat radian)
		{
			return radian / 2. * M_PI;
		}
		PFloat ConvertRadian2Degree(PFloat radian)
		{
			return radian * M_PI / 180.;
		}
		PFloat ConvertDegree2Radian(PFloat degree)
		{
			return degree * 180. / M_PI;
		}

		namespace NewtonRules
		{
			PVec3 ComputeVelocity(const PVec3& Df, const PVec3& Di, PFloat td)
			{
#ifdef PHYSICS_DEBUG
				assert(td > 0.);
#endif
				return (Df - Di) / td;
			}
			PVec3 ComputeAcceleration(const PVec3& Vf, const PVec3& Vi, PFloat td)
			{
#ifdef PHYSICS_DEBUG
				assert(td > 0.);
#endif
				return (Vf - Vi) / td;
			}

			PScalar ComputeWork(const PVec3& F, const PVec3& displacement)
			{
				return glm::dot(displacement, F);	// 이게 아래와 동일하다

				//const auto normDispl = glm::normalize(displacement);	// 1. 변위의 방향으로 작동한 힘을 구함
				//const auto FOnDispl = glm::dot(normDispl, F);	// 1. 변위의 방향으로 작동한 힘을 구함
				//return glm::length(F) * glm::length(displacement);
			}

			PScalar ComputeKineticEnergy(PScalar mass, const PVec3& velocity)
			{
#ifdef PHYSICS_DEBUG
				assert(mass >= 0.);
#endif
				const auto velLength = glm::length(velocity);
				return (0.5 * mass) * (velLength * velLength);
			}

			PScalar ComputeWork(PScalar KEf, PScalar KEi)
			{
				return KEf - KEi;
			}

			PScalar ComputeGravitationalPotentialEnergy(PScalar mass, PScalar gravityY, PScalar height)
			{
#ifdef PHYSICS_DEBUG
				assert(mass >= 0.);
#endif
				return mass * gravityY * height;
			}
			PScalar ComputeGPE(PScalar m, PScalar g, PScalar h)
			{
				return ComputeGravitationalPotentialEnergy(m, g, h);
			}

			PVec3 ComputeReflectionVectorN(const PVec3& N, const PVec3& Vi)
			{
#ifdef PHYSICS_DEBUG
				assert(IsNearlyEqual(glm::length(N), 1.));
#endif
				const auto P = glm::dot(N, PFloat(-1.) * Vi) * N;
				return glm::normalize( (PFloat(2.) * P) + Vi );
			}

			PVec3 ComputeMomentum(PScalar mass, const PVec3& velocity)
			{
#ifdef PHYSICS_DEBUG
				assert(mass >= 0.);
#endif
				return mass * velocity;
			}

			PVec3 ComputeImpulse(PScalar mass, const PVec3& Vi, const PVec3& Vf)
			{
#ifdef PHYSICS_DEBUG
				assert(mass >= 0.);
#endif
				const auto pf = ComputeMomentum(mass, Vf), pi = ComputeMomentum(mass, Vi);
				return pf - pi;
			}

			PVec3 ComputeConservedMomentum(PScalar mass1, const PVec3& Vi1, PScalar mass2, const PVec3& Vi2, PVec3& outVf1, PVec3& outVf2)
			{
#ifdef PHYSICS_DEBUG
				assert(mass1 >= 0.);
				assert(mass2 >= 0.);
#endif

				const auto pi1 = ComputeMomentum(mass1, Vi1), pi2 = ComputeMomentum(mass2, Vi2);

				const auto Pi = pi1 + pi2;
				const auto Pf = Pi;	// pi1 + pi2 == pf1 + pf2

				// Pf == m1 * vf1 + m2 * vf2
				outVf2 = Pf / mass2;	// (m1 * vf1 / m2) == -vf2
				outVf1 = Pf / mass1;

				return Pf;
			}

			PFloat ComputeRestitutionCoefficient(const PVec3& Vi1, const PVec3& Vi2, const PVec3& Vf1, const PVec3& Vf2)
			{
				return glm::length(Vf2 - Vf1) / glm::length(Vi1 - Vi2);
			}

			void ApplyRestitutionCoefficient(PFloat e, PScalar mass1, const PVec3& Vi1, PScalar mass2, const PVec3& Vi2, PVec3& outVf1, PVec3& outVf2)
			{
#ifdef PHYSICS_DEBUG
				assert(e >= 0. && e <= 1.);
				assert(mass1 >= 0.);
				assert(mass2 >= 0.);
#endif
				// 참고로 정면충돌이 아닌 경우에는 충돌면에 수직 방향의 성분 벡터를 고려하면 된다.

				const auto m1_plus_m2 = mass1 + mass2;
				const auto _1_plus_e = PFloat(1.) + e;

				outVf1 = ((mass1 - e * mass2) * Vi1 + (mass2 * _1_plus_e) * Vi2) / m1_plus_m2;
				outVf2 = ((mass2 - e * mass1) * Vi2 + (mass1 * _1_plus_e) * Vi1) / m1_plus_m2;
			}

			PFloat ComputeAngularDisplacement(PFloat arcLength, PFloat radius)
			{
#ifdef PHYSICS_DEBUG
				assert(arcLength > 0.);
				assert(radius > 0.);
#endif
				const auto thetaRadian = arcLength / radius;
				return thetaRadian;
			}

			PScalar ComputeAngularVelocity(PFloat anglDispI, PFloat anglDispF, PFloat td)
			{
#ifdef PHYSICS_DEBUG
				assert(td > 0.);
#endif
				return (anglDispF - anglDispI) / td;
			}

			PScalar ComputeAngularAcceleration(PFloat anglVelI, PFloat anglVelF, PFloat td)
			{
#ifdef PHYSICS_DEBUG
				assert(td > 0.);
#endif
				return (anglVelF - anglVelI) / td;
			}

			PScalar ComputeTangentVelocity(PScalar anglVelInstantaneous, PFloat radius)
			{
				return anglVelInstantaneous * radius;
			}
			PScalar ComputeTangentAcceleration(PScalar anglAccInstantaneous, PFloat radius)
			{
				return anglAccInstantaneous * radius;
			}

			PScalar ComputeInertialMoment(PScalar mass, PFloat radius)
			{
#ifdef PHYSICS_DEBUG
				assert(mass > 0.);
				assert(radius > 0.);
#endif
				return mass * radius * radius;
			}

			PScalar ComputeTorque(PScalar mass, PFloat radius, PScalar anglAcc)
			{
				return ComputeInertialMoment(mass, radius) * anglAcc;
			}

			PScalar ComputeRotationalKineticEnergy(PScalar I, PScalar anglVel)
			{
				return 0.5 * I * anglVel * anglVel;
			}

			PScalar ComputeAngluarMomentum(PScalar I, PScalar anglVel)
			{
				return I * anglVel;
			}

			PScalar ComputeAngularImpulse(PScalar mass, PScalar anglVelI, PScalar anglVelF)
			{
#ifdef PHYSICS_DEBUG
				assert(mass >= 0.);
#endif
				const auto pf = ComputeAngluarMomentum(mass, anglVelF), pi = ComputeAngluarMomentum(mass, anglVelI);
				return pf - pi;
			}
		}

		SContactPlane::SContactPlane(PUniqueKey uk, PVec3 n, EMaterial m, class CRigidBody* rb) : uniqueKey_(uk), from_(rb)
		{
			normal_ = n;
			material_ = m;
		}

		SForce::SForce(PVec3 f, class CRigidBody* rb) : force_(f), from_(rb)
		{
		}
		bool SForce::IsExpired() const
		{
			return (durationMS_ <= 0./* || (force_.x <= 0. && force_.y <= 0. && force_.z <= 0.)*/);
		}
		bool SForce::IsImpact() const
		{
			return durationMS_ <= 0.;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CRigidBody::CRigidBody(const class CWorld& world) : world_(world)
		{
			world_.Register(*this);
		}

		CRigidBody::~CRigidBody()
		{
			world_.Unregister(*this);
		}

		// 무게는 방향을 포함하며 뉴턴이 단위이다
		PVec3 CRigidBody::Weight() const
		{
			return mass_ * world_.Gravity();	// 뉴턴
		}

		// 무게 벡터와 무게를 받는 경사면의 노멀 을 이용하여 무게를 버티는데 필요한 수직항력을 계산한다, 뉴턴이 단위이다
		PVec3 CRigidBody::NeedNormalForce(PVec3 contactPlaneNormal, PVec3* outPerpForce) const
		{
			const auto w = Weight();
			const auto wn = glm::normalize(w);
#ifdef PHYSICS_DEBUG
			assert(IsNearlyEqual(glm::length(contactPlaneNormal), 1.));
#endif
			const auto cosT = glm::dot(wn, contactPlaneNormal);

			if (outPerpForce)
			{	// 접면에 수평으로 작용하는 힘이 필요하다면 sinT 를 구해서 곱해주면 된다
				const auto sinTsinT = (1. * 1.) - (cosT * cosT);
				*outPerpForce = w * static_cast<PFloat>(std::sqrt(sinTsinT));
			}

			return w * cosT;	// 접면에 수직으로 작용하는 힘 - 수직항력
		}

		// 마찰력을 구한다
		PVec3 CRigidBody::CurrentFrictionalForce(EMaterial contactPlaneMat, PVec3 contactPlaneNormal, bool isKinetic) const
		{
			const auto nf = NeedNormalForce(contactPlaneNormal);
			return world_.FrictionalForce(material_, contactPlaneMat, isKinetic) * -1 * nf;
		}

		// 접면에서 미끌어지는가?
		bool CRigidBody::IsSliding(EMaterial contactPlaneMat, PVec3 contactPlaneNormal) const
		{
			PVec3 perpForce;

			// 최적화를 위한 중복 코드 허용 - 최적화 - 최근 계산한 결과를 갖고 있으면 될 것 같다, 보통 연속으로 쓰이므로...
			const auto nf = NeedNormalForce(contactPlaneNormal, &perpForce);
			const auto staticFrictForce = world_.FrictionalForce(material_, contactPlaneMat, false) * -1 * nf;

			return glm::length2(perpForce) > glm::length2(staticFrictForce);
		}

		// 접면에서 미끌어지는 힘을 구한다
		PVec3 CRigidBody::ComputeSlidingForce(EMaterial contactPlaneMat, PVec3 contactPlaneNormal) const
		{
			PVec3 perpForce;

			// 최적화를 위한 중복 코드 허용 - 최적화 - 최근 계산한 결과를 갖고 있으면 될 것 같다, 보통 연속으로 쓰이므로...
			const auto nf = NeedNormalForce(contactPlaneNormal, &perpForce);
			const auto staticFrictForce = world_.FrictionalForce(material_, contactPlaneMat, false) * -1 * nf;

			auto isSliding = glm::length2(perpForce) > glm::length2(staticFrictForce);
			if (isSliding)
			{	// 평면에 수평으로 작용하는 힘이 정지 마찰력 즉 이동을 시작하지 못하게 하는 힘보다 큰 상황
				const auto kineticFrictForce = world_.FrictionalForce(material_, contactPlaneMat, true) * -1 * nf;
				const auto slidingForce = perpForce - kineticFrictForce;	// 평면에 수평으로 작용하는 힘이 이동 마찰력보다 크므로 그 차이만큼의 힘이 작용하게 된다

				return slidingForce;
			}

			return PVec3{ 0. };
		}

		void CRigidBody::SetActive(bool a)
		{
			if (isActive_ != a)
			{
				isActive_ = a;

				OnChangedActive();
			}
		}

		void CRigidBody::OnChangedActive()
		{
			if (isActive_)
			{	// 방금 활성화됨
			}
			else
			{
			}
		}

		void CRigidBody::SetOwnUniformVelocity(PVec3 ouv)
		{
			ownUniformVelocity_ = ouv;
		}
		PVec3 CRigidBody::GetOwnUniformVelocity() const
		{
			return ownUniformVelocity_;
		}

		void CRigidBody::SetPhysicsPosition(PVec3 pos)
		{
			oldPosition_ = position_;
			position_ = pos;
		}
		PVec3 CRigidBody::GetPhysicsPosition() const
		{
			return position_;
		}
		PVec3 CRigidBody::GetOldPhysicsPosition() const
		{
			return oldPosition_;
		}

		void CRigidBody::AddForce(SForce f)
		{
			if (f.IsImpact())
			{	// 충격은 1회만 적용되고 곧바로 가속도로 계산해서 넣어둬야함
				impactAcceleration_ += ComputeAccelerationFromForce(f.force_);
				return;
			}

			thrustForces_.emplace_front(f);
		}
		PVec3 CRigidBody::GetTotalThrustForce() const
		{
			PVec3 ret{ 0. };
			for (const auto& f : thrustForces_)
				ret += f.force_;
			return ret;
		}

		const auto& CRigidBody::GetContactPlanes() const
		{
			return contactPlanes_;
		}

		auto CRigidBody::FindContactPlane(PUniqueKey collUniKey) const
		{
			return std::find_if(contactPlanes_.begin(), contactPlanes_.end(), [collUniKey](const auto& cp)
				{
					return cp.uniqueKey_ == collUniKey;
				});
		}

		void CRigidBody::OnContactStart(CRigidBody* other, PUniqueKey collUniKey, PVec3 normal, EMaterial mat)
		{
#ifdef PHYSICS_DEBUG
			auto already = FindContactPlane(collUniKey);
			assert(already == contactPlanes_.end());
#endif
			contactPlanes_.emplace(collUniKey, normal, mat, other);
		}

		void CRigidBody::OnContactEnd(CRigidBody* other, PUniqueKey collUniKey)
		{
			contactPlanes_.erase(FindContactPlane(collUniKey));
		}

		void CRigidBody::Tick(float td)
		{
			if (IsActive() == false)
				return;

			const auto gravityAccel = world_.Gravity();

			PVec3 freeFallForce{ 0. };

			// 접면들로부터 미끌어지는 힘 구하기
			size_t contactCount = 0;

			PVec3 totalPerpForce{ 0. }, totalStaticFrictForce{ 0. }, totalKineticFrictForce{ 0. };
			for (auto& cp : GetContactPlanes())
			{
				PVec3 pf{ 0. };
				const auto nf = NeedNormalForce(cp.normal_, &pf);

				const auto minusNF = -1.f * nf;
				const auto staticFrictForce = world_.FrictionalForce(Material(), cp.material_, false) * minusNF;
				const auto kineticFrictForce = world_.FrictionalForce(Material(), cp.material_, true) * minusNF;

				totalPerpForce += pf;
				totalStaticFrictForce += staticFrictForce;
				totalKineticFrictForce += kineticFrictForce;

				++contactCount;
			}

			if (contactCount == 0)
			{	// 접면이 없으면 중력만 적용 - 자유낙하
				freeFallForce = Mass() * gravityAccel;
			}
			else
			{	// 접면이 있으면
				const auto tpf2 = glm::length2(totalPerpForce), tsf2 = glm::length2(totalStaticFrictForce);
				const auto isSliding = tpf2 > tsf2;
				if (isSliding)
				{
					freeFallForce = totalPerpForce - totalKineticFrictForce;
				}
			}

			const auto thrustForce = freeFallForce + GetTotalThrustForce();
			thrustAcceleration_ = ComputeAccelerationFromForce(thrustForce);

			const auto totalVel = (impactAcceleration_ + thrustAcceleration_) * td;
			const auto unifoVal = GetOwnUniformVelocity() * td;
			const auto finalVel = unifoVal + totalVel;

			const auto curPosition = GetPhysicsPosition();
			const auto moved = finalVel * td;
			SetPhysicsPosition(curPosition + moved);

			////////////////////////////////////////////////////////////////////////////////////////////////
			// 종료 처리
			for (auto& f : thrustForces_)
				f.durationMS_ -= td;

			thrustForces_.remove_if([](auto& f) {	return f.IsExpired();	});

			// 가속도의 변화
			const auto gacc = gravityAccel * td;
			impactAcceleration_ += gacc;
			thrustAcceleration_ += gacc;
		}

		PVec3 CRigidBody::ComputeAccelerationFromForce(PVec3 totalForce)
		{
			return totalForce / Mass();
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CWorld::CWorld()
		{
			gravity_ = { 0.f, Constants::_DEFAULT_GRAVITY_Y_MS2, 0.f };
		}

		void CWorld::Tick(float td)
		{
			for (auto& rbW : rigidBodies_)
			{
				rbW.get().Tick(td);
			}
		}

		PVec3 CWorld::Gravity() const
		{
			return gravity_;
		}

		void CWorld::Register(CRigidBody& rb) const
		{
			rigidBodies_.push_back(rb);
		}

		void CWorld::Unregister(CRigidBody& rb) const
		{
			const auto found = std::find_if(rigidBodies_.begin(), rigidBodies_.end(),
				[&rb](const auto& rw)
				{
					return &rw.get() == &rb;
				});
			rigidBodies_.erase(found);
		}

		void CWorld::LoadConfigurations()
		{
			frictionalForceTable_[EMaterial::NONE][EMaterial::NONE][0] = 0.0;
			frictionalForceTable_[EMaterial::NONE][EMaterial::NONE][1] = 0.0;
			frictionalForceTable_[EMaterial::NONE][EMaterial::IRON][0] = 0.0;
			frictionalForceTable_[EMaterial::NONE][EMaterial::IRON][1] = 0.0;

			frictionalForceTable_[EMaterial::IRON][EMaterial::NONE][0] = 0.0;
			frictionalForceTable_[EMaterial::IRON][EMaterial::NONE][1] = 0.0;
			frictionalForceTable_[EMaterial::IRON][EMaterial::IRON][0] = 0.0;
			frictionalForceTable_[EMaterial::IRON][EMaterial::IRON][1] = 0.0;

			// assert 필요상황 - 정지마찰력이 항상 이동마찰력보다 커야한다!
		}

		PFloat CWorld::FrictionalForce(EMaterial going, EMaterial contacting, bool isKinetic) const
		{
			return frictionalForceTable_[going][contacting][isKinetic ? 1 : 0];
		}
	}
};
