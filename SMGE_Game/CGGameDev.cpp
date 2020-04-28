#include "CGGameDev.h"
#include "Objects/CGActor.h"

namespace MonoMaxGraphics
{
	void CGGameDev::Tick(float dt)
	{
		Super::Tick(dt);

		CGActor actor;

		SGStringStreamOut strOut;
		strOut << actor.getReflection();

		SGStringStreamIn strIn;
		strIn >> actor.getReflection();
	}
};
