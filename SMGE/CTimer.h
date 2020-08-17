#pragma once

#include "GECommonIncludes.h"
#include <ctime>

namespace SMGE
{
	using TimeT = float;	// std::clock_t �� long �̰� MS �����̸�, �ִ�ġ�� �뷫 35000��, 583�ð�, 24��
    using DurationT = float;
    using DurationRatioT = DurationT;   // 0.f ~ 1.f

    enum class ETimerCategory : unsigned char
    {
        Real = 0,
        World,
        Object,
        Max,
    };

	class CTimer
	{
	public:
        CTimer();
		CTimer(CTimer* parent);

		~CTimer();

		TimeT current() const;  // ms

        void start();
        void pause();
        void resume();

        void setRatio(float ratio);
        float getRatio(bool isMy);

    protected:
        TimeT clockMS() const;

    protected:
        ETimerCategory category_ = ETimerCategory::Max;
        float ratio_ = 1.0f;

        TimeT startedMS_ = 0;
        bool isStarted_ = false;
        bool isPaused_ = false;

        CTimer* parent_ = nullptr;
	};
}
