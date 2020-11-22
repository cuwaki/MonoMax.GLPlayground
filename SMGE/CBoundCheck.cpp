#include "CBoundCheck.h"
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

	bool SSegmentBound::check(const SPlaneBound& plane, glm::vec3& outCross) const
	{
		const auto toEnd = end_ - start_;
		const auto toEndLen = glm::length(toEnd);
		const auto toEndDir = toEnd / toEndLen;
		
		auto baseLineLen = plane.getDistanceOnPlane(start_);

		bool isMinus = std::signbit(baseLineLen);
		if (isMinus)	// 평면 밑에서 쏜 경우
			baseLineLen *= -1.f;
		else
			;	// 평면 위에서 쏜 경우

		const auto cosT = glm::dot(plane.normal_ * (isMinus ? 1.f : -1.f), toEndDir);
		if (isInRange(0.f, -1.f, cosT))	// 평행 또는 반대방향
			return false;

		const auto hypotenuseLen = baseLineLen * 1.f / cosT;	// 삼각비를 이용
		if (toEndLen < hypotenuseLen)	// 안닿았음
			return false;

		outCross = start_ + toEndDir * hypotenuseLen;
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
};
