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
		// Tick ������� Render �����带 ���� ������
		//graphicsEngine->Render(nullptr);
	}

	return 0;
}
