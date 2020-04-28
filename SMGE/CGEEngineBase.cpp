#include "CGEEngineBase.h"

namespace MonoMaxGraphics
{
	CGEEngineBase::CGEEngineBase()
	{
	}

	CGEEngineBase::~CGEEngineBase()
	{
	}

	std::int32_t CGEEngineBase::main()
	{
		if (gameMain_ != nullptr)
			gameMain_->Tick(0.01f);

		return 0;
	}
}
