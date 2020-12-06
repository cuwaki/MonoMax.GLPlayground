#pragma once

#include "../packages/glm.0.9.9.800/build/native/include/glm/glm.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/ext.hpp"
#include <memory>
#include "CGameBase.h"

namespace SMGE
{
	inline bool isNearlyEqual(const float& l, const float& r, const float& epsilon = Configs::BoundCheckEpsilon)
	{
		return std::fabsf(l - r) < epsilon;
	}
	inline bool isInRange(const float& rangeL, const float& rangeR, const float& value)
	{
		if (rangeL < rangeR)
			return value >= rangeL && value < rangeR;
		return value >= rangeR && value < rangeL;
	}
	inline float getDistanceSquared(const glm::vec3& l, const glm::vec3& r)
	{
		const auto x = (l.x - r.x), y = (l.y - r.y), z = (l.z - r.z);
		return x * x + y * y + z * z;
	}
	template<typename VecT>
	void findMinAndMaxVector(const std::initializer_list<VecT>& points, VecT& outMin, VecT& outMax)
	{
		std::for_each(points.begin(), points.end(), [&outMin, &outMax](const auto& point)
			{
				if (point.x < outMin.x)
					outMin.x = point.x;
				if (point.y < outMin.y)
					outMin.y = point.y;
				if (point.z < outMin.z)
					outMin.z = point.z;

				if (point.x > outMax.x)
					outMax.x = point.x;
				if (point.y > outMax.y)
					outMax.y = point.y;
				if (point.z > outMax.z)
					outMax.z = point.z;
			});
	}

	enum class EBoundType
	{
		POINT,
		SEGMENT,
		PLANE,
		TRIANGLE,
		QUAD,

		SPHERE,
		CUBE,

		AABB,
		OBB,

		INHERIT_START,

		MAX = 255,
	};

	struct SAABB;

	struct SBound
	{
		SBound(EBoundType t) : type_(t) {}

		EBoundType type_ = EBoundType::MAX;
	};

	struct SPointBound : public SBound
	{
		SPointBound() : SBound(EBoundType::POINT) {}
		SPointBound(const glm::vec3& p) : SPointBound()
		{
			loc_ = p;
		}

		glm::vec3 loc_;

		bool check(const struct SPointBound& point) const;
		bool operator==(const SPointBound& other) const;

		operator SAABB() const;
	};

	struct SSegmentBound : public SBound
	{
		SSegmentBound() : SBound(EBoundType::SEGMENT) {}
		SSegmentBound(const glm::vec3& s, const glm::vec3& e) : SSegmentBound()
		{
			start_ = s;
			end_ = e;
		}

		glm::vec3 start_, end_;

		void reverse();
		
		float getDistanceFromSegment(const glm::vec3& loc) const;

		float getSlope2D_XY() const;
		bool check2D_XY(const SSegmentBound& other, glm::vec3& outCross) const;
		bool check2D_XY(const SPointBound& point) const;
		
		//bool check3D(const struct SSegmentBound& ...
		bool operator==(const SSegmentBound& other) const;

		bool check(const struct SPlaneBound& plane, glm::vec3& outCross) const;
		bool check(const struct STriangleBound& tri, glm::vec3& outCross, bool isCheckWithPlane = true) const;
		bool check(const struct SQuadBound& quad, glm::vec3& outCross, bool isCheckWithPlane = true) const;

		bool check(const struct SSphereBound& sphere, glm::vec3& outCross) const;
		bool check(const struct SCubeBound& cube, glm::vec3& outCross) const;

		operator SAABB() const;
	};

	struct SPlaneBound : public SBound
	{
		SPlaneBound() : SBound(EBoundType::PLANE) {}
		SPlaneBound(const glm::vec3& norm, const glm::vec3& loc);

		float getSignedDistanceFromPlane(const glm::vec3& loc) const;

		bool isInFront(const glm::vec3& loc) const;
		bool isOnPlane(const glm::vec3& loc) const;
		bool isInBack(const glm::vec3& loc) const;
		
		bool check(const SPointBound& point) const;
		bool check(const struct SPlaneBound& plane, glm::vec3& ourCrossVector, glm::vec3& outCrossPoint) const;

		bool operator==(const SPlaneBound& other) const;

		const glm::vec3& getNormal() const { return normal_; }
		float getD() const { return d_; }

	protected:
		glm::vec3 normal_;
		float d_;
	};

	struct STriangleBound : public SPlaneBound
	{
		STriangleBound();
		STriangleBound(const glm::vec3& ccw_p0, const glm::vec3& ccw_p1, const glm::vec3& ccw_p2);
		STriangleBound(const STriangleBound& other) noexcept
		{
			*this = other;
		}

		bool operator==(const STriangleBound& other) const;
		STriangleBound& operator=(const STriangleBound& other) noexcept
		{
			this->SPlaneBound::SPlaneBound(other);

			p0_ = other.p0_;
			p1_ = other.p1_;
			p2_ = other.p2_;
			cachedSegments_.reset();
			return *this;
		}

		void getSegments(SSegmentBound(&outSegs)[3]) const;

		const glm::vec3& getP0() const { return p0_; }
		const glm::vec3& getP1() const { return p1_; }
		const glm::vec3& getP2() const { return p2_; }

		operator SAABB() const;

	protected:
		glm::vec3 p0_, p1_, p2_;
		mutable std::unique_ptr<SSegmentBound[]> cachedSegments_;
	};

	struct SQuadBound : public SPlaneBound
	{
		SQuadBound();
		SQuadBound(const glm::vec3& ccw_p0, const glm::vec3& ccw_p1, const glm::vec3& ccw_p2, const glm::vec3& ccw_p3);
		SQuadBound(const SQuadBound& other) noexcept
		{
			*this = other;
		}

		bool operator==(const SQuadBound& other) const;
		SQuadBound& operator=(const SQuadBound& other) noexcept
		{
			this->SPlaneBound::SPlaneBound(other);

			p0_ = other.p0_;
			p1_ = other.p1_;
			p2_ = other.p2_;
			p3_ = other.p3_;
			cachedSegments_.reset();
			return *this;
		}

		void getSegments(SSegmentBound(&outSegs)[4]) const;

		const glm::vec3& getP0() const { return p0_; }
		const glm::vec3& getP1() const { return p1_; }
		const glm::vec3& getP2() const { return p2_; }
		const glm::vec3& getP3() const { return p3_; }

		operator SAABB() const;

	protected:
		glm::vec3 p0_, p1_, p2_, p3_;
		mutable std::unique_ptr<SSegmentBound[]> cachedSegments_;
	};

	struct SSphereBound : public SBound
	{
		SSphereBound() : SBound(EBoundType::SPHERE) {}
		SSphereBound(const glm::vec3& pos, float r) : SSphereBound()
		{
			loc_ = pos;
			radius_ = r;
		}

		bool operator==(const SSphereBound& other) const;

		bool check(const SPointBound& point) const;
		bool check(const SPlaneBound& plane, SSegmentBound& outCrossSegment) const;
		bool check(const STriangleBound& tri, SSegmentBound& outCrossSegment) const;
		bool check(const SQuadBound& quad, SSegmentBound& outCrossSegment) const;
		bool check(const SSphereBound& sphere, SSegmentBound& outCrossSegment) const;
		bool check(const SCubeBound& cube, SSegmentBound& outCrossSegment) const;

		const glm::vec3& getCenterPos() const { return loc_; }
		float getRadius() const { return radius_; }

		operator SAABB() const;

	protected:
		glm::vec3 loc_;
		float radius_;
	};

	// 큐브의 바운드 연산은 매우 비효율적이기 때문에, 스피어를 기본으로 체크하도록 한다
	struct SCubeBound : public SSphereBound
	{
		SCubeBound();
		SCubeBound(const glm::vec3& centerPos, const glm::vec3& size, const glm::vec3& eulerDegreesXYZ);
		SCubeBound(const glm::vec3& centerPos, const glm::vec3& size, const glm::vec3 (&eulerAxis)[3]);

		bool operator==(const SCubeBound& other) const;
		bool check(const SPointBound& point) const;
		bool check(const SCubeBound& cube, SSegmentBound& outCrossSegment) const;

		void getQuads(SQuadBound (&outQuads)[6], bool isJustWantFrontAndBack = false) const;

		const glm::vec3& getEulerAxis(int axis) const { return eulerAxis_[axis]; }
		float getSize(int axis) const { return axis == 0 ? size_.x : (axis == 1 ? size_.y : size_.z); }

		float getFarthestDistance() const { return radius_; }

		operator SAABB() const;

	protected:
		// 캐시가 있고, SSphereBound 의 역할도 하므로, 이 값들은 생성된 후 절대로 외부에서 수정이 되면 안된다! 뭐.. 이건 다른 것도 마찬가지
		glm::vec3 size_;
		glm::vec3 eulerAxis_[3];	// x, y, z축

		mutable std::unique_ptr<SQuadBound[]> cachedQuads_;
	};

	struct SAABB : public SBound
	{
		SAABB() : SBound(EBoundType::AABB) {}
		SAABB(const glm::vec3& min, const glm::vec3& max) : SAABB()
		{
			min_ = min;
			max_ = max;
		}

		bool isIntersect(const SAABB& other) const;
		inline bool isContains(const glm::vec3& point) const;

		inline const glm::vec3& min() const;
		inline const glm::vec3& max() const;
		std::initializer_list<glm::vec3> points() const;

		inline glm::vec3 getSize() const;

		operator SCubeBound() const;

	protected:
		inline bool isXContains(const SAABB& other) const;
		inline bool isYContains(const SAABB& other) const;
		inline bool isZContains(const SAABB& other) const;
		bool isIntersectPoints(const SAABB& other) const;

		glm::vec3 min_, max_;
	};
}
