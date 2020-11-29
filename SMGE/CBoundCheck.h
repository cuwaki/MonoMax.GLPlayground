#pragma once

#include "../packages/glm.0.9.9.800/build/native/include/glm/glm.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/ext.hpp"
#include <memory>

namespace SMGE
{
	constexpr float BoundCheckEpsilon = 0.00001f;
	inline bool isNearlyEqual(const float& l, const float& r, const float& epsilon = BoundCheckEpsilon)
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

		glm::vec3 getNormal() const { return normal_; }
		float getD() const { return d_; }

	protected:
		glm::vec3 normal_;
		float d_;
	};

	struct STriangleBound : public SPlaneBound
	{
		STriangleBound();
		STriangleBound(const glm::vec3& ccw_p0, const glm::vec3& ccw_p1, const glm::vec3& ccw_p2);

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

	protected:
		glm::vec3 p0_, p1_, p2_;
		mutable std::unique_ptr<SSegmentBound[]> cachedSegments_;
	};

	struct SQuadBound : public SPlaneBound
	{
		SQuadBound();
		SQuadBound(const glm::vec3& ccw_p0, const glm::vec3& ccw_p1, const glm::vec3& ccw_p2, const glm::vec3& ccw_p3);

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

		glm::vec3 loc_;
		float radius_;
	};

	struct SCubeBound : public SBound
	{
		SCubeBound() : SBound(EBoundType::CUBE) {}
		SCubeBound(const glm::vec3& centerPos, const glm::vec3& size, const glm::vec3& eulerDegreesXYZ);
		SCubeBound(const glm::vec3& centerPos, const glm::vec3& size, const glm::vec3 (&eulerAxis)[3]);

		bool operator==(const SCubeBound& other) const;
		bool check(const SPointBound& point) const;
		bool check(const SCubeBound& cube) const;

		void getQuads(SQuadBound (&outQuads)[6]) const;

		glm::vec3 getEulerAxis(int axis) const { return eulerAxis_[axis]; }
		float getSize(int axis) const { return axis == 0 ? size_.x : (axis == 1 ? size_.y : size_.z); }
		glm::vec3 getCenterPos() const { return centerPos_; }

	protected:
		// 캐시가 있으므로 생성된 후 절대로 외부에서 수정이 되면 안된다!
		glm::vec3 centerPos_;
		glm::vec3 size_;
		glm::vec3 eulerAxis_[3];	// x, y, z축

		mutable std::unique_ptr<SQuadBound[]> cachedQuads_;
	};

	struct SAABB : public SBound
	{
		SAABB() : SBound(EBoundType::AABB) {}

		glm::vec3 lb_, rt_;

		bool isIntersect(const SAABB& other) const;
		inline bool isContains(const glm::vec3& point) const;

		inline const glm::vec3& lb() const;
		inline const glm::vec3& rt() const;
		inline glm::vec3 lt() const;
		inline glm::vec3 rb() const;

	protected:
		bool isXContains(const SAABB& other) const;
		bool isYContains(const SAABB& other) const;
		bool isZContains(const SAABB& other) const;
		bool isIntersectPoints(const SAABB& other) const;
	};
}
