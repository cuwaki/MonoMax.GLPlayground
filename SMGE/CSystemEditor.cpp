#include "CSystemEditor.h"
#include "CEngineBase.h"
#include "Objects/CMap.h"
#include "Objects/CCameraActor.h"
#include "Objects/CCollideActor.h"
#include "Objects/CEditorActor.h"
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
		// 여기 - 라이트 액터 구현 필요함
		auto lightPos = glm::vec3(0);

		const auto system = GetSystem();
		const auto& allActors = system->GetAllActors();

#if IS_EDITOR
		CVector<nsRE::WorldModel*> worldModels;
		worldModels.reserve(2048);
#else
		ShortAllocVector<nsRE::WorldModel*, 2048> worldModelsO;	// 여기 - C3821 때문에 에디터일 때는 사용할 수 없다
		auto& worldModels = worldModelsO();
		worldModels.reserve(2048);
#endif

		// 1. 그려질 월드 모델 수집 - 
		// 최적화 - SystemBase 와의 중복 코드 제거하고, 한번 전체 돌면서 빌드를 한번만 하도록 하자
		for (auto& actor : allActors)
		{
			if (actor->IsRendering() == true && dynamic_cast<CEditorActor*>(actor.get()) != nullptr)
			{
				CascadeTodoPreorder(actor.get(),
					[&worldModels](auto comp)
					{
						auto drawComp = dynamic_cast<CDrawComponent*>(comp);	// 최적화 - isdrawable 같은 함수 만들어서 대체하자
						if (drawComp && drawComp->IsRendering())
							worldModels.push_back(drawComp);
					});
			}
		}

		// 2. 실제로 렌더링
		if (worldModels.size() > 0)
		{	// 렌더모델로 정렬
			std::sort(worldModels.begin(), worldModels.end(), [](auto l, auto r)
				{
					return l->GetRenderModel() < r->GetRenderModel();
				});

			writeRT->BindFrameBuffer();

			// 여기 - 월드가 그려진 버퍼 위에 그려야하는 것이므로 뎁스랑 스텐실만 지우고 덮어그린다
			writeRT->ClearFrameBuffer(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// CRenderingPass 와의 엮인 처리로 좀 낭비가 있다 - ##renderingpasswith03
			auto rm = worldModels[0]->GetRenderModel();
			rm->UseShader(V, lightPos);	// 셰이더 마다 1회
			rm->BeginRender();

			for (auto wm : worldModels)
			{
				if (rm != wm->GetRenderModel())
				{
					rm = wm->GetRenderModel();
					rm->EndRender();

					rm->UseShader(V, lightPos);
					rm->BeginRender();
				}

				rm->Render(VP, wm);
			}
			rm->EndRender();

			writeRT->UnbindFrameBuffer();
		}

		// 핑퐁 안함!
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

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 렌더 패스 등록
		renderingPasses.emplace_back(std::make_unique<CRenderPassWorld>(this));
		renderingPasses.emplace_back(std::make_unique<CRenderPassGizmo>(this));

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

				if (userInput.IsPressed('A'))
					currentPos += camTransform.GetFinalLeft() * deltaTime * Configs::MouseMoveSpeed;
				if (userInput.IsPressed('D'))
					currentPos -= camTransform.GetFinalLeft() * deltaTime * Configs::MouseMoveSpeed;
				if (userInput.IsPressed('W'))
					currentPos += camTransform.GetFinalFront() * deltaTime * Configs::MouseMoveSpeed;
				if (userInput.IsPressed('S'))
					currentPos -= camTransform.GetFinalFront() * deltaTime * Configs::MouseMoveSpeed;

				camTransform.Translate(currentPos);

				// 회전
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
					yawDegrees += mouseMoved.x * Configs::MouseAngleSpeed;
					pitchDegrees += mouseMoved.y * Configs::MouseAngleSpeed;

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
			if (userInput.IsDragging(CUserInput::LButton))
			{
				OnLButtonDragging(userInput);
			}
			else if (userInput.IsJustReleased(CUserInput::LButton))
			{
				if (userInput.WasDragging(CUserInput::LButton))
					return false;	// 드래그 끝이면 아무것도 안함

				auto currentMap = Globals::GCurrentMap;
				if (currentMap == nullptr)
					return false;

				// 테스트 코드 - 동적 액터 스폰 샘플 코드 {
				auto mouseScreenPos = userInput.GetMousePosition();

				glm::vec3 ray_origin;
				glm::vec3 ray_direction;
				renderingEngine_->ScreenPosToWorld(mouseScreenPos, ray_origin, ray_direction);

				CCollideActor* rayActor = &StartSpawnActorVARIADIC<CCollideActor>(currentMap, true, 
					VARIADIC_START currentMap, ECheckCollideRule::NEAREST, false,
					[this, currentMap, &userInput](class CActor* SRC, class CActor* TAR, const class CBoundComponent* SRC_BOUND, const class CBoundComponent* TAR_BOUND, const SSegmentBound& COLL_SEG)
					{
						//// 포인트 표시
						//CCollideActor* pointActor = &StartSpawnActorVARIADIC<CCollideActor>(currentMap, true, currentMap);
						//auto prefab = CAssetManager::LoadAssetDefault<CActor>(Globals::GetGameAssetPath(wtext("/actor/prefabPointActor.asset")));
						//pointActor->CopyFromTemplate(prefab->getContentClass());
						//{
						//	// 얘는 단독 액터니까 이렇게 직접 트랜스폼 해줘야한다
						//	pointActor->getTransform().Translate(COLL_SEG.end_);
						//}
						//FinishSpawnActor(currentMap, pointActor);
						//pointActor->SetLifeTickCount(100);

						const auto isMultipleAdd = userInput.IsPressed(CUserInput::AddMultipleKey);
						OnSelectActor(currentMap, TAR, isMultipleAdd);
					});

				if (rayActor)
				{
					auto prefab = CAssetManager::LoadAssetDefault<CActor>(Globals::GetGameAssetPath(wtext("/actor/prefabRayActor.asset")));
					rayActor->CopyFromTemplate(prefab->getContentClass());	// 여기 - CopyFromTemplate 이게 FinishSpawnActor 보다 일찍 호출되어야한다!!, 안그러면 staticTag 나 actorKey 등이 초기화되어버린다
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

	auto CSystemEditor::RemoveSelectedActor(class CActor* actor)
	{
		auto found = std::find(selectedActors_.begin(), selectedActors_.end(), actor);
		if (found != selectedActors_.end())
		{	// actor 연관된 기즈모 삭제
			for (auto it = gizmoActors_.lower_bound(actor); it != gizmoActors_.upper_bound(actor); it++)
			{
				auto gizmo = it->second;
				gizmo->SetPendingKill();
			}
			std::experimental::erase_if(gizmoActors_, [](const auto& it)
				{
					const auto gizmo = it.second;
					return gizmo->IsPendingKill();
				});

			selectedActors_.remove(actor);
		}
	}

	void CSystemEditor::OnSelectActor(CMap* itsMap, class CActor* selActor, bool isMultipleAdd)
	{
		if (itsMap == nullptr)
			return;

		auto it = std::find(selectedActors_.begin(), selectedActors_.end(), selActor);
		if (it != selectedActors_.end())
		{	// 이미 선택됨이면 토글
			RemoveSelectedActor(selActor);
			return;
		}

		CEditorActor* gizmo = &StartSpawnActorVARIADIC<CGizmoActorTransform>(itsMap, true, VARIADIC_START selActor);
		if (gizmo != nullptr)
		{
			auto prefab = CAssetManager::LoadAssetDefault<CActor>(Globals::GetGameAssetPath(wtext("/templates/CGizmoActorTransform.asset")));
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
		}

		if (isMultipleAdd == false)
			ClearSelectedActors();

		selectedActors_.push_front(selActor);
	}

	void CSystemEditor::ClearSelectedActors()
	{
		if (selectedActors_.empty())
			return;

		do
		{
			RemoveSelectedActor(*selectedActors_.begin());
		} while (selectedActors_.empty() == false);
	}

	void CSystemEditor::ProcessPendingKill(class CActor* actor)
	{
		__super::ProcessPendingKill(actor);

		RemoveSelectedActor(actor);
	}

	void CSystemEditor::OnLButtonDragging(const CUserInput& userInput)
	{
		auto currentMap = Globals::GCurrentMap;
		if (currentMap == nullptr)
			return;

		auto currentCamera = currentMap->GetCurrentCamera();
		if (currentCamera == nullptr)
			return;

		const auto cameraUp = currentCamera->getTransform().GetPendingUp();
		const auto cameraRight = currentCamera->getTransform().GetPendingLeft() * -1.f;

		const auto diffPixelPerTick = userInput.GetDraggedScreenOffset(CUserInput::LButton);

		const auto screenYMove = cameraUp *		-diffPixelPerTick.y * Configs::MouseMoveSpeed;	// y축은 반전
		const auto screenXMove = cameraRight *	+diffPixelPerTick.x * Configs::MouseMoveSpeed;

		for (auto actor : selectedActors_)
		{
			auto curPos = actor->getTransform().GetPendingPosition();
			curPos -= screenYMove;
			curPos -= screenXMove;
			actor->getTransform().Translate(curPos);

			for (auto it = gizmoActors_.lower_bound(actor); it != gizmoActors_.upper_bound(actor); it++)
			{
				auto gizmo = it->second;
				gizmo->getTransform().Translate(curPos);
			}
		}
	}
};
