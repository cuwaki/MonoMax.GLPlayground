#include "CBoundCheck.h"
#include "../MonoMax.EngineCore/RenderingEngine.h"
#include <cmath>

namespace SMGE
{
	const bool BoundCheckMatrix[int32(EBoundType::MAX)][int32(EBoundType::MAX)] =
	{
		// POINT	SEGMENT	PLANE	TRI		QUAD	CIRC	SPHE	CUBE	AABB
		{ true,		false,	false,	false,	false,	false,	false,	false,	false	},	// POINT
		{ true,		true,	true,	true,	true,	true,	true,	true,	false	},	// SEGMENT

		{ true,		false,	false,	false,	false,	false,	false,	false,	false	},	// PLANE
		{ true,		false,	false,	false,	false,	false,	false,  false,	false	},	// TRIANGLE
		{ true,		false,	false,	false,	false,	false,	false,  false,	false	},	// QUAD
		{ true,		false,	false,	false,	false,	false,	false,  false,	false	},	// CIRCLE

		{ true,		false,	true,	true,	true,	true,	true,	true,	false  },	// SPHERE
		{ true,		false,	false,	false,	false,	false,	false,	true,	false  },	// CUBE

		{ true,		false,	false,	false,	false,	false,	false,	false,	true	},	// AABB
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SBound::operator SAABB() const
	{
		return SAABB();
	}

	bool SBound::check(const SBound& other, SSegmentBound& outCrossSeg) const
	{
		const int32 thisBT = int32(type_), otherBT = int32(other.type_);

		if (BoundCheckMatrix[thisBT][otherBT])
			return check(other.type_, other, outCrossSeg);
		else if (BoundCheckMatrix[otherBT][thisBT])
			return other.check(this->type_, *this, outCrossSeg);
		else
		{
			assert(false && "need to imple this case!");
			return false;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool SPointBound::operator==(const SPointBound& other) const
	{
		return loc_ == other.loc_;
	}

	SPointBound::operator SAABB() const
	{
		const glm::vec3 epsilonVec3(Configs::BoundEpsilon);
		return { loc_ - epsilonVec3, loc_ + epsilonVec3 };
	}

	bool SPointBound::check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const
	{
		switch (otherType)
		{
		case EBoundType::POINT:
			return check(static_cast<const SPointBound&>(other), outCrossSeg);
		default:
			return false;
		}
	}

	bool SPointBound::check(const struct SPointBound& point, SSegmentBound& outCrossSeg) const
	{
		outCrossSeg = SSegmentBound(point);
		return *this == point;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SSegmentBound::reverse()
	{
		std::swap(start_, end_);
	}

	float SSegmentBound::getDistanceFromSegment(const glm::vec3& loc) const
	{
		const auto dir = glm::normalize(end_ - start_);
		const auto toLoc = loc - start_;
		return std::fabsf(glm::dot(dir, toLoc));
	}

	SSegmentBound::operator SAABB() const
	{
		const glm::vec3 epsilonVec3(Configs::BoundEpsilon);

		glm::vec3 min(start_), max(start_);
		findMinAndMaxVector({ start_ - epsilonVec3, start_ + epsilonVec3, end_ - epsilonVec3, end_ + epsilonVec3 }, min, max);

		return { min, max };
	}

	bool SSegmentBound::operator==(const SSegmentBound& other) const
	{
		return start_ == other.start_ && end_ == other.end_;
	}

	float SSegmentBound::getSlope2D_XY() const
	{
		auto to = end_ - start_;
		if (isNearlyEqual(to.x, 0.f))
			return NAN;

		return to.y / to.x;
	}

	bool SSegmentBound::check2D_XY(const SSegmentBound& other, SSegmentBound& outCross) const
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
			outCross = SSegmentBound(glm::vec3(crossX, crossY, 0.f));
			return true;
		}

		return false;
	}

	bool SSegmentBound::check2D_XY(const SPointBound& point, SSegmentBound& outCross) const
	{
		const auto a = this->getSlope2D_XY();
		if (std::isnan(a))
		{
			if (isNearlyEqual(start_.x, point.loc_.x) && isInRange(start_.y, end_.y, point.loc_.y))
			{
				outCross = SSegmentBound(glm::vec3(point.loc_.x, point.loc_.y, 0.f));
				return true;
			}
		}
		else
		{
			if (isInRange(start_.x, end_.x, point.loc_.x))
			{
				const auto y = a * (point.loc_.x - start_.x) + start_.y;

				bool ret = isNearlyEqual(y, point.loc_.y);
				if (ret)
					outCross = SSegmentBound(glm::vec3(point.loc_.x, point.loc_.y, 0.f));

				return ret;
			}
		}

		return false;
	}

	bool SSegmentBound::check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const
	{
		switch (otherType)
		{
		case EBoundType::POINT:		return check2D_XY(static_cast<const SPointBound&>(other), outCrossSeg);
		case EBoundType::SEGMENT:	return check2D_XY(static_cast<const SSegmentBound&>(other), outCrossSeg);
		case EBoundType::PLANE:		return check(static_cast<const SPlaneBound&>(other), outCrossSeg);
		case EBoundType::TRIANGLE:	return check(static_cast<const STriangleBound&>(other), outCrossSeg);
		case EBoundType::QUAD:		return check(static_cast<const SQuadBound&>(other), outCrossSeg);
		case EBoundType::CIRCLE:	return check(static_cast<const SCircleBound&>(other), outCrossSeg);
		case EBoundType::SPHERE:	return check(static_cast<const SSphereBound&>(other), outCrossSeg);
		case EBoundType::CUBE:		return check(static_cast<const SCubeBound&>(other), outCrossSeg);
		default:
			return false;
		}
	}

	bool SSegmentBound::check(const SPlaneBound& plane, SSegmentBound& outCross) const
	{
		const auto toEnd = end_ - start_;
		const auto toEndLen = glm::length(toEnd);
		const auto toEndDir = toEnd / toEndLen;
		
		auto baseLineLen = plane.getSignedDistanceFromPlane(start_);
		if (isNearlyEqual(baseLineLen, 0.f))
			return false;

		bool isUnderOfPlane = std::signbit(baseLineLen);
		if (isUnderOfPlane)	// 평면 밑에서 쏜 경우
			baseLineLen *= -1.f;
		else
			;	// 평면 위에서 쏜 경우

		const auto cosT = glm::dot(plane.getNormal() * (isUnderOfPlane ? 1.f : -1.f), toEndDir);
		if (isInRange(-1.f, 0.f + Configs::BoundCheckEpsilon, cosT))	// 평행 또는 반대방향
			return false;

		const auto hypotenuseLen = baseLineLen * 1.f / cosT;	// 삼각비를 이용
		if (toEndLen < hypotenuseLen)	// 안닿았음
			return false;

		outCross = SSegmentBound(start_, start_ + toEndDir * hypotenuseLen);
		return true;
	}

	bool SSegmentBound::check(const struct STriangleBound& tri, SSegmentBound& outCross, bool isCheckWithPlane) const
	{
		if (isCheckWithPlane == true && check(static_cast<const SPlaneBound&>(tri), outCross) == false)
			return false;

		const auto d = tri.getP1() - tri.getP0();
		const auto e = tri.getP2() - tri.getP0();

		const auto g = outCross.end_ - tri.getP0();
		
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

	bool SSegmentBound::check(const struct SQuadBound& quad, SSegmentBound& outCross, bool isCheckWithPlane) const
	{
		if (isCheckWithPlane == true && check(static_cast<const SPlaneBound&>(quad), outCross) == false)
			return false;

		const auto d = quad.getP1() - quad.getP0();
		const auto e = quad.getP3() - quad.getP0();

		const auto g = outCross.end_ - quad.getP0();

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

	bool SSegmentBound::check(const struct SCircleBound& circle, SSegmentBound& outCross, bool isCheckWithPlane) const
	{
		if (isCheckWithPlane == true && check(static_cast<const SPlaneBound&>(circle), outCross) == false)
			return false;

		const auto rr = circle.getRadius() * circle.getRadius();
		const auto distSQ = getDistanceSquared(outCross.end_, circle.getCenterPos());

		return distSQ < rr;
	}

	bool SSegmentBound::check(const SSphereBound& sphere, SSegmentBound& outCross) const
	{
		const auto radiusSQ = sphere.getRadius() * sphere.getRadius();

		auto distanceSQ = getDistanceSquared(start_, sphere.getCenterPos());
		if (distanceSQ < radiusSQ)
		{	// 시점이 구 안에 있는 경우
			distanceSQ = getDistanceSquared(end_, sphere.getCenterPos());
			if (distanceSQ < radiusSQ)
			{	// 종점도 구 안에 있는 경우
				return false;
			}
			else
			{	// 종점은 구 밖이다
				SSegmentBound reversedSeg(*this);
				reversedSeg.reverse();	// 시점과 종점을 바꿔서 체크한다
				return reversedSeg.check(sphere, outCross);
			}
		}
		else
		{
			// 아래 코드는 활성화 해도 되고 안해도 되는데 일단 막아두었다, glm::normalize 할 때 sqrtf 를 하기 때문...
			//const auto distanceToSphereCenter = getDistanceFromSegment(sphere.getCenterPos());
			//if (distanceToSphereCenter >= sphere.getRadius())	// >= 에 주의! 현재 딱 경계라면 비충돌로 판정하는 것이 기준이다
			//	return false;

			const auto toEnd = end_ - start_;
			const auto xa = start_.x - sphere.getCenterPos().x, ya = start_.y - sphere.getCenterPos().y, za = start_.z - sphere.getCenterPos().z;

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

			outCross = SSegmentBound(start_, start_ + toEnd * minT);
			return true;
		}
	}

	bool SSegmentBound::check(const SCubeBound& cube, SSegmentBound& outCross) const
	{
		if(check(static_cast<const SSphereBound&>(cube), outCross) == false)
			return false;

		SQuadBound quads[6];
		cube.getQuads(quads);

		const auto segDirInverse = glm::normalize(start_ - end_);	// 내적을 위한 inverse

		for (const auto& quad : quads)
		{
			const auto cosT = glm::dot(segDirInverse, quad.getNormal());
			const auto isInFront = isInRange(0.f + Configs::BoundCheckEpsilon, 1.f + Configs::BoundCheckEpsilon, cosT);

			if (isInFront && check(quad, outCross) == true)
			{	// 선분의 방향의 역이 quad 의 앞쪽이 아니라면 체크할 필요가 없다, cube 는 입체이고 입체라면 앞에서 오는 충돌이 뒷면에 닿을 일이 없기 때문이다
				return true;
			}
		}

		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SPlaneBound::SPlaneBound(const glm::vec3& norm, const glm::vec3& loc) : SPlaneBound()
	{
		normal_ = glm::normalize(norm);
		d_ = -normal_.x * loc.x + -normal_.y * loc.y + -normal_.z * loc.z;
	}

	float SPlaneBound::getSignedDistanceFromPlane(const glm::vec3& loc) const
	{
		const auto distance = normal_.x * loc.x + normal_.y * loc.y + normal_.z * loc.z + d_;
		return distance;
	}

	bool SPlaneBound::isInFront(const glm::vec3& loc) const
	{
		return getSignedDistanceFromPlane(loc) > 0.f;
	}
	bool SPlaneBound::isOnPlane(const glm::vec3& loc) const
	{
		return isNearlyEqual(getSignedDistanceFromPlane(loc), 0.f);
	}
	bool SPlaneBound::isInBack(const glm::vec3& loc) const
	{
		return getSignedDistanceFromPlane(loc) < 0.f;
	}

	bool SPlaneBound::check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const
	{
		switch (otherType)
		{
		case EBoundType::POINT:	return check(static_cast<const SPointBound&>(other), outCrossSeg);
		default:
			return false;
		}
	}

	bool SPlaneBound::check(const SPointBound& point, SSegmentBound& outCross) const
	{
		bool ret = isOnPlane(point.loc_);
		if (ret)
			outCross = point.loc_;
		return ret;
	}

	bool SPlaneBound::check(const struct SPlaneBound& plane, SSegmentBound& outCross) const
	{
		assert(false && "need to imple");
		return false;
	}

	bool SPlaneBound::operator==(const SPlaneBound& other) const
	{
		return normal_ == other.normal_ && isNearlyEqual(d_, other.d_);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	STriangleBound::STriangleBound() : SPlaneBound({ 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f })
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

	STriangleBound::operator SAABB() const
	{
		glm::vec3 min(p0_), max(p0_);
		findMinAndMaxVector({ p0_, p1_, p2_ }, min, max);
		return { min, max };
	}

	bool STriangleBound::operator==(const STriangleBound& other) const
	{
		return p0_ == other.getP0() && p1_ == other.getP1() && p2_ == other.getP2();
	}

	void STriangleBound::getSegments(SSegmentBound(&outSegs)[3]) const
	{
		if (cachedSegments_ == false)
		{
			cachedSegments_ = std::make_unique<SSegmentBound[]>(3);

			cachedSegments_.get()[0] = SSegmentBound(p0_, p0_ + (p1_ - p0_));
			cachedSegments_.get()[1] = SSegmentBound(p1_, p1_ + (p2_ - p1_));
			cachedSegments_.get()[2] = SSegmentBound(p2_, p2_ + (p0_ - p2_));
		}

		outSegs[0] = cachedSegments_.get()[0];
		outSegs[1] = cachedSegments_.get()[1];
		outSegs[2] = cachedSegments_.get()[2];
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SQuadBound::SQuadBound() : SPlaneBound({ 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f })
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

	SQuadBound::operator SAABB() const
	{
		glm::vec3 min(p0_), max(p0_);
		findMinAndMaxVector({ p0_, p1_, p2_, p3_ }, min, max);
		return { min, max };
	}

	bool SQuadBound::operator==(const SQuadBound& other) const
	{
		return p0_ == other.getP0() && p1_ == other.getP1() && p2_ == other.getP2() && p3_ == other.getP3();
	}

	void SQuadBound::getSegments(SSegmentBound(&outSegs)[4]) const
	{
		if (cachedSegments_ == false)
		{
			cachedSegments_ = std::make_unique<SSegmentBound[]>(4);

			cachedSegments_.get()[0] = SSegmentBound(p0_, p0_ + (p1_ - p0_));
			cachedSegments_.get()[1] = SSegmentBound(p1_, p1_ + (p2_ - p1_));
			cachedSegments_.get()[2] = SSegmentBound(p2_, p2_ + (p3_ - p2_));
			cachedSegments_.get()[3] = SSegmentBound(p3_, p3_ + (p0_ - p3_));
		}

		outSegs[0] = cachedSegments_.get()[0];
		outSegs[1] = cachedSegments_.get()[1];
		outSegs[2] = cachedSegments_.get()[2];
		outSegs[3] = cachedSegments_.get()[3];
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SCircleBound::SCircleBound() : SPlaneBound({ 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f })
	{
		type_ = EBoundType::CIRCLE;

		cachePerp();
	}

	SCircleBound::SCircleBound(const glm::vec3& norm, const glm::vec3& center, float radius) : SPlaneBound(norm, center)
	{
		type_ = EBoundType::CIRCLE;
		loc_ = center;
		radius_ = radius;

		cachePerp();
	}

	void SCircleBound::cachePerp() const
	{
		glm::vec3 axis(0.f, 1.f, 0.f);
		
		cachedPerp_ = glm::cross(axis, normal_);
		if (isNearlyEqual(glm::length(cachedPerp_), 0.f))
		{
			axis = { 1.f, 0.f, 0.f };

			cachedPerp_ = glm::cross(axis, normal_);
			if (isNearlyEqual(glm::length(cachedPerp_), 0.f))
			{
				axis = { 0.f, 0.f, 1.f };

				cachedPerp_ = glm::cross(axis, normal_);
				// 여기까지 왔으면 무조건 성공해야한다
			}
		}

		cachedPerp_ = glm::normalize(cachedPerp_);
	}

	SCircleBound::operator SAABB() const
	{
		const glm::vec3 front = normal_ * Configs::BoundEpsilon, 
			perpU = cachedPerp_ * radius_,
			perpV = glm::normalize(glm::cross(front, perpU)) * radius_;

		// 위에서 front = z, perpU = x, perpV = y 라고 치고, 3차원 오른손 좌표계 축을 만든것이다

		glm::vec3 min = (loc_ + front * -1.f + perpU * -1.f + perpV * -1.f), 
				max = (loc_ + front * +1.f + perpU * +1.f + perpV * +1.f);
		findMinAndMaxVector({ min, max }, min, max);
		return { min, max };
	}

	bool SCircleBound::operator==(const SCircleBound& other) const
	{
		return normal_ == other.normal_ && loc_ == other.loc_ && radius_ == other.radius_;
	}

	void SCircleBound::getSegments(SSegmentBound(&outSegs)[CIRCUMFERENCE_SEGMENT_MAX]) const
	{
		// 미구현 - 필요시 구현하라
		assert(false && "need implements");

		if (cachedSegments_ == false)
		{
			cachedSegments_ = std::make_unique<SSegmentBound[]>(CIRCUMFERENCE_SEGMENT_MAX);

			//cachedSegments_.get()[0] = SSegmentBound(p0_, p0_ + (p1_ - p0_));
		}

		//outSegs[0] = cachedSegments_.get()[0];
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool SSphereBound::operator==(const SSphereBound& other) const
	{
		return loc_ == other.loc_ && isNearlyEqual(radius_, other.getRadius());
	}

	SSphereBound::operator SAABB() const
	{
		auto radiusVec3 = glm::vec3(radius_, radius_, radius_);
		return { loc_ - radiusVec3, loc_ + radiusVec3 };
	}

	bool SSphereBound::check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const
	{
		switch (otherType)
		{
		case EBoundType::POINT:		return check(static_cast<const SPointBound&>(other), outCrossSeg);
		case EBoundType::PLANE:		return check(static_cast<const SPlaneBound&>(other), outCrossSeg);
		case EBoundType::TRIANGLE:	return check(static_cast<const STriangleBound&>(other), outCrossSeg);
		case EBoundType::QUAD:		return check(static_cast<const SQuadBound&>(other), outCrossSeg);
		case EBoundType::SPHERE:	return check(static_cast<const SSphereBound&>(other), outCrossSeg);
		case EBoundType::CUBE:		return check(static_cast<const SCubeBound&>(other), outCrossSeg);
		default:
			return false;
		}
	}

	bool SSphereBound::check(const SPointBound& point, SSegmentBound& outCrossSegment) const
	{
		const auto distSQ = getDistanceSquared(loc_, point.loc_);
		bool ret = distSQ < (radius_* radius_);
		if (ret)
			outCrossSegment = point.loc_;
		return ret;
	}

	bool SSphereBound::check(const SPlaneBound& plane, SSegmentBound& outCrossSegment) const
	{
		const auto dist = plane.getSignedDistanceFromPlane(loc_);
		const auto absDist = std::fabsf(dist);
		if (absDist < radius_)
		{
			bool isUnderOfPlane = std::signbit(dist);

			const auto gap = radius_ - absDist;
			const auto end = loc_ + (plane.getNormal() * (isUnderOfPlane ? 1.f : -1.f)) * (radius_ - gap);
			outCrossSegment = SSegmentBound(loc_, end);
			return true;
		}

		return false;
	}

	bool SSphereBound::check(const STriangleBound& tri, SSegmentBound& outCrossSegment) const
	{
		// 1. 평면과의 체크
		bool isCross = check(static_cast<const SPlaneBound&>(tri), outCrossSegment);
		if (isCross)
		{
			SSegmentBound segs[3];
			tri.getSegments(segs);

			// 차후 최적화 가능성 - 구를 평면도형에 투영한 넓이와 평면도형의 넓이를 비교하여 평면도형이 더 넓다면 선분의 교차보다 먼저 면과의 교차를 검사하는 것이 빠를 것 같다

			SSegmentBound temp;
			for (auto& seg : segs)
			{	// 차후 최적화 가능성 - 평면도형의 중심이 구의 밖이라면, 내적을 이용하여 둔각인 것들의 경우 체크 안해도 될 것 같다 - 확실한지는 테스트 해봐야할 듯
				// 2. 구가 평면도형의 선분과 교차하였는가?
				if (seg.check(*this, temp) == true)
				{
					outCrossSegment = temp;
					return true;
				}
			}

			// 3. 구가 평면도형의 면에 교차하였는가?
			bool ret = outCrossSegment.check(tri, temp, false);	// 이미 평면과는 체크되었으므로 false
			if (ret)
				outCrossSegment = temp;
			return ret;
		}

		return false;
	}

	bool SSphereBound::check(const SQuadBound& quad, SSegmentBound& outCrossSegment) const
	{
		// 1. 평면과의 체크
		bool isCross = check(static_cast<const SPlaneBound&>(quad), outCrossSegment);
		if (isCross)
		{
			SSegmentBound segs[4];
			quad.getSegments(segs);

			// 차후 최적화 가능성 - 구를 평면도형에 투영한 넓이와 평면도형의 넓이를 비교하여 평면도형이 더 넓다면 선분의 교차보다 먼저 면과의 교차를 검사하는 것이 빠를 것 같다

			SSegmentBound temp;
			for (auto& seg : segs)
			{	// 차후 최적화 가능성 - 평면도형의 중심이 구의 밖이라면, 내적을 이용하여 둔각인 것들의 경우 체크 안해도 될 것 같다 - 확실한지는 테스트 해봐야할 듯
				// 2. 구가 평면도형의 선분과 교차하였는가?
				if (seg.check(*this, temp) == true)
				{
					outCrossSegment = temp;
					return true;
				}
			}

			// 3. 구가 평면도형의 면에 교차하였는가?
			bool ret = outCrossSegment.check(quad, temp, false);	// 이미 평면과는 체크되었으므로 false
			if (ret)
				outCrossSegment = temp;
			return ret;
		}

		return false;
	}

	bool SSphereBound::check(const SSphereBound& otherSphere, SSegmentBound& outCrossSegment) const
	{
		const auto rr = radius_ + otherSphere.getRadius();

		const auto distSQ = getDistanceSquared(loc_, otherSphere.getCenterPos());
		if (distSQ < (rr * rr))
		{
			const auto dist = std::sqrtf(distSQ);
			const auto gap = rr - dist;

			const auto toDir = glm::normalize(loc_ - otherSphere.getCenterPos());
			const auto end = otherSphere.getCenterPos() + toDir * (otherSphere.getRadius() - gap);

			outCrossSegment = SSegmentBound(otherSphere.getCenterPos(), end);
			return true;
		}

		return false;
	}

	bool SSphereBound::check(const SCubeBound& cube, SSegmentBound& outCrossSegment) const
	{
		if (check(static_cast<const SSphereBound&>(cube), outCrossSegment) == false)
			return false;

		SQuadBound quads[6];
		cube.getQuads(quads);

		const auto cubeToSphereDir = glm::normalize(loc_ - cube.getCenterPos());

		for (const auto& quad : quads)
		{
			const auto cosT = glm::dot(cubeToSphereDir, quad.getNormal());
			const auto isSameDir = isInRange(0.f + Configs::BoundCheckEpsilon, 1.f + Configs::BoundCheckEpsilon, cosT);

			if (isSameDir && check(quad, outCrossSegment))
			{	// 큐브는 입체이므로 방향이 다른 면들은 충돌체크 할 필요가 없다
				return true;
			}
		}

		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SCubeBound::SCubeBound() : SSphereBound()
	{
		type_ = EBoundType::CUBE;
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

		type_ = EBoundType::CUBE;
	}

	SCubeBound::SCubeBound(const glm::vec3& centerPos, const glm::vec3& size, const glm::vec3(&eulerAxis)[3]) : SCubeBound()
	{
		using namespace nsRE::TransformConst;

		size_ = size;
		eulerAxis_[ETypeAxis::X] = eulerAxis[ETypeAxis::X];
		eulerAxis_[ETypeAxis::Y] = eulerAxis[ETypeAxis::Y];
		eulerAxis_[ETypeAxis::Z] = eulerAxis[ETypeAxis::Z];

		const auto xHalfLeng = size_.x / 2.f;
		const auto yHalfLeng = size_.y / 2.f;
		const auto zHalfLeng = size_.z / 2.f;
		
		const auto farthestDistanceFromCenter = std::sqrtf(xHalfLeng * xHalfLeng + yHalfLeng * yHalfLeng + zHalfLeng * zHalfLeng);
		this->SSphereBound::SSphereBound(centerPos, farthestDistanceFromCenter);

		type_ = EBoundType::CUBE;
	}

	SCubeBound::operator SAABB() const
	{
		SQuadBound outQuads[6];
		getQuads(outQuads, true);

		glm::vec3 min(outQuads[0].getP0()), max(outQuads[0].getP0());
		findMinAndMaxVector(
			{
				outQuads[0].getP0(), outQuads[0].getP1(), outQuads[0].getP2(), outQuads[0].getP3(),
				outQuads[1].getP0(), outQuads[1].getP1(), outQuads[1].getP2(), outQuads[1].getP3()
			},
			min, max);
		return { min, max };
	}

	bool SCubeBound::operator==(const SCubeBound& other) const
	{
		return loc_ == other.loc_ && size_ == other.size_ &&
			eulerAxis_[0] == other.eulerAxis_[0] &&
			eulerAxis_[1] == other.eulerAxis_[1] &&
			eulerAxis_[2] == other.eulerAxis_[2];
	}

	void SCubeBound::getQuads(SQuadBound(&outQuads)[6], bool isJustWantFrontAndBack) const
	{
		using namespace nsRE::TransformConst;

		if (cachedQuads_ == false)
		{
			cachedQuads_ = std::make_unique<SQuadBound[]>(6);

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
			// 평면의 방향은 큐브의 바깥쪽으로 <- 이건 절대로 바뀌면 안됨

			const auto frontCenter = loc_ + halfZVec;
			cachedQuads_.get()[0] = SQuadBound(LB(frontCenter, halfXVec, halfYVec), RB(frontCenter, halfXVec, halfYVec), RT(frontCenter, halfXVec, halfYVec), LT(frontCenter, halfXVec, halfYVec));	// 앞Z+

			const auto backCenter = loc_ - halfZVec;	// 역방향 지정에 주의, 현재 상황에서 뒷면이므로 노멀이 반대를 보고 있어야하니깐
			cachedQuads_.get()[1] = SQuadBound(LT(backCenter, halfXVec, halfYVec), RT(backCenter, halfXVec, halfYVec), RB(backCenter, halfXVec, halfYVec), LB(backCenter, halfXVec, halfYVec));	// 뒤Z-

			if (isJustWantFrontAndBack)
			{
				outQuads[0] = cachedQuads_.get()[0];
				outQuads[1] = cachedQuads_.get()[1];
				return;
			}

			const auto upCenter = loc_ + halfYVec;
			cachedQuads_.get()[2] = SQuadBound(LB(upCenter, halfZVec, halfXVec), RB(upCenter, halfZVec, halfXVec), RT(upCenter, halfZVec, halfXVec), LT(upCenter, halfZVec, halfXVec));	// 위Y+

			const auto bottomCenter = loc_ - halfYVec;
			cachedQuads_.get()[3] = SQuadBound(LT(bottomCenter, halfZVec, halfXVec), RT(bottomCenter, halfZVec, halfXVec), RB(bottomCenter, halfZVec, halfXVec), LB(bottomCenter, halfZVec, halfXVec));	// 아래Y-

			const auto rightCenter = loc_ + halfXVec;
			cachedQuads_.get()[4] = SQuadBound(LB(rightCenter, halfYVec, halfZVec), RB(rightCenter, halfYVec, halfZVec), RT(rightCenter, halfYVec, halfZVec), LT(rightCenter, halfYVec, halfZVec));	// 우X+

			const auto leftCenter = loc_ - halfXVec;
			cachedQuads_.get()[5] = SQuadBound(LT(leftCenter, halfYVec, halfZVec), RT(leftCenter, halfYVec, halfZVec), RB(leftCenter, halfYVec, halfZVec), LB(leftCenter, halfYVec, halfZVec));	// 우X-
		}

		outQuads[0] = cachedQuads_.get()[0];
		outQuads[1] = cachedQuads_.get()[1];

		if (isJustWantFrontAndBack)
			return;

		outQuads[2] = cachedQuads_.get()[2];
		outQuads[3] = cachedQuads_.get()[3];
		outQuads[4] = cachedQuads_.get()[4];
		outQuads[5] = cachedQuads_.get()[5];
	}

	bool SCubeBound::check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const
	{
		switch (otherType)
		{
		case EBoundType::POINT:		return check(static_cast<const SPointBound&>(other), outCrossSeg);
		case EBoundType::CUBE:		return check(static_cast<const SCubeBound&>(other), outCrossSeg);
		default:
			return false;
		}
	}

	bool SCubeBound::check(const SPointBound& point, SSegmentBound& outCrossSegment) const
	{
		if(this->SSphereBound::SSphereBound::check(point, outCrossSegment) == false)
			return false;

		SQuadBound quads[6];
		getQuads(quads);

		for (const auto& quad : quads)
		{
			if (quad.isInBack(point.loc_) == false)
				return false;
		}

		outCrossSegment = point.loc_;
		return true;
	}

	bool SCubeBound::check(const SCubeBound& otherCube, SSegmentBound& outCrossSegment) const
	{
		if (this->SSphereBound::SSphereBound::check(static_cast<const SSphereBound&>(otherCube), outCrossSegment) == false)
			return false;

		SQuadBound otherQuads[6], thisQuads[6];
		otherCube.getQuads(otherQuads);
		this->getQuads(thisQuads);

		const auto otherToThisDir = glm::normalize(getCenterPos() - otherCube.getCenterPos());

		for (const auto& otherQuad : otherQuads)
		{
			const auto otherCosT = glm::dot(otherToThisDir, otherQuad.getNormal());
			const auto isSameDir = isInRange(0.f + Configs::BoundCheckEpsilon, 1.f + Configs::BoundCheckEpsilon, otherCosT);

			if (isSameDir)
			{	// 큐브는 입체이므로 방향이 다른 면들은 충돌체크 할 필요가 없다
				for (const auto& thisQuad : thisQuads)
				{
					const auto thisCosT = glm::dot(otherToThisDir, thisQuad.getNormal());
					const auto isOppositeDir = isInRange(-1.f, 0.f, thisCosT);

					if (isOppositeDir)
					{	// 역시 큐브는 입체...
						SSegmentBound otherSegs[4];
						otherQuad.getSegments(otherSegs);

						for (const auto& seg : otherSegs)
						{
							if (seg.check(thisQuad, outCrossSegment))
							{
								return true;
							}
						}
					}
				}
			}
		}

		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool SAABB::check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const
	{
		switch (otherType)
		{
		case EBoundType::POINT:		return check(static_cast<const SPointBound&>(other), outCrossSeg);
		case EBoundType::AABB:		return check(static_cast<const SAABB&>(other), outCrossSeg);
		default:
			return false;
		}
	}

	bool SAABB::check(const SPointBound& point, SSegmentBound& outCross) const
	{
		bool ret = point.loc_.x >= min_.x && point.loc_.x < max_.x&&
					point.loc_.y >= min_.y && point.loc_.y < max_.y&&
					point.loc_.z >= min_.z && point.loc_.z < max_.z;
		if (ret)
			outCross = point.loc_;

		return ret;
	}

	bool SAABB::check(const SAABB& other, SSegmentBound& outCross) const
	{
		if (isIntersectPoints(other))	// 점이 포함된 경우 또는 this가 other 를 완전히 감싼 경우
		{
			outCross = other.center();
			return true;
		}

		if (other.isIntersectPoints(*this))	// other 가 this 를 완전히 감싼 경우
		{
			outCross = other.center();
			return true;
		}

		// 점이 포함이 아니고 선분이 포함인 경우의 체크 : 십자가 같은 경우
		bool xyCross = (isXContains(other) && other.isYContains(*this)) || other.isXContains(*this) && this->isYContains(other);
		if (xyCross == true)
		{	// xy 로는 크로스 관계임이 확실해짐

			// zx 로 크로스인지 체크
			bool zxCross = (isZContains(other) && other.isXContains(*this)) || other.isZContains(*this) && this->isXContains(other);
			if (zxCross)
				outCross = other.center();
			return zxCross;
		}

		return false;
	}

	inline bool SAABB::isXContains(const SAABB& other) const
	{
		const auto otherMIN = other.min(), otherMAX = other.max();
		return otherMIN.x >= min_.x && otherMIN.x < max_.x&& otherMAX.x >= min_.x && otherMAX.x < max_.x;
	}
	inline bool SAABB::isYContains(const SAABB& other) const
	{
		const auto otherMIN = other.min(), otherMAX = other.max();
		return otherMIN.y >= min_.y && otherMIN.y < max_.y&& otherMAX.y >= min_.y && otherMAX.y < max_.y;
	}
	inline bool SAABB::isZContains(const SAABB& other) const
	{
		const auto otherMIN = other.min(), otherMAX = other.max();
		return otherMIN.z >= min_.z && otherMIN.z < max_.z&& otherMAX.z >= min_.z && otherMAX.z < max_.z;
	}

	bool SAABB::isIntersectPoints(const SAABB& other) const
	{
		SSegmentBound cross;

		const auto points = other.points();
		for (auto& p : points)
		{
			if (check(p, cross))
				return true;
		}
		return false;
	}

	inline const glm::vec3& SAABB::min() const
	{
		return min_;
	}
	inline const glm::vec3& SAABB::max() const
	{
		return max_;
	}
	inline const glm::vec3& SAABB::center() const
	{
		return min() + getSize() * 0.5f;
	}

	inline glm::vec3 SAABB::getSize() const
	{
		return { max().x - min().x, max().y - min().y, max().z - min().z };
	}

	inline std::initializer_list<SPointBound> SAABB::points() const
	{
		const auto size = getSize();
		return
		{	// 반시계 방향으로 만들어서 리턴한다
			min(), { min().x, min().y, min().z + size.z }, { min().x + size.x, min().y, min().z + size.z }, { min().x + size.x, min().y, min().z },	// 아랫면의 네 귀퉁이
			max(), { max().x, max().y, max().z - size.z }, { max().x - size.x, max().y, max().z - size.z }, { max().x - size.x, max().y, max().z }	// 윗면의 네 귀퉁이
		};
	}

	SAABB::operator SCubeBound() const
	{
		const auto size = getSize();
		return { min() + size / 2.f, size, { 0.f, 0.f, 0.f } };
	}
};

/* 구형 컬라이드 체크 식
	if (checkTarget->GetBoundType() == EBoundType::SPHERE)
	{
		outCollidingPoint = nsRE::TransformConst::Vec3_Zero;

		CSphereComponent* sphere = DCast<CSphereComponent*>(checkTarget);

		this->RecalcMatrix();
		sphere->RecalcMatrix();

		auto rayLoc = this->GetWorldPosition(), sphereLoc = sphere->GetWorldPosition();

		auto ray2sphere = sphereLoc - rayLoc;
		float r2sLen = glm::distance(sphereLoc, rayLoc);

		// https://m.blog.naver.com/PostView.nhn?blogId=hermet&logNo=68084286&proxyReferer=https:%2F%2Fwww.google.com%2F
		// 내용을 기준으로 조금 변형 하였다

		const auto rayLength = this->getRayLength();
		const auto sphereRadius = sphere->GetRadius();
		const auto rayDirection = this->getRayDirection();

		// 사이즈가 택도 없을 경우를 먼저 걸러낸다
		float minSize = r2sLen - sphereRadius;
		if (rayLength < minSize)
			return false;

		if (r2sLen <= sphereRadius)	// 접하는 것 포함
		{	// 레이 원점과 원 중심의 거리가 원의 반지름보다 작다 - 레이가 원의 안에서 발사된 것임
			outCollidingPoint = rayLoc;
			return true;
		}

		//auto normR2S = glm::normalize(ray2sphere);

		float cosTheta = glm::dot(ray2sphere, rayDirection) / r2sLen;	// 이거랑
		//float cosTheta = glm::dot(normR2S, this->direction_);	// 이거랑
		if (cosTheta <= 0.f)	// 레이의 방향이 원쪽과 반대방향 또는 직각임
			return false;

		float hypo = r2sLen;
		//float base = glm::dot(this->direction_, ray2sphere);	// 이거를 똑같이 써도 cosTheta 논리가 동일하지 않을까? 그러면 dot 한번 줄일 수 있다.
		float base = cosTheta * r2sLen;

		float radiusSQ = sphereRadius * sphereRadius;
		float heightSQ = hypo * hypo - base * base;

		if (heightSQ <= radiusSQ)
		{	// 광선과 구의 거리가 구의 반지름보다 작거나 같으면 거리 검사
			float intersectBase = std::sqrtf(radiusSQ - heightSQ);
			float distToCollidePoint = (base - intersectBase);

			if (distToCollidePoint <= rayLength)
			{	//  충돌한 지점과의 거리가 크기보다 작으면 충돌
				outCollidingPoint = rayLoc + (rayDirection * distToCollidePoint);
				return true;
			}
		}
	}
	*/