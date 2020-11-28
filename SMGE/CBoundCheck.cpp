#include "CBoundCheck.h"
#include "../MonoMax.EngineCore/RenderingEngine.h"
#include <cmath>

namespace SMGE
{
	bool SAABB::isIntersect(const SAABB& other) const
	{
		if (isIntersectPoints(other))	// 점이 포함된 경우 또는 this가 other 를 완전히 감싼 경우
			return true;

		if (other.isIntersectPoints(*this))	// other 가 this 를 완전히 감싼 경우
			return true;

		// 점이 포함이 아니고 선분이 포함인 경우의 체크 : + 이런 식으로 크로스 된 경우
		bool xyCross = (isXContains(other) && other.isYContains(*this)) || other.isXContains(*this) && this->isYContains(other);
		if (xyCross == true)
		{	// xy 로는 크로스 관계임이 확실해짐

			// zx 로 크로스인지 체크
			bool zxCross = (isZContains(other) && other.isXContains(*this)) || other.isZContains(*this) && this->isXContains(other);
			return zxCross;
		}

		return false;
	}

	bool SAABB::isXContains(const SAABB& other) const
	{
		const auto otherLB = other.lb(), otherRT = other.rt();
		return otherLB.x >= lb_.x && otherLB.x < rt_.x&& otherRT.x >= lb_.x && otherRT.x < rt_.x;
	}
	bool SAABB::isYContains(const SAABB& other) const
	{
		const auto otherLB = other.lb(), otherRT = other.rt();
		return otherLB.y >= lb_.y && otherLB.y < rt_.y&& otherRT.y >= lb_.y && otherRT.y < rt_.y;
	}
	bool SAABB::isZContains(const SAABB& other) const
	{
		const auto otherLB = other.lb(), otherRT = other.rt();
		return otherLB.z >= lb_.z && otherLB.z < rt_.z&& otherRT.z >= lb_.z && otherRT.z < rt_.z;
	}
	bool SAABB::isIntersectPoints(const SAABB& other) const
	{
		return this->isContains(other.lb()) || this->isContains(other.rt()) || this->isContains(other.lt()) || this->isContains(other.rb());
	}

	inline bool SAABB::isContains(const glm::vec3& point) const
	{
		return
			point.x >= lb_.x && point.x < rt_.x&&
			point.y >= lb_.y && point.y < rt_.y&&
			point.z >= lb_.z && point.z < rt_.z;
	}

	inline const glm::vec3& SAABB::lb() const
	{
		return lb_;
	}
	inline const glm::vec3& SAABB::rt() const
	{
		return rt_;
	}
	inline glm::vec3 SAABB::lt() const
	{
		return { lb_.x, rt_.y, lb_.z };
	}
	inline glm::vec3 SAABB::rb() const
	{
		return { rt_.x, lb_.y, rt_.z };
	}

	bool SPointBound::check(const struct SPointBound& point) const
	{
		return loc_ == point.loc_;
	}

	bool SPointBound::operator==(const SPointBound& other) const
	{
		return check(other);
	}

	bool SSphereBound::operator==(const SSphereBound& other) const
	{
		return loc_ == other.loc_ && isNearlyEqual(radius_, other.radius_);
	}

	float SSegmentBound::getSlope2D_XY() const
	{
		auto to = end_ - start_;
		if (isNearlyEqual(to.x, 0.f))
			return NAN;

		return to.y / to.x;
	}

	bool SSegmentBound::check2D_XY(const SSegmentBound& other, glm::vec3& outCross) const
	{
		const auto a = this->getSlope2D_XY(), c = other.getSlope2D_XY();
		if (std::isnan(a) && std::isnan(c))
			return false;

		float crossX, crossY;

		if (std::isnan(a) || std::isnan(c))
		{
			if (std::isnan(c))
			{
				crossX = other.start_.x;
				
				const auto b = this->start_.y;
				crossY = a * crossX + b;
			}
			else
			{
				crossX = this->start_.x;

				const auto d = other.start_.y;
				crossY = c * crossX + d;
			}
		}
		else
		{	// a(x - x0) + y0 == c(x - x1) + y1
			if (isNearlyEqual(a, c))
				return false;

			const auto x0 = this->start_.x, x1 = other.start_.x;
			const auto y0 = this->start_.y, y1 = other.start_.y;

			crossX = (y1 - y0 + a * x0 - c * x1) / (a - c);
			crossY = a * (crossX - x0) + y0;
		}

		if (isInRange(this->start_.x, this->end_.x, crossX) && isInRange(other.start_.x, other.end_.x, crossX) &&
			isInRange(this->start_.y, this->end_.y, crossY) && isInRange(other.start_.y, other.end_.y, crossY))
		{
			outCross.x = crossX;
			outCross.y = crossY;
			return true;
		}

		return false;
	}

	bool SSegmentBound::check2D_XY(const SPointBound& point) const
	{
		const auto a = this->getSlope2D_XY();
		if (std::isnan(a))
		{
			if (isNearlyEqual(start_.x, point.loc_.x) && isInRange(start_.y, end_.y, point.loc_.y))
			{
				return true;
			}
		}
		else
		{
			if (isInRange(start_.x, end_.x, point.loc_.x))
			{
				const auto y = a * (point.loc_.x - start_.x) + start_.y;
				return isNearlyEqual(y, point.loc_.y);
			}
		}

		return false;
	}

	bool SSegmentBound::operator==(const SSegmentBound& other) const
	{
		return start_ == other.start_ && end_ == other.end_;
	}

	bool SSegmentBound::check(const SPlaneBound& plane, glm::vec3& outCross) const
	{
		const auto toEnd = end_ - start_;
		const auto toEndLen = glm::length(toEnd);
		const auto toEndDir = toEnd / toEndLen;
		
		auto baseLineLen = plane.getDistanceOnPlane(start_);
		if (isNearlyEqual(baseLineLen, 0.f))
			return false;

		bool isMinus = std::signbit(baseLineLen);
		if (isMinus)	// 평면 밑에서 쏜 경우
			baseLineLen *= -1.f;
		else
			;	// 평면 위에서 쏜 경우

		const auto cosT = glm::dot(plane.getNormal() * (isMinus ? 1.f : -1.f), toEndDir);
		if (isInRange(0.f, -1.f, cosT))	// 평행 또는 반대방향
			return false;

		const auto hypotenuseLen = baseLineLen * 1.f / cosT;	// 삼각비를 이용
		if (toEndLen < hypotenuseLen)	// 안닿았음
			return false;

		outCross = start_ + toEndDir * hypotenuseLen;
		return true;
	}

	bool SSegmentBound::check(const struct STriangleBound& tri, glm::vec3& outCross) const
	{
		if (check(static_cast<const SPlaneBound&>(tri), outCross) == false)
			return false;

		const auto d = tri.p1_ - tri.p0_;
		const auto e = tri.p2_ - tri.p0_;

		const auto g = outCross - tri.p0_;
		
		const auto gd = glm::dot(g, d);
		const auto ge = glm::dot(g, e);
		const auto dd = glm::dot(d, d);
		const auto ee = glm::dot(e, e);
		const auto de = glm::dot(d, e);

		const auto denom = (de * de - ee * dd);
		const auto u = (gd * de - ge * dd) / denom;
		if (u < 0.f || u > 1.f)
			return false;

		const auto v = (ge * de - gd * ee) / denom;
		if (v < 0.f || v > 1.f)
			return false;

		if ((u + v) > 1.0f)	// 이러면 삼각형 영역의 밖에 있는 것이다
			return false;

		return true;
	}

	bool SSegmentBound::check(const struct SQuadBound& quad, glm::vec3& outCross) const
	{
		if (check(static_cast<const SPlaneBound&>(quad), outCross) == false)
			return false;

		const auto d = quad.p1_ - quad.p0_;
		const auto e = quad.p2_ - quad.p0_;

		const auto g = outCross - quad.p0_;

		const auto gd = glm::dot(g, d);
		const auto ge = glm::dot(g, e);
		const auto dd = glm::dot(d, d);
		const auto ee = glm::dot(e, e);
		const auto de = glm::dot(d, e);

		const auto denom = (de * de - ee * dd);
		const auto u = (gd * de - ge * dd) / denom;
		if (u < 0.f || u > 1.f)
			return false;

		const auto v = (ge * de - gd * ee) / denom;
		if (v < 0.f || v > 1.f)
			return false;

		return true;
	}

	bool SSegmentBound::check(const SSphereBound& sphere, glm::vec3& outCross) const
	{
		const auto radiusSQ = sphere.radius_ * sphere.radius_;

		const auto distanceSQ = getDistanceSquared(start_, sphere.loc_);
		if (distanceSQ < radiusSQ)
		{	// 시점이 구 안에 있는 경우
			outCross = start_;
			return true;
		}

		const auto toEnd = end_ - start_;
		const auto xa = start_.x - sphere.loc_.x, ya = start_.y - sphere.loc_.y, za = start_.z - sphere.loc_.z;

		const auto a = toEnd.x * toEnd.x + toEnd.y * toEnd.y + toEnd.z * toEnd.z;
		const auto b = 2.f * (toEnd.x * xa + toEnd.y * ya + toEnd.z * za);
		const auto c = xa * xa + ya * ya + za * za - radiusSQ;

		const auto discriminant = b * b - 4.f * a * c;
		if (discriminant < 0.f)
			return false;

		const auto sqrtD = std::sqrtf(discriminant);

		const auto plus = (-b + sqrtD) / (2.f * a);
		const auto minus = (-b - sqrtD) / (2.f * a);

		// 일단 작은 값으로 고정
		const auto minT = plus > minus ? minus : plus;
		if (minT > 1.f)
			return false;

		outCross = start_ + toEnd * minT;
		return true;
	}

	bool SSegmentBound::check(const SCubeBound& cube, glm::vec3& outCross) const
	{
		SQuadBound quads[6];
		cube.getQuads(quads);

		const auto segDirInverse = glm::normalize(start_ - end_);

		for (auto& quad : quads)
		{
			const auto cosT = glm::dot(quad.getNormal(), segDirInverse);
			const auto isInFront = isInRange(0.f, 1.f, cosT);

			if (isInFront && check(quad, outCross) == true)
			{	// 선분의 방향의 역이 quad 의 앞쪽이 아니라면 체크할 필요가 없다, cube 는 입체이고 입체라면 앞에서 오는 충돌이 뒷면에 닿을 일이 없기 때문이다
				return true;
			}
		}

		return false;
	}

	SPlaneBound::SPlaneBound(const glm::vec3& norm, const glm::vec3& loc) : SPlaneBound()
	{
		normal_ = glm::normalize(norm);
		d_ = -normal_.x * loc.x + -normal_.y * loc.y + -normal_.z * loc.z;
	}

	float SPlaneBound::getDistanceOnPlane(const glm::vec3& loc) const
	{
		const auto distance = normal_.x * loc.x + normal_.y * loc.y + normal_.z * loc.z + d_;
		return distance;
	}

	bool SPlaneBound::isInFront(const glm::vec3& loc) const
	{
		return getDistanceOnPlane(loc) > 0.f;
	}
	bool SPlaneBound::isOnPlane(const glm::vec3& loc) const
	{
		return isNearlyEqual(getDistanceOnPlane(loc), 0.f);
	}
	bool SPlaneBound::isInBack(const glm::vec3& loc) const
	{
		return getDistanceOnPlane(loc) < 0.f;
	}

	bool SPlaneBound::check(const SPointBound& point) const
	{
		return isOnPlane(point.loc_);
	}

	bool SPlaneBound::check(const struct SPlaneBound& plane, glm::vec3& ourCrossVector, glm::vec3& outCrossPoint) const
	{
		assert(false && "need to imple");
		return false;
	}

	bool SPlaneBound::operator==(const SPlaneBound& other) const
	{
		return normal_ == other.normal_ && isNearlyEqual(d_, other.d_);
	}

	STriangleBound::STriangleBound() : SPlaneBound({ 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f })
	{
		type_ = EBoundType::TRIANGLE;
	}

	STriangleBound::STriangleBound(const glm::vec3& ccw_p0, const glm::vec3& ccw_p1, const glm::vec3& ccw_p2)
	{
		const auto norm = glm::normalize(glm::cross((ccw_p1 - ccw_p0), (ccw_p2 - ccw_p0)));
#if DEBUG || _DEBUG
		if (isNearlyEqual(glm::length(norm), 1.f) == false)
			assert(false && "it is not a triangle");
#endif
		this->SPlaneBound::SPlaneBound(norm, ccw_p0);

		type_ = EBoundType::TRIANGLE;
		p0_ = ccw_p0;
		p1_ = ccw_p1;
		p2_ = ccw_p2;
	}

	bool STriangleBound::operator==(const STriangleBound& other) const
	{
		return p0_ == other.p0_ && p1_ == other.p1_ && p2_ == other.p2_;
	}

	SQuadBound::SQuadBound() : SPlaneBound({ 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f })
	{
		type_ = EBoundType::QUAD;
	}

	SQuadBound::SQuadBound(const glm::vec3& ccw_p0, const glm::vec3& ccw_p1, const glm::vec3& ccw_p2, const glm::vec3& ccw_p3)
	{
		const auto norm = glm::normalize(glm::cross((ccw_p1 - ccw_p0), (ccw_p2 - ccw_p0)));
#if DEBUG || _DEBUG
		if (isNearlyEqual(glm::length(norm), 1.f) == false)
			assert(false && "it is not a quad - normal");
#endif
		this->SPlaneBound::SPlaneBound(norm, ccw_p0);
#if DEBUG || _DEBUG
		if (isOnPlane(ccw_p3) == false)
			assert(false && "it is not a quad - p3");
#endif
		type_ = EBoundType::QUAD;
		p0_ = ccw_p0;
		p1_ = ccw_p1;
		p2_ = ccw_p2;
		p3_ = ccw_p3;
	}

	bool SQuadBound::operator==(const SQuadBound& other) const
	{
		return p0_ == other.p0_ && p1_ == other.p1_ && p2_ == other.p2_ && p3_ == other.p3_;
	}

	SCubeBound::SCubeBound(const glm::vec3& centerPos, const glm::vec3& size, const glm::vec3& eulerDegreesXYZ) : SCubeBound()
	{
		using namespace nsRE::TransformConst;

		glm::mat4 rotMat = glm::rotate(Mat4_Identity, glm::radians(eulerDegreesXYZ.x), WorldAxis[ETypeRot::PITCH]);
		rotMat = glm::rotate(rotMat, glm::radians(eulerDegreesXYZ.y), WorldAxis[ETypeRot::YAW]);
		rotMat = glm::rotate(rotMat, glm::radians(eulerDegreesXYZ.z), WorldAxis[ETypeRot::ROLL]);

		glm::vec3 eulerAxis[3];
		eulerAxis[ETypeAxis::X] = rotMat[ETypeAxis::X];
		eulerAxis[ETypeAxis::Y] = rotMat[ETypeAxis::Y];
		eulerAxis[ETypeAxis::Z] = rotMat[ETypeAxis::Z];
		this->SCubeBound::SCubeBound(centerPos, size, eulerAxis);
	}

	SCubeBound::SCubeBound(const glm::vec3& centerPos, const glm::vec3& size, const glm::vec3(&eulerAxis)[3]) : SCubeBound()
	{
		using namespace nsRE::TransformConst;

		centerPos_ = centerPos;
		size_ = size;
		eulerAxis_[ETypeAxis::X] = eulerAxis[ETypeAxis::X];
		eulerAxis_[ETypeAxis::Y] = eulerAxis[ETypeAxis::Y];
		eulerAxis_[ETypeAxis::Z] = eulerAxis[ETypeAxis::Z];
	}

	bool SCubeBound::operator==(const SCubeBound& other) const
	{
		return centerPos_ == other.centerPos_ && 
			size_ == other.size_ &&
			eulerAxis_[0] == other.eulerAxis_[0] &&
			eulerAxis_[1] == other.eulerAxis_[1] &&
			eulerAxis_[2] == other.eulerAxis_[2];
	}

	void SCubeBound::getQuads(SQuadBound(&outQuads)[6]) const
	{
		using namespace nsRE::TransformConst;

		if (cachedQuads_[0] == false)
		{
			auto LB = [](auto& center, auto& halfU, auto& halfV)
			{
				return center - halfU - halfV;
			};
			auto RB = [](auto& center, auto& halfU, auto& halfV)
			{
				return center + halfU - halfV;
			};
			auto RT = [](auto& center, auto& halfU, auto& halfV)
			{
				return center + halfU + halfV;
			};
			auto LT = [](auto& center, auto& halfU, auto& halfV)
			{
				return center - halfU + halfV;
			};

			const auto halfSize = size_ / 2.f;
			const auto halfXVec = (eulerAxis_[ETypeAxis::X] * halfSize.x);
			const auto halfYVec = (eulerAxis_[ETypeAxis::Y] * halfSize.y);
			const auto halfZVec = (eulerAxis_[ETypeAxis::Z] * halfSize.z);

			// 앞Z+ 뒤Z- 위Y+ 아래Y- 우X+ 좌X- 의 순서로
			// 평면의 방향은 큐브의 바깥쪽으로

			const auto frontCenter = centerPos_ + halfZVec;
			cachedQuads_[0] = std::make_unique<SQuadBound>(LB(frontCenter, halfXVec, halfYVec), RB(frontCenter, halfXVec, halfYVec), RT(frontCenter, halfXVec, halfYVec), LT(frontCenter, halfXVec, halfYVec));	// 앞Z+

			const auto backCenter = centerPos_ - halfZVec;	// 역방향 지정에 주의, 뒷면이므로 반대를 보고 있어야하니깐
			cachedQuads_[1] = std::make_unique<SQuadBound>(LT(backCenter, halfXVec, halfYVec), RT(backCenter, halfXVec, halfYVec), RB(backCenter, halfXVec, halfYVec), LB(backCenter, halfXVec, halfYVec));	// 뒤Z-

			const auto upCenter = centerPos_ + halfYVec;
			cachedQuads_[2] = std::make_unique<SQuadBound>(LB(upCenter, halfZVec, halfXVec), RB(upCenter, halfZVec, halfXVec), RT(upCenter, halfZVec, halfXVec), LT(upCenter, halfZVec, halfXVec));	// 위Y+

			const auto bottomCenter = centerPos_ - halfYVec;
			cachedQuads_[3] = std::make_unique<SQuadBound>(LT(bottomCenter, halfZVec, halfXVec), RT(bottomCenter, halfZVec, halfXVec), RB(bottomCenter, halfZVec, halfXVec), LB(bottomCenter, halfZVec, halfXVec));	// 아래Y-

			const auto rightCenter = centerPos_ + halfXVec;
			cachedQuads_[4] = std::make_unique<SQuadBound>(LB(rightCenter, halfYVec, halfZVec), RB(rightCenter, halfYVec, halfZVec), RT(rightCenter, halfYVec, halfZVec), LT(rightCenter, halfYVec, halfZVec));	// 우X+

			const auto leftCenter = centerPos_ - halfXVec;
			cachedQuads_[5] = std::make_unique<SQuadBound>(LT(leftCenter, halfYVec, halfZVec), RT(leftCenter, halfYVec, halfZVec), RB(leftCenter, halfYVec, halfZVec), LB(leftCenter, halfYVec, halfZVec));	// 우X-
		}

		outQuads[0] = *cachedQuads_[0].get();
		outQuads[1] = *cachedQuads_[1].get();
		outQuads[2] = *cachedQuads_[2].get();
		outQuads[3] = *cachedQuads_[3].get();
		outQuads[4] = *cachedQuads_[4].get();
		outQuads[5] = *cachedQuads_[5].get();
	}
};
