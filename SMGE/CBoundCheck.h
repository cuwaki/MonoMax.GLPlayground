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

	// 순서 바꾸면 여기저기 수정해야함
	enum class EBoundType
	{
		POINT,
		SEGMENT,

		// 면
		PLANE,
		TRIANGLE,
		QUAD,
		CIRCLE,
		
		// 입체
		SPHERE,
		CUBE,

		// 바운딩박스
		AABB,
		//OBB,	OBB 는 자동으로 모델 축에 정렬된 CUBE와 동일하다

		MAX,
	};

	// EBoundType 로 2차원 매트릭스를 만들어서 각 바운드타입이 서로 check 를 실행할 수 있는지 저장
	extern const bool BoundCheckMatrix[int32(EBoundType::MAX)][int32(EBoundType::MAX)];

	struct SAABB;
	struct SSegmentBound;

	struct SBound
	{
		SBound(EBoundType t) : type_(t) {}

		EBoundType type_ = EBoundType::MAX;

		virtual operator SAABB() const;

		// 메인 check 함수이다, 여기서 type_ 에 따라서 각 객체로 분배시킨다
		bool check(const SBound& other, SSegmentBound& outCrossSeg) const;

		virtual bool check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const = 0;
	};

	struct SPointBound : public SBound
	{
		SPointBound() : SBound(EBoundType::POINT) {}
		SPointBound(float x, float y, float z) : SPointBound(glm::vec3(x, y, z)) {}
		SPointBound(const glm::vec3& p) : SPointBound()	{	loc_ = p;	}
		SPointBound(glm::vec3&& p) : SPointBound() { loc_ = p; }

		glm::vec3 loc_;

		virtual bool check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const override;
		bool check(const struct SPointBound& point, SSegmentBound& outCrossSeg) const;

		bool operator==(const SPointBound& other) const;

		virtual operator SAABB() const override;

		operator glm::vec2() const { return glm::vec2(loc_.x, loc_.y); }
		operator glm::vec3() const { return loc_; }
		operator glm::vec4() const { return glm::vec4(loc_.x, loc_.y, loc_.z, 1.f); }
	};

	struct SSegmentBound : public SBound
	{
		SSegmentBound() : SBound(EBoundType::SEGMENT)
		{
			start_ = end_ = { 0.f, 0.f, 0.f };
		}
		SSegmentBound(const glm::vec3& p) : SSegmentBound() { start_ = end_ = p; }
		SSegmentBound(const glm::vec3& s, const glm::vec3& e) : SSegmentBound()
		{
			start_ = s;
			end_ = e;
		}

		glm::vec3 start_, end_;

		inline bool isPoint() const { return start_ == end_; }
		inline bool isNullPoint() const { return start_ == end_ && end_ == glm::vec3(0.f, 0.f, 0.f); }

		void reverse();
		float getDistanceFromSegment(const glm::vec3& loc) const;

		float getSlope2D_XY() const;
		bool operator==(const SSegmentBound& other) const;

		virtual bool check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const override;

		bool check2D_XY(const SPointBound& point, SSegmentBound& outCross) const;
		bool check2D_XY(const SSegmentBound& other, SSegmentBound& outCross) const;
		bool check(const struct SPlaneBound& plane, SSegmentBound& outCross) const;
		bool check(const struct STriangleBound& tri, SSegmentBound& outCross, bool isCheckWithPlane = true) const;
		bool check(const struct SQuadBound& quad, SSegmentBound& outCross, bool isCheckWithPlane = true) const;
		bool check(const struct SCircleBound& quad, SSegmentBound& outCross, bool isCheckWithPlane = true) const;
		bool check(const struct SSphereBound& sphere, SSegmentBound& outCross) const;
		bool check(const struct SCubeBound& cube, SSegmentBound& outCross) const;

		virtual operator SAABB() const override;
	};

	struct SPlaneBound : public SBound
	{
		SPlaneBound() : SBound(EBoundType::PLANE) {}
		SPlaneBound(const glm::vec3& norm, const glm::vec3& loc);

		float getSignedDistanceFromPlane(const glm::vec3& loc) const;

		bool isInFront(const glm::vec3& loc) const;
		bool isOnPlane(const glm::vec3& loc) const;
		bool isInBack(const glm::vec3& loc) const;

		virtual bool check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const override;
		bool check(const SPointBound& point, SSegmentBound& outCross) const;
		bool check(const struct SPlaneBound& plane, SSegmentBound& outCross) const;

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
		STriangleBound(STriangleBound&& other) noexcept
		{
			*this = std::move(other);
		}

		bool operator==(const STriangleBound& other) const;
		
		STriangleBound& operator=(const STriangleBound& other) noexcept
		{
			this->normal_ = other.normal_;
			this->d_ = other.d_;

			p0_ = other.p0_;
			p1_ = other.p1_;
			p2_ = other.p2_;
			cachedSegments_.reset();
			return *this;
		}
		STriangleBound& operator=(STriangleBound&& other) noexcept
		{
			this->normal_ = std::move(other.normal_);
			this->d_ = std::move(other.d_);

			p0_ = std::move(other.p0_);
			p1_ = std::move(other.p1_);
			p2_ = std::move(other.p2_);
			cachedSegments_ = std::move(other.cachedSegments_);
			return *this;
		}

		void getSegments(SSegmentBound(&outSegs)[3]) const;

		const glm::vec3& getP0() const { return p0_; }
		const glm::vec3& getP1() const { return p1_; }
		const glm::vec3& getP2() const { return p2_; }

		virtual operator SAABB() const override;

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
		SQuadBound(SQuadBound&& other) noexcept
		{
			*this = std::move(other);
		}

		bool operator==(const SQuadBound& other) const;

		SQuadBound& operator=(const SQuadBound& other) noexcept
		{
			this->normal_ = other.normal_;
			this->d_ = other.d_;

			p0_ = other.p0_;
			p1_ = other.p1_;
			p2_ = other.p2_;
			p3_ = other.p3_;
			cachedSegments_.reset();
			return *this;
		}
		SQuadBound& operator=(SQuadBound&& other) noexcept
		{
			this->normal_ = std::move(other.normal_);
			this->d_ = std::move(other.d_);

			p0_ = std::move(other.p0_);
			p1_ = std::move(other.p1_);
			p2_ = std::move(other.p2_);
			p3_ = std::move(other.p3_);
			cachedSegments_ = std::move(other.cachedSegments_);
			return *this;
		}

		void getSegments(SSegmentBound(&outSegs)[4]) const;

		const glm::vec3& getP0() const { return p0_; }
		const glm::vec3& getP1() const { return p1_; }
		const glm::vec3& getP2() const { return p2_; }
		const glm::vec3& getP3() const { return p3_; }

		virtual operator SAABB() const override;

	protected:
		glm::vec3 p0_, p1_, p2_, p3_;
		mutable std::unique_ptr<SSegmentBound[]> cachedSegments_;
	};

	struct SCircleBound : public SPlaneBound
	{
		static constexpr int CIRCUMFERENCE_SEGMENT_MAX = 18;	// 원주를 몇분할 하여 세그먼트로 만들지

		SCircleBound();
		SCircleBound(const glm::vec3& norm, const glm::vec3& center, float radius);
		SCircleBound(const SCircleBound& other) noexcept
		{
			*this = other;
		}
		SCircleBound(SCircleBound&& other) noexcept
		{
			*this = std::move(other);
		}

		bool operator==(const SCircleBound& other) const;

		SCircleBound& operator=(const SCircleBound& other) noexcept
		{
			this->normal_ = other.normal_;
			this->d_ = other.d_;

			loc_ = other.loc_;
			radius_ = other.radius_;

			cachedSegments_.reset();
			return *this;
		}
		SCircleBound& operator=(SCircleBound&& other) noexcept
		{
			this->normal_ = std::move(other.normal_);
			this->d_ = std::move(other.d_);

			loc_ = std::move(other.loc_);
			radius_ = std::move(other.radius_);

			cachedSegments_ = std::move(other.cachedSegments_);
			return *this;
		}

		void getSegments(SSegmentBound(&outSegs)[CIRCUMFERENCE_SEGMENT_MAX]) const;
		int32 getSegmentMax() const { return CIRCUMFERENCE_SEGMENT_MAX; }

		const glm::vec3& getCenterPos() const { return loc_; }
		float getRadius() const { return radius_; }

		virtual operator SAABB() const override;

		const glm::vec3 getPerp() const { return cachedPerp_; }

	protected:
		void cachePerp() const;

		glm::vec3 loc_;
		float radius_;

		mutable glm::vec3 cachedPerp_;

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

		virtual bool check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const override;
		bool check(const SPointBound& point, SSegmentBound& outCrossSegment) const;
		bool check(const SPlaneBound& plane, SSegmentBound& outCrossSegment) const;
		bool check(const STriangleBound& tri, SSegmentBound& outCrossSegment) const;
		bool check(const SQuadBound& quad, SSegmentBound& outCrossSegment) const;
		bool check(const SSphereBound& sphere, SSegmentBound& outCrossSegment) const;
		bool check(const SCubeBound& cube, SSegmentBound& outCrossSegment) const;

		const glm::vec3& getCenterPos() const { return loc_; }
		float getRadius() const { return radius_; }

		virtual operator SAABB() const override;

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

		virtual bool check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const override;
		bool check(const SPointBound& point, SSegmentBound& outCrossSegment) const;
		bool check(const SCubeBound& cube, SSegmentBound& outCrossSegment) const;

		void getQuads(SQuadBound (&outQuads)[6], bool isJustWantFrontAndBack = false) const;

		const glm::vec3& getEulerAxis(int axis) const { return eulerAxis_[axis]; }
		float getSize(int axis) const { return axis == 0 ? size_.x : (axis == 1 ? size_.y : size_.z); }

		float getFarthestDistance() const { return radius_; }

		virtual operator SAABB() const override;

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

		bool isValid() const { return min_ != max_; }

		virtual bool check(EBoundType otherType, const SBound& other, SSegmentBound& outCrossSeg) const override;
		bool check(const SPointBound& point, SSegmentBound& outCross) const;
		bool check(const SAABB& other, SSegmentBound& outCross) const;

		inline const glm::vec3& min() const;
		inline const glm::vec3& max() const;
		inline glm::vec3 center() const;
		std::initializer_list<SPointBound> points() const;

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
