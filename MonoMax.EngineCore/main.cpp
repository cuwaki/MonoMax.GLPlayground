#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include "common.h"
#include "EngineCore.h"

int main()
{
	using namespace SMGE;

	auto graphicsEngine = std::make_unique<nsRE::CRenderingEngine>(1024, 768, 1.0f/*::GetWindowDPIRate()*/);
	graphicsEngine->Init();

	while (true)
	{
		// Tick 쓰레드와 Render 쓰레드를 만들어서 돌려라
		//graphicsEngine->Render(nullptr);
	}

	return 0;
}
