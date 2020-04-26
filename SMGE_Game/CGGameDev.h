#pragma once

#include "../SMGE/CGEGameMain.h"

class CGGameDev : public CGEGameMain
{
	using Super = CGEGameMain;

	virtual void main(float) override;
};
