#pragma once

#include "../packages/glm.0.9.9.800/build/native/include/glm/glm.hpp"
#include "../packages/glm.0.9.9.800/build/native/include/glm/ext.hpp"

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
			return value >= rangeL && value < rangeR + BoundCheckEpsilon;
		return value >= rangeR && value < rangeL + BoundCheckEpsilon;
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

		float getSlope2D_XY() const;

		bool check2D_XY(const SSegmentBound& other, glm::vec3& outCross) const;
		bool check2D_XY(const SPointBound& point) const;
		
		bool check(const struct SPlaneBound& plane, glm::vec3& outCross) const;
		bool check(const struct SSphereBound& sphere, glm::vec3& outCross) const;
	};

	struct SPlaneBound : public SBound
	{
		SPlaneBound() : SBound(EBoundType::PLANE) {}
		SPlaneBound(const glm::vec3& norm, const glm::vec3& loc);

		glm::vec3 normal_;
		float d_;

		float getDistanceOnPlane(const glm::vec3& loc) const;
	};

	struct SSphereBound : public SBound
	{
		SSphereBound() : SBound(EBoundType::SPHERE) {}
		SSphereBound(const glm::vec3& pos, float r) : SSphereBound()
		{
			loc_ = pos;
			radius_ = r;
		}

		glm::vec3 loc_;
		float radius_;
	};

	struct SCubeBound : public SBound
	{
		SCubeBound() : SBound(EBoundType::CUBE) {}

		glm::vec3 centerPos_;
		glm::vec3 size_;
		glm::vec3 dir_[3];
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
