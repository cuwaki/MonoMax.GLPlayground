#pragma once

#include "GECommonIncludes.h"
#include "Physics.h"

namespace SMGE
{
	namespace Physics
	{
		CWorld GWorld;

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
#if defined(_DEBUG) || defined(DEBUG)
			assert(IsNearlyEqual(glm::length(contactPlaneNormal), 1.));
#endif
			const auto cosT = glm::dot(wn, contactPlaneNormal);

			if (outPerpForce)
			{	// 평면에 수평으로 작용하는 힘이 필요하다면 sinT 를 구해서 곱해주면 된다
				const auto sinTsinT = (1. * 1.) - (cosT * cosT);
				*outPerpForce = w * static_cast<PFloat>(std::sqrt(sinTsinT));
			}

			return w * cosT;
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
			{	// 충격은 1회만 적용
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

			const auto totalVel = TotalAcceleration() * td;
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
