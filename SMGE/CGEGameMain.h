#pragma once

#include "GECommonIncludes.h"

struct SGEGameSettings
{
	CWString gameProjectName_;
};

class CGEGameMain
{
public:
	CGEGameMain();

	virtual void main(float timeDelta) = 0;

public:
	SGEGameSettings* gameSettings_;
};
