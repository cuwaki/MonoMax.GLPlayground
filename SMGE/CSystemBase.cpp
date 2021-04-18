#include "CSystemBase.h"
#include "CEngineBase.h"
#include "../MonoMax.EngineCore/RenderingEngine.h"
#include "Objects/CMap.h"

namespace SMGE
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CRenderPassWorld::CRenderPassWorld(CSystemBase* system) : CRenderPassWithSystem(system)
	{
	}

	void CRenderPassWorld::RenderTo(const glm::mat4& V, const glm::mat4& VP, nsRE::CRenderTarget*& writeRT, nsRE::CRenderTarget*& readRT)
	{
		// 여기 - 라이트 액터 구현 필요함
		auto lightPos = glm::vec3(0);

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
				actor->AmIEditorActor() == false)
			{
				for (auto comp : actor->getAllComponents())
				{
					auto drawComp = dynamic_cast<CDrawComponent*>(comp);	// 최적화 - isdrawable 같은 함수 만들어서 대체하자
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

			// 여기 - 일단 가장 첫번째 패스이기 때문에 지워야한다
			writeRT->SetClearColor(nsRE::ColorConst::Gray);	// 테스트 코드 ㅡ 빨강으로 칠해서 확인하기 위함
			writeRT->ClearFrameBuffer();

			// CRenderingPass 와의 엮인 처리로 좀 낭비가 있다 - ##renderingpasswith03
			for (auto rm : renderModels)
			{
				rm->UseShader(V, lightPos);	// 셰이더 마다 1회
				rm->BeginRender();
				rm->Render(VP);
				rm->EndRender();
			}
			writeRT->UnbindFrameBuffer();
		}	
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TActorKey CSystemBase::DynamicActorKey = 33332;	// 디버깅할 때 티나라고 이상한 값에서 시작하게 해둠

	//// deprecated 201227 - 카메라 액터 구현하면서 안쓰게 됨
	//auto EditorProcessCameraMove = [this](const CUserInput& userInput)
	//{
	//	auto& renderCam = GetRenderingEngine()->GetRenderingCamera();

	//	renderCam.MoveCamera(userInput.IsPressed(VK_LEFT), userInput.IsPressed(VK_RIGHT), userInput.IsPressed(VK_UP), userInput.IsPressed(VK_DOWN));

	//	static glm::vec2 RPressedPos;
	//	bool isJustRPress = userInput.IsJustPressed(VK_RBUTTON);
	//	if (isJustRPress)
	//		RPressedPos = userInput.GetMousePosition();

	//	bool isRPress = userInput.IsPressed(VK_RBUTTON);
	//	if (isJustRPress == false && isRPress == true)
	//	{
	//		auto moved = RPressedPos - userInput.GetMousePosition();
	//		renderCam.RotateCamera(moved);

	//		RPressedPos = userInput.GetMousePosition();
	//	}

	//	return false;
	//};
	//AddProcessUserInputs(EditorProcessCameraMove);

	CSystemBase::CSystemBase(class CEngineBase* engine) : engine_(engine)
	{
	}

	void CSystemBase::OnLinkWithRenderingEngine()
	{
		renderingEngine_ = engine_->GetRenderingEngine();
	}

	void CSystemBase::OnDestroyingGameEngine()
	{

	}

	// 여기 - Tick 들 불리우는 순서 등 명시적으로 하고, 코드 정리하자
	void CSystemBase::Tick(float timeDelta)
	{
		ProcessPendingKills();
		ProcessUserInput();
	}

	bool CSystemBase::ProcessUserInput()
	{
		auto& userInput = engine_->GetUserInput();

		userInput.QueryStateKeyOrButton();

		if (auto activeWindow = engine_->HasWindowFocus())
		{	// 포커스가 있을 때만
			userInput.QueryStateMousePos(activeWindow);

			for (auto& pui : delegateUserInputs_)
			{
				if (pui(userInput) == true)
					break;
			}
		}

		return true;
	}

	void CSystemBase::AddProcessUserInputs(const DELEGATE_ProcessUserInput& delegPUI)
	{
		delegateUserInputs_.push_back(delegPUI);
	}

	void CSystemBase::OnChangedSystemState(const CString& stateName)
	{

	}

	void CSystemBase::ProcessPendingKill(CActor* actor)
	{

	}

	void CSystemBase::ProcessPendingKills()
	{
		if (allActors_.size() == 0)
			return;

		for (size_t i = allActors_.size() - 1; i > 0;)
		{
			auto& actor = allActors_[i];
			if (actor->IsPendingKill())
			{
				auto itsMap = actorsMap_.find(actor.get());
				(*itsMap).second->ProcessPendingKill(actor.get());

				this->ProcessPendingKill(actor.get());

				allActors_.erase(allActors_.begin() + i);
				actorsMap_.erase(itsMap);
			}

			if (i == 0)
				break;
			else
				--i;
		}
	}

	CActor& CSystemBase::StartSpawnActorINTERNAL(CMap* targetMap, CObject* newObj, bool isDynamic)
	{
		assert(targetMap != nullptr && "never null");

		auto newActor = static_cast<CActor*>(newObj);

		if (targetMap->IsTemplate() == false)
		{	// 맵이 템플릿일 경우에는 등록처리를 하지 않는다, 진짜 사용되는 맵이 아니고 템플릿일 뿐이기 때문이다
			auto& rb = allActors_.emplace_back(std::move(newActor));
			actorsMap_.insert(std::make_pair(rb.get(), targetMap));
		}

		newActor->OnSpawnStarted(targetMap, isDynamic);

		return static_cast<CActor&>(*newActor);
	}

	CActor* CSystemBase::FinishSpawnActor(CMap* targetMap, CActor* targetActor)
	{
		assert(targetMap != nullptr && "never null");

		const auto isDynamic = targetActor->getActorKey() == InvalidActorKey;
		if (isDynamic == true)
		{	// DynamicActorKey
			targetActor->setActorKey(DynamicActorKey++);
		}

		targetActor->OnSpawnFinished(targetMap);

		targetMap->AddMapActor(targetActor);
		return targetActor;
	}

	//std::unique_ptr<CActor>&& CSystemBase::RemoveActor(TActorKey ak)
	//{
	//	assert(false && "구현하라");
	//	return std::make_unique<CActor>(nullptr);
	//	//return std::move(allActors_[EActorLayer::Game][0]);
	//}	
};
