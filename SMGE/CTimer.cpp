#include "CTimer.h"

namespace SMGE
{
	CTimer::CTimer()
	{
	}

	CTimer::CTimer(CTimer* parent)
	{
		parent_ = parent;
	}

	CTimer::~CTimer()
	{
	}

	TimeT CTimer::current() const	// ms
	{
		if (isStarted_ == false)
			return 0;

		return SCast<TimeT>((clockMS() - startedMS_) * ratio_);
	}

	TimeT CTimer::clockMS() const	// ms
	{
		auto c = clock() / ((float)CLOCKS_PER_SEC);
		return SCast<TimeT>(c * 1000.f);	// to MS
	}

	void CTimer::setRatio(float ratio)
	{
		ratio_ = ratio;
	}

	float CTimer::getRatio(bool isMy)
	{
		float ratio = ratio_;

		if (isMy == false && parent_)
		{	// 끝까지 올라가면서 누적
			ratio += parent_->getRatio(isMy);
		}

		// 다 올라왔다
		return ratio;
	}

	void CTimer::start()
	{
		isStarted_ = true;
		startedMS_ = clockMS();
	}

	void CTimer::pause()
	{
		if (isPaused_)
			return;

		// 자식 타이머들도 다 퍼즈해야한다 - 즉 child_ 를 알아야함, 양방향 연결 필요

		// 현재 시간 갱신, 백업해놓고 계속 current에서 이 값으로 리턴해야한다
	}

	void CTimer::resume()
	{
		if (isPaused_ == false)
			return;
	}
}
