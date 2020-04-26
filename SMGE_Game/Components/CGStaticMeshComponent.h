#pragma once

#include "../GCommonIncludes.h"
#include "CGDrawComponent.h"
#include "../Assets/CGStaticMeshAsset.h"

class CGStaticMeshComponent : public CGDrawComponent
{
public:
	CGStaticMeshComponent();

	CSharPtr<CGStaticMeshAsset> asset_;
};
