#include "CGEEngineBase.h"

CGEEngineBase::CGEEngineBase()
{
}

CGEEngineBase::~CGEEngineBase()
{
}

std::int32_t CGEEngineBase::main()
{
	if (gameMain_ != nullptr)
		gameMain_->main(0.01f);

	return 0;
}