#pragma once

#include "CSystemBase.h"
#include "../MonoMax.EngineCore/RenderingEngine.h"
#include <map>

namespace SMGE
{
	class CRenderPassGizmo : public CRenderPassWithSystem
	{
	public:
		CRenderPassGizmo(CSystemBase* system);
		virtual void RenderTo(const glm::mat4& V, const glm::mat4& VP, nsRE::CRenderTarget*& writeRT, nsRE::CRenderTarget*& readRT) override;
	};

	class CSystemEditor : public CSystemBase
	{
	public:
		enum class ETransformEditMode : char
		{
			TRANSLATE = 0,
			ROTATE,
			SCALE,
			MAX,
		};

	public:
		CSystemEditor(class CEngineBase* engine);

		virtual void OnLinkWithRenderingEngine() override;
		virtual void OnDestroyingGameEngine() override;

		virtual void OnChangedSystemState(const CString& stateName) override;

		// CActor 관리
		virtual void ProcessPendingKill(class CActor* actor) override;

	protected:
		void OnSelectActor(CMap* itsMap, class CActor* actor, bool isMultipleAdd);
		auto RemoveSelectedActor(class CActor* actor);
		void ClearSelectedActors();

		void OnLButtonDragging(const CUserInput& userInput);

	protected:
		CForwardList<class CActor*> selectedActors_;
		std::multimap<class CActor*, class CEditorActor*> gizmoActors_;

		ETransformEditMode gizmoMode_;
	};
};
