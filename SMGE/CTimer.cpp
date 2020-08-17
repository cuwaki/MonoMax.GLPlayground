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
		{	// ������ �ö󰡸鼭 ����
			ratio += parent_->getRatio(isMy);
		}

		// �� �ö�Դ�
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

		// �ڽ� Ÿ�̸ӵ鵵 �� �����ؾ��Ѵ� - �� child_ �� �˾ƾ���, ����� ���� �ʿ�

		// ���� �ð� ����, ����س��� ��� current���� �� ������ �����ؾ��Ѵ�
	}

	void CTimer::resume()
	{
		if (isPaused_ == false)
			return;
	}
}
