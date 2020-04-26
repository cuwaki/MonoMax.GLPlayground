#pragma once

#include "GECommonIncludes.h"
#include "CGEGameMain.h"

struct SGEEngineSettings
{
	glm::vec2 gameScreenSize_{};
};

class CGEEngineBase
{
public:
	CGEEngineBase();
	virtual ~CGEEngineBase();

	std::int32_t main();

protected:
	CGEGameMain* gameMain_;

	SGEEngineSettings *settings_;
};
