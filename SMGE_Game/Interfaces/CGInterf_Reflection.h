#pragma once

#include "../GCommonIncludes.h"
#include "CGInterface.h"
#include "../Objects/CGObject.h"

namespace MonoMaxGraphics
{
	struct SGReflection;

	template<typename T>
	struct SGReflectionStream
	{
	};

	template<typename T>
	struct SGReflectionStreamOut : public SGReflectionStream<T>
	{
		SGReflectionStreamOut& operator<<(const SGReflection& reflData);

		T out_;
	};

	template<typename T>
	struct SGReflectionStreamIn : public SGReflectionStream<T>
	{
		SGReflectionStreamIn& operator>>(SGReflection& reflData);

		T in_;
	};

	using SGStringStreamOut = SGReflectionStreamOut<CWString>;
	using SGStringStreamIn = SGReflectionStreamIn<CWString>;

	struct SGReflection
	{
		SGReflection(const CGObject& obj) :
			reflClassName_(obj.reflClassName_)
		{
		}

		const CWString& reflClassName_;
	};

	class CGInterf_Reflection : public CGInterface
	{
	public:
		virtual SGReflection& getReflection() = 0;
		virtual const SGReflection& getReflection() const { return getReflection(); }
	};
};
