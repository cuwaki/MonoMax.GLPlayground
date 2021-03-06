#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include "common.h"
#include "EngineCore.h"

int main()
{
	using namespace SMGE;

	auto graphicsEngine = std::make_unique<nsRE::CRenderingEngine>();
	graphicsEngine->Init();

	while (true)
	{
		// Tick 쓰레드와 Render 쓰레드를 만들어서 돌려라
		//graphicsEngine->Render(nullptr);
	}

	return 0;
}
