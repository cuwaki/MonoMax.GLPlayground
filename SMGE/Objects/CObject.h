#pragma once

#include "../GECommonIncludes.h"

namespace SMGE
{
	class CObject
	{
		friend struct SGReflection;

	public:
		CObject();
		virtual ~CObject() {}

		CWString getClassName() { return className_; }

		virtual void CGCtor();

	protected:
		CWString className_;
	};
};
