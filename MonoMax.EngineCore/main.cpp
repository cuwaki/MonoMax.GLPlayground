#include <iostream>
#include <string>
#include <fstream>
#include "common.h"
#include "EngineCore.h"

int main()
{
	using namespace SMGE;

	nsRE::CRenderingEngine* graphicsEngine = new nsRE::CRenderingEngine();
	graphicsEngine->Init();
	while (true)
	{
		// Tick 쓰레드와 Render 쓰레드를 만들어서 돌려라
		//graphicsEngine->Render(nullptr);
	}

	return 0;
}
