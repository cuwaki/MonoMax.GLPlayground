#pragma once

#include "CTimer.h"
#include <cmath>

namespace SMGE
{
    enum class ECurveType : unsigned char
    {
        Linear = 0, // 1차
        Quad_In,   // 2차
        Quad_Out,
        Quad_InOut,
        Cubic_In,  // 3차
        Cubic_Out,
        Cubic_InOut,
        Sin,
        Cos,
        Max,
    };

    constexpr float fPI = 3.141592f;
    constexpr float f2PI = fPI * 2.f;

    // 이하의 커브 함수들에 들어가는 x 는 0.f ~ 1.f 로 정규화된 값이다
    // https://www.gizma.com/easing/#cub1 - 여기 있는 것들을 옮겨나가자

    template<typename T>
    inline T FCurveLinear(const T& sv, const T& ev, DurationRatioT x)
    {
        const auto a = (ev - sv);
        return (a * x) + sv;
    }

    template<typename T>
    inline T FCurveQuadEaseIn(const T& sv, const T& ev, DurationRatioT x)
    {
        x -= 1.f;   // 0~1 -> -1~0

        const auto a = (ev - sv);
        const auto squaredX = -(x * x) + 1.f;
        return a * squaredX + sv;
    }

    template<typename T>
    inline T FCurveQuadEaseOut(const T& sv, const T& ev, DurationRatioT x)
    {
        const auto a = (ev - sv);
        const auto squaredX = x * x;
        return a * squaredX + sv;
    }

    template<typename T>
    inline T FCurveCubicEaseIn(const T& sv, const T& ev, DurationRatioT x)
    {
        const auto a = (ev - sv);
        return a * (x * x * x) + sv;
    }

    template<typename T>
    inline T FCurveCubicEaseOut(const T& sv, const T& ev, DurationRatioT x)
    {
        x -= 1.f;   // 0~1 -> -1~0

        const auto a = (ev - sv);
        return a * (x * x * x + 1.f) + sv;
    }

    template<typename T>
    inline T FCurveSin(const T& sv, const T& ev, DurationRatioT x)
    {   // y = sin(x) * a
        const auto a = (ev - sv);
        const auto theta = x * f2PI;
        return (std::sin(theta) * a) + sv;
    }

    template<typename T>
    inline T FCurveCos(const T& sv, const T& ev, DurationRatioT x)
    {   // y = cos(x) * a
        const auto a = (ev - sv);
        const auto theta = x * f2PI;
        return (std::cos(theta) * a) + sv;
    }

    template<typename T>
	class CInterpolation
	{
	public:
        CInterpolation()
        {
        }

		CInterpolation(CTimer* usingTimer)
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
            DurationRatioT cur = std::min(1.0f, elapsed / duration_);

            switch (type_)
            {
            case ECurveType::Linear: ret = FCurveLinear(startValue_, endValue_, cur);   break;
            case ECurveType::Quad_In: ret = FCurveQuadEaseIn(startValue_, endValue_, cur);   break;
            case ECurveType::Quad_Out: ret = FCurveQuadEaseOut(startValue_, endValue_, cur);   break;
            case ECurveType::Cubic_In: ret = FCurveCubicEaseIn(startValue_, endValue_, cur);   break;
            case ECurveType::Cubic_Out: ret = FCurveCubicEaseOut(startValue_, endValue_, cur);   break;
            case ECurveType::Sin: ret = FCurveSin(startValue_, endValue_, cur);   break;
            case ECurveType::Cos: ret = FCurveCos(startValue_, endValue_, cur);   break;
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
            duration_ = SCast<DurationT>(duration);

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
        
        DurationT duration_ = 0;
        TimeT endTime_ = 0;
	};
}
