#include "CGInterf_Reflection.h"

namespace MonoMaxGraphics
{
	template<typename T>
	SGReflectionStreamOut<T>& SGReflectionStreamOut<T>::operator<<(const SGReflection& reflData)
	{
		//out_ = reflData;¿©±â ÇÒ Â÷·Ê

		return *this;
	}

	template<typename T>
	SGReflectionStreamIn<T>& SGReflectionStreamIn<T>::operator>>(SGReflection& reflData)
	{
		//reflData = in_;¿©±â ÇÒ Â÷·Ê

		return *this;
	}

	void test()
	{
		CGObject ob;
		SGReflection a(ob);
		SGStringStreamOut o;
		o << a;
		SGStringStreamIn i;
		i >> a;
	}
};
