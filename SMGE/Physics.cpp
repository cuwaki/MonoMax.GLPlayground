#pragma once

#include "GECommonIncludes.h"
#include "Physics.h"

namespace SMGE
{
	namespace Physics
	{
		CWorld GWorld;

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
			return -1 * world_.FrictionalForce(material_, contactPlaneMat, isKinetic) * nf;
		}

		// 접면에서 미끌어지는가?
		bool CRigidBody::IsSliding(EMaterial contactPlaneMat, PVec3 contactPlaneNormal) const
		{
			PVec3 perpForce;

			// 최적화를 위한 중복 코드 허용 - 최적화 - 최근 계산한 결과를 갖고 있으면 될 것 같다, 보통 연속으로 쓰이므로...
			const auto nf = NeedNormalForce(contactPlaneNormal, &perpForce);
			const auto staticFrictForce = -1 * world_.FrictionalForce(material_, contactPlaneMat, false) * nf;

			return glm::length2(perpForce) > glm::length2(staticFrictForce);
		}

		// 접면에서 미끌어지는 가속도를 구한다
		PVec3 CRigidBody::ComputeSlidingAccel(EMaterial contactPlaneMat, PVec3 contactPlaneNormal) const
		{
			PVec3 ret{ 0. };

			PVec3 perpForce;

			// 최적화를 위한 중복 코드 허용 - 최적화 - 최근 계산한 결과를 갖고 있으면 될 것 같다, 보통 연속으로 쓰이므로...
			const auto nf = NeedNormalForce(contactPlaneNormal, &perpForce);
			const auto staticFrictForce = -1 * world_.FrictionalForce(material_, contactPlaneMat, false) * nf;

			auto isSliding = glm::length2(perpForce) > glm::length2(staticFrictForce);
			if (isSliding)
			{	// 평면에 수평으로 작용하는 힘이 정지 마찰력 즉 이동을 시작하지 못하게 하는 힘보다 큰 상황
				const auto kineticFrictForce = -1 * world_.FrictionalForce(material_, contactPlaneMat, true) * nf;
				const auto slidingForce = perpForce - kineticFrictForce;	// 평면에 수평으로 작용하는 힘이 이동 마찰력보다 크므로 그 차이만큼의 힘이 작용하게 된다

				ret = slidingForce / mass_;	// F = ma 이므로 a = F/m 이니까
			}

			return ret;
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

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CWorld::CWorld()
		{
			gravity_ = { 0.f, Constants::_DEFAULT_GRAVITY_Y_MS2, 0.f };
		}

		void CWorld::Tick(float td)
		{
			for (auto& rb : rigidBodies_)
			{
				// 중력 적용
				// rb 스스로의 힘 적용
				// 최종 가속도 적용
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
