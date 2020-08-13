#pragma once

#include "GECommonIncludes.h"
#include <ctime>

namespace SMGE
{
	using TimeT = std::clock_t;	// long - MS 단위이며, 최대치는 대략 35000분, 583시간, 24일

    //선행
    //    타이머
    //    게임 타이머 == 현실 타이머
    //    월드 타이머
    //    액터 타이머
    //    컴포넌트 타이머
    //    기능
    //    시작 / 끝 / 리셋
    //    증가율
    //    is리버서블 ?
    //    repeat ?
    //    알람

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
