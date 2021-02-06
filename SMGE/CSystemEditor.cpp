#include "CSystemEditor.h"
#include "CEngineBase.h"
#include "../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	CSystemEditor::CSystemEditor(class CEngineBase* engine) : CSystemBase(engine)
	{
	}

	void CSystemEditor::OnStartSystem()
	{
		auto re = engine_->GetRenderingEngine();
	}

	void CSystemEditor::OnEndSystem()
	{

	}

	void CSystemEditor::OnChangedSystemState(const CString& stateName)
	{

	}
};
