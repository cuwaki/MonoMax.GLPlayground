#include "CSystemEditor.h"
#include "CEngineBase.h"
#include "Objects/CMap.h"
#include "Objects/CCameraActor.h"
#include "Objects/CCollideActor.h"
#include "Objects/CGizmoActor.h"
#include "Assets/CAssetManager.h"
#include "../MonoMax.EngineCore/RenderingEngine.h"
#include <experimental/map>

namespace SMGE
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CRenderPassGizmo::CRenderPassGizmo(CSystemBase* system) : CRenderPassWithSystem(system)
	{
	}

	void CRenderPassGizmo::RenderTo(const glm::mat4& V, const glm::mat4& VP, nsRE::CRenderTarget*& writeRT, nsRE::CRenderTarget*& readRT)
	{
		auto dummy = glm::vec3(0);	// 여기선 라이트 안씀

		const auto system = GetSystem();
		const auto& allActors = system->GetAllActors();

		constexpr auto setSize = 1024;

		using TStackSetRenderModels = StackSet<nsRE::RenderModel*, setSize>;	// 여기 - 나중에 RenderModel 다양해지면 터질 수 있다
		TStackSetRenderModels::allocator_type::arena_type stackArena;
		TStackSetRenderModels renderModels(stackArena);
		renderModels.reserve(setSize);

		// 1. 그려질 렌더모델 수집
		for (const auto& actor : allActors)
		{
			if (actor->IsRendering() == true &&
				actor->AmIEditorActor() == true)	// 기즈모는 에디터 액터이다!!!
			{
				for (auto comp : actor->getAllComponents())
				{
					auto drawComp = dynamic_cast<CDrawComponent*>(comp);
					if (drawComp && drawComp->IsRendering())
					{
						renderModels.insert(drawComp->GetRenderModel());
					}
				}
			}
		}

		// 2. 실제로 렌더링
		if (renderModels.size() > 0)
		{	// 중복 코드 정리 필요
			writeRT->BindFrameBuffer();

			// 여기 - 월드 그려진 후에 그 위에 그려야하는 것이므로 지우면 안된다, 자동화 할 수 있는 판단이 필요하다
			//writeRT->SetClearColor(nsRE::ColorConst::Blue);	// 테스트 코드 ㅡ 빨강으로 칠해서 확인하기 위함
			//writeRT->ClearFrameBuffer();

			// CRenderingPass 와의 엮인 처리로 좀 낭비가 있다 - ##renderingpasswith03
			for (auto rm : renderModels)
			{
				if (rm->GetShaderID() > 0)
				{
					rm->UseShader(V, dummy);	// 셰이더 마다 1회
					rm->BeginRender();
					rm->Render(VP);
					rm->EndRender();
				}
			}
			writeRT->UnbindFrameBuffer();
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CSystemEditor::CSystemEditor(class CEngineBase* engine) : CSystemBase(engine)
	{
	}

	void CSystemEditor::OnLinkWithRenderingEngine()
	{
		__super::OnLinkWithRenderingEngine();

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		auto re = renderingEngine_;
		auto& renderingPasses = re->GetRenderingPasses();

		// 월드 렌더 패스 등록
		auto& worldPass = renderingPasses.emplace_back(std::make_unique<CRenderPassWorld>(this));
		// 기즈모용 렌더 패스 등록
		auto& gizmoPass = renderingPasses.emplace_back(std::make_unique<CRenderPassGizmo>(this));

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 입력 처리 등록
		// 1.
		auto EditorInputForCameraActor = [this](const CUserInput& userInput)
		{
			auto currentMap = Globals::GCurrentMap;
			if (currentMap == nullptr)
				return false;

			if (auto currentCamera = currentMap->GetCurrentCamera())
			{
				constexpr float deltaTime = 1000.f / 60.f;	// 테스트 코드 - 이거 하드코딩이고 다른 데 또 있으니 검색해라

				auto& camTransform = currentCamera->getTransform();

				// 이동
				auto currentPos = camTransform.GetFinalPosition();

				constexpr float moveSpeed = 20.0f / 1000.f;

				if (userInput.IsPressed('A'))
					currentPos += camTransform.GetFinalLeft() * deltaTime * moveSpeed;
				if (userInput.IsPressed('D'))
					currentPos -= camTransform.GetFinalLeft() * deltaTime * moveSpeed;
				if (userInput.IsPressed('W'))
					currentPos += camTransform.GetFinalFront() * deltaTime * moveSpeed;
				if (userInput.IsPressed('S'))
					currentPos -= camTransform.GetFinalFront() * deltaTime * moveSpeed;

				camTransform.Translate(currentPos);

				// 회전
				constexpr float angleSpeed = 48.f / 1000.f;

				static glm::vec2 RPressedPos;

				const bool isJustRPress = userInput.IsJustPressed(VK_RBUTTON);
				const bool isRPress = userInput.IsPressed(VK_RBUTTON);

				if (isJustRPress)
					RPressedPos = userInput.GetMousePosition();
				
				if (isJustRPress == false && isRPress == true)
				{
					auto mouseMoved = RPressedPos - userInput.GetMousePosition();

					// 누적하기
					static float yawDegrees = 180.f, pitchDegrees = 0.f;
					yawDegrees += mouseMoved.x * angleSpeed;
					pitchDegrees += mouseMoved.y * angleSpeed;

					// 오일러 처리
					camTransform.RotateEuler({ pitchDegrees, yawDegrees, 0.f }, true);

					// 누적해서 - 쿼터니언 처리
					/*auto rotPit = glm::rotate(SMGE::nsRE::TransformConst::Mat4_Identity, pitchDegrees, { 1.f, 0.f, 0.f });
					auto rotYaw = glm::rotate(SMGE::nsRE::TransformConst::Mat4_Identity, yawDegrees, { 0.f, 1.f, 0.f });
					auto newDir = rotYaw * rotPit * glm::vec4(0.f, 0.f, 1.f, 0.f);
					auto newUp = rotYaw * rotPit * glm::vec4(0.f, 1.f, 0.f, 0.f);
					*/

					/* 누적 안하고 쿼터니언 처리 - 버그 있어서 제대로 안됨
					//const auto yawDegrees = mouseMoved.x * angleSpeed;
					//const auto pitchDegrees = mouseMoved.y * angleSpeed;

					//static auto yawDegrees = 15.f;
					//static auto pitchDegrees = 0.f;

					// 여기 - RotateEuler 등의 통합 함수로 처리해야겠다, 나중에 여기저기서 회전 적용 순서 다른 문제 생길 수 있음, 다른 곳도 회전 적용 순서를 통합 처리해라
					// XYZ 순서로 적용하는 것이 주의
					const auto rotPit = glm::rotate(nsRE::TransformConst::Mat4_Identity, glm::radians(pitchDegrees), camTransform.GetFinalLeft());

					// 1 - 노 누적, 피치에 따른 업 변화
					const auto pitchedFinalUp = rotPit * glm::vec4(camTransform.GetFinalUp(), 0.f);
					const auto rotPitYaw = glm::rotate(rotPit, glm::radians(yawDegrees), glm::vec3(pitchedFinalUp));

					// 2 - 노 누적, 업 변화 없음
					//const auto rotYaw = glm::rotate(nsRE::TransformConst::Mat4_Identity, glm::radians(yawDegrees), camTransform.GetFinalUp());
					//const auto rotPitYaw = rotYaw * rotPit;
					//const auto pitchedFinalUp = camTransform.GetFinalUp();

					const auto finalFront = camTransform.GetFinalFront();
					const auto newDir = rotPitYaw * glm::vec4(finalFront, 0.f);
					//const auto newUp = rotPitYaw * glm::vec4(camTransform.GetFinalUp(), 0.f);
					const auto newUp = pitchedFinalUp;

#ifdef REFACTORING_TRNASFORM
					camTransform.RotateDirection(newDir, newUp);
#else
					camTransform.RotateQuat(newDir);
#endif
					*/

					RPressedPos = userInput.GetMousePosition();
				}
			}

			return false;
		};
		AddProcessUserInputs(EditorInputForCameraActor);

		// 2
		auto EditorProcessUserInput = [this](const CUserInput& userInput)
		{
			if (userInput.IsJustPressed(CUserInput::LBUTTON))
			{
				auto currentMap = Globals::GCurrentMap;
				if (currentMap == nullptr)
					return false;

				// 테스트 코드 - 동적 액터 스폰 샘플 코드 {
				auto mouseScreenPos = userInput.GetMousePosition();

				glm::vec3 ray_origin;
				glm::vec3 ray_direction;
				renderingEngine_->ScreenPosToWorld(mouseScreenPos, ray_origin, ray_direction);

				CCollideActor* rayActor = &StartSpawnActorVARIETY<CCollideActor>(currentMap, true, 
					Args_START currentMap, ECheckCollideRule::NEAREST, false,
					[this, currentMap](class CActor* SRC, class CActor* TAR, const class CBoundComponent* SRC_BOUND, const class CBoundComponent* TAR_BOUND, const SSegmentBound& COLL_SEG)
					{
						// 포인트 표시
						CCollideActor* pointActor = &StartSpawnActorVARIETY<CCollideActor>(currentMap, true, currentMap);
						auto prefab = CAssetManager::LoadAssetDefault<CActor>(Globals::GetGameAssetPath(wtext("/actor/prefabPointActor.asset")));
						pointActor->CopyFromTemplate(prefab->getContentClass());
						{
							// 얘는 단독 액터니까 이렇게 직접 트랜스폼 해줘야한다
							pointActor->getTransform().Translate(COLL_SEG.end_);
						}
						FinishSpawnActor(currentMap, pointActor);
						pointActor->SetLifeTickCount(100);

						//AddSelectedActor(currentMap, TAR);
					});

				auto prefab = CAssetManager::LoadAssetDefault<CActor>(Globals::GetGameAssetPath(wtext("/actor/prefabRayActor.asset")));
				rayActor->CopyFromTemplate(prefab->getContentClass());
				{	// 얘는 단독 액터니까 이렇게 직접 트랜스폼 해줘야한다
					// rayCompo 를 조작하는 게 아니고 rayActor 를 조작하고 있음에 주의!
					float rayLength = renderingEngine_->GetRenderingCamera().GetZFar();
					rayActor->getTransform().Translate(ray_origin);
					rayActor->getTransform().Scale(nsRE::TransformConst::DefaultAxis_Front, rayLength);
#ifdef REFACTORING_TRNASFORM
					rayActor->getTransform().RotateDirection(ray_direction);
#else
					rayActor->getTransform().RotateQuat(ray_direction);
#endif
				}
				FinishSpawnActor(currentMap, rayActor);

				auto targets = rayActor->QueryCollideCheckTargets();
				bool hasCollide = rayActor->CheckCollideAndProcess(targets);
				rayActor->SetLifeTickCount(33);	// 테스트 코드 - 레이 트레이싱 시각화

				if (hasCollide == false)
				{	// 아무것도 선택되지 않았다
					ClearSelectedActors();
				}
				// }
			}
			return false;
		};
		AddProcessUserInputs(EditorProcessUserInput);
	}

	void CSystemEditor::OnDestroyingGameEngine()
	{

	}

	void CSystemEditor::OnChangedSystemState(const CString& stateName)
	{

	}

	void CSystemEditor::AddSelectedActor(CMap* itsMap, class CActor* selActor)
	{
		auto it = std::find(selectedActors_.begin(), selectedActors_.end(), selActor);
		if (it != selectedActors_.end())
		{	// 이미 선택됨이면 토글
			RemoveSelectedActor(selActor);
			return;
		}

		if (itsMap == nullptr)
			return;

		CGizmoActor* gizmo = nullptr;
		switch (gizmoMode_)
		{
		case EGizmoMode::TRANSLATE:
			gizmo = &StartSpawnActorVARIETY<CGizmoActorTranslate>(itsMap, true, Args_START selActor);
			break;
		case EGizmoMode::ROTATE:
			break;
		case EGizmoMode::SCALE:
			break;
		default:
			break;
		}

		if (gizmo != nullptr)
		{
			auto prefab = CAssetManager::LoadAssetDefault<CActor>(Globals::GetGameAssetPath(wtext("/templates/CGizmoActorTranslate.asset")));
			gizmo->CopyFromTemplate(prefab->getContentClass());

			// 트랜스폼 그대로 카피
			gizmo->getTransform().Translate(selActor->getTransform().GetPendingPosition());
#ifdef REFACTORING_TRNASFORM
			gizmo->getTransform().Rotate(selActor->getTransform().GetPendingRotationMatrix());	// 액터에 곧바로 붙어있음
#else
			//gizmo->getTransform().Rotate(selActor->getTransform().GetPendingRotationMatrix());	// 액터에 곧바로 붙어있음	
#endif
			gizmo->getTransform().Scale(selActor->getTransform().GetPendingScales());

			FinishSpawnActor(itsMap, gizmo);

			gizmoActors_.insert(std::make_pair(selActor, gizmo));
			selectedActors_.push_front(selActor);
		}
	}

	void CSystemEditor::RemoveSelectedActor(class CActor* actor)
	{
		for (auto it = gizmoActors_.lower_bound(actor); it != gizmoActors_.upper_bound(actor); it++)
		{
			auto gizmo = it->second;
			gizmo->SetPendingKill();
		}

		std::experimental::erase_if(gizmoActors_, [](const auto& it)
			{
				auto gizmo = it.second;
				return gizmo->IsPendingKill();
			});

		selectedActors_.remove(actor);
	}

	void CSystemEditor::ClearSelectedActors()
	{
		for (auto it = selectedActors_.begin(); it != selectedActors_.end(); ++it)
		{
			RemoveSelectedActor(*it);
		}
	}

	void CSystemEditor::ProcessPendingKill(class CActor* actor)
	{
		__super::ProcessPendingKill(actor);

		auto found = std::find(selectedActors_.begin(), selectedActors_.end(), actor);
		if (found != selectedActors_.end())
		{
			RemoveSelectedActor(actor);
		}
	}
};
