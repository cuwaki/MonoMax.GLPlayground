#pragma once

#include "CSystemBase.h"
#include "../MonoMax.EngineCore/RenderingEngine.h"

#include <forward_list>
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
		enum class EGizmoMode : char
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
		void AddSelectedActor(CMap* itsMap, class CActor* actor);
		void RemoveSelectedActor(class CActor* actor);
		void ClearSelectedActors();

	protected:
		std::forward_list<class CActor*> selectedActors_;
		std::multimap<class CActor*, class CActor*> gizmoActors_;

		EGizmoMode gizmoMode_;
	};
};
