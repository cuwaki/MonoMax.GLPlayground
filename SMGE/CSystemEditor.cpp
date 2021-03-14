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
		TStackSetRenderModels::allocator_type::arena_type stackArean;
		TStackSetRenderModels renderModels(stackArean);
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
				auto currentPos = camTransform.GetWorldPosition();

				constexpr float moveSpeed = 20.0f / 1000.f;

				if (userInput.IsPressed('A'))
					currentPos += camTransform.GetWorldLeft() * deltaTime * moveSpeed;
				if (userInput.IsPressed('D'))
					currentPos -= camTransform.GetWorldLeft() * deltaTime * moveSpeed;
				if (userInput.IsPressed('W'))
					currentPos += camTransform.GetWorldFront() * deltaTime * moveSpeed;
				if (userInput.IsPressed('S'))
					currentPos -= camTransform.GetWorldFront() * deltaTime * moveSpeed;

				camTransform.Translate(currentPos);

				// 회전
				constexpr float angleSpeed = 3.f / 1000.f;

				static glm::vec2 RPressedPos;
				bool isJustRPress = userInput.IsJustPressed(VK_RBUTTON);
				if (isJustRPress)
					RPressedPos = userInput.GetMousePosition();

				bool isRPress = userInput.IsPressed(VK_RBUTTON);
				if (isJustRPress == false && isRPress == true)
				{
					auto movedPixel = RPressedPos - userInput.GetMousePosition();
					movedPixel.y *= -1.f;	// screen to gl

					// degrees 를 누적하지 않았을 때의 코드
					auto yawDegrees = movedPixel.x * angleSpeed;
					auto pitchDegrees = movedPixel.y * angleSpeed;
					auto rotYaw = glm::rotate(SMGE::nsRE::TransformConst::Mat4_Identity, yawDegrees, camTransform.GetWorldUp());
					auto rotPit = glm::rotate(SMGE::nsRE::TransformConst::Mat4_Identity, pitchDegrees, camTransform.GetWorldLeft());
					auto newDir = rotYaw * rotPit * glm::vec4(camTransform.GetWorldFront(), 0.f);

					// degrees 를 누적할 때의 코드 - 처음 회전이 확 튀고 그 뒤로는 잘 작동하는데 여전히 조금씩 휘어진다
					//static float yawDegrees = 0.f, pitchDegrees = 0.f;
					//yawDegrees += movedPixel.x * angleSpeed;
					//pitchDegrees += movedPixel.y * angleSpeed;
					//auto rotYaw = glm::rotate(SMGE::nsRE::TransformConst::Mat4_Identity, yawDegrees, { 0.f, 1.f, 0.f });
					//auto rotPit = glm::rotate(SMGE::nsRE::TransformConst::Mat4_Identity, pitchDegrees, { 1.f, 0.f, 0.f });
					//auto newDir = rotYaw * rotPit * glm::vec4(0.f, 0.f, 1.f, 0.f);

					camTransform.RotateQuat(newDir);

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
						/* 포인트 표시
						CCollideActor* pointActor = &StartSpawnActorVARIETY<CCollideActor>(currentMap, true, currentMap);
						auto prefab = CAssetManager::LoadAssetDefault<CActor>(Globals::GetGameAssetPath(wtext("/actor/prefabPointActor.asset")));
						pointActor->CopyFromTemplate(prefab->getContentClass());
						{
							// 얘는 단독 액터니까 이렇게 직접 트랜스폼 해줘야한다
							pointActor->getTransform().Translate(COLL_SEG.end_);
						}
						FinishSpawnActor(currentMap, *pointActor);
						pointActor->SetLifeTickCount(100);
						*/

						AddSelectedActor(currentMap, TAR);
					});

				auto prefab = CAssetManager::LoadAssetDefault<CActor>(Globals::GetGameAssetPath(wtext("/actor/prefabRayActor.asset")));
				rayActor->CopyFromTemplate(prefab->getContentClass());
				{	// 얘는 단독 액터니까 이렇게 직접 트랜스폼 해줘야한다
					// rayCompo 를 조작하는 게 아니고 rayActor 를 조작하고 있음에 주의!

					float rayLength = renderingEngine_->GetRenderingCamera().GetZFar();
					//rayCompo->SetBoundData(rayLength, ray_direction);
					rayActor->getTransform().Translate(ray_origin);
					rayActor->getTransform().Scale({ Configs::BoundEpsilon, Configs::BoundEpsilon, Configs::BoundEpsilon });	// 여기 - GetOBB 를 위하여 약간의 두께를 갖게 했다, 이거 생각해봐야한다, 레이의 입장에서는 xy 크기는 0인게 맞지만 obb 로 역할하려면 BoundEpsilon 만큼은 있어야하므로...
					rayActor->getTransform().Scale(nsRE::TransformConst::DefaultAxis_Front, rayLength);
					rayActor->getTransform().RotateQuat(ray_direction);
				}
				FinishSpawnActor(currentMap, rayActor);

				auto targets = rayActor->QueryCollideCheckTargets();
				bool hasCollide = rayActor->CheckCollideAndProcess(targets);
				rayActor->SetLifeTickCount(33);

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
			gizmo->getTransform().Translate(selActor->getLocation());
			gizmo->getTransform().RotateEuler(selActor->getRotationEulerDegrees());	// 여기 - 쿼터니언회전까지 반영된 것을 적용해야한다
			gizmo->getTransform().Scale(selActor->getScales());
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
