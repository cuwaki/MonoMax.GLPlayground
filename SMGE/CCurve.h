#pragma once

#include "CTimer.h"

namespace SMGE
{
/*    커브 객체
        0 ~1 의 사이에서 time 에 대한 value 의 함수
        value = function(time)
        지원할 수식
        1차식
        2차식
        3차식
        sin
        cos
        ...
        리버스 기능 ?
        리피트 ? ?
        리버스 ?
        리스타트 ?*/

    using CurveDurationT = float;

    enum class ECurveType : unsigned char
    {
        Linear = 0, // 1차
        Quad,   // 2차
        Cubic,  // 3차
        Sin,
        Cos,
        Max,
    };

    template<typename T>
    inline T FCurveLinear(const T& b, const T& ev, CurveDurationT x)
    {
        const auto a = (ev - b);
        return (a * x) + b;
    }

    template<typename T>
    inline T FCurveQuad(const T& b, const T& ev, CurveDurationT x)
    {   // f: y=-(x-1)^(2)+1
        const auto a = (ev - b);
        const auto squaredX = (x - 1.f) * (x - 1.f);
        return a * (-1.f * squaredX + 1.f) + b;
    }

    template<typename T>
	class CCurve
	{
	public:
        CCurve()
        {
        }

		CCurve(CTimer* usingTimer)
        {
            setTimer(usingTimer);
        }

        void setTimer(CTimer* usingTimer)
        {
            usingTimer_ = usingTimer;
        }

        T current()
        {
            T ret{ 0 };

            if(usingTimer_ == nullptr)
                return ret;

            auto elapsed = duration_ - (endTime_ - usingTimer_->current());
            CurveDurationT cur = std::min(1.0f, elapsed / duration_);

            switch (type_)
            {
            case ECurveType::Linear: ret = FCurveLinear(startValue_, endValue_, cur);   break;
            case ECurveType::Quad: ret = FCurveQuad(startValue_, endValue_, cur);   break;
            default:
                break;
            }

            if (cur == 1.0f)
            {
                finish();
            }

            return ret;
        }

        void setCurveType(ECurveType et)
        {
            type_ = et;
        }

        void start(const T& sv, const T& ev, TimeT duration)
        {
            if (usingTimer_ == nullptr)
                return;

            endTime_ = duration + usingTimer_->current();
            duration_ = static_cast<CurveDurationT>(duration);

            startValue_ = sv;
            endValue_ = ev;
        }

        bool isRunning() const
        {
            return endTime_ != 0;
        }

        void finish()
        {
            endTime_ = 0;
            duration_ = 0;
        }

    protected:
        CTimer* usingTimer_ = nullptr;
        ECurveType type_ = ECurveType::Linear;

        T startValue_{ 0 };
        T endValue_{ 0 };
        
        CurveDurationT duration_ = 0;
        TimeT endTime_ = 0;
	};
}
