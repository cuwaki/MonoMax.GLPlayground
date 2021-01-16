#include "CMap.h"
#include "../GECommonIncludes.h"
#include "../Assets/CAssetManager.h"
#include "../CEngineBase.h"
#include "../CGameBase.h"
#include "CCameraActor.h"

#include <windows.h>	// 테스트 코드 - for VK_LEFT ...

namespace SMGE
{
	namespace Globals
	{
		CMap* GCurrentMap;
	}

	SGRefl_Map::SGRefl_Map(CMap& map) :
		outerMap_(map),
		SGReflection(map)
	{
		linkINST2REFL();
	}

	void SGRefl_Map::linkINST2REFL()
	{
		actorLayersREFL_.resize(outerMap_.actorLayers_.size());

		ReflectionUtils::clearAndEmplaceBackINST2REFL(actorLayersREFL_[EActorLayer::System], outerMap_.actorLayers_[EActorLayer::System]);
		ReflectionUtils::clearAndEmplaceBackINST2REFL(actorLayersREFL_[EActorLayer::Game], outerMap_.actorLayers_[EActorLayer::Game]);
	}

	SGRefl_Map::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += ReflectionUtils::ToCVector(actorLayersREFL_[0], L"CVector<SGReflection&>", L"actorLayersREFL_[0]", std::optional<size_t>{});
		ret += ReflectionUtils::ToCVector(actorLayersREFL_[1], L"CVector<SGReflection&>", L"actorLayersREFL_[1]", std::optional<size_t>{});

		return ret;
	}

	SGReflection& SGRefl_Map::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		actorLayersREFL_.resize(etoi(EActorLayer::Max));

		// 이거긴 한데 이렇게 쓰면 auto 를 못쓴다...
		//using REFL_CVECTOR_FUNC = void(CVector<TupleVarName_VarType_Value>& variableSplitted, size_t childKey);
		auto FuncSpawnActor = [this](auto& variableSplitted, size_t childKey, EActorLayer layer)
		{
			// 여기서 SGRefl_Actor 가 생성되려면 CActor & 가 필요하다

			// 그러므로 SGRefl_Map 의 = 가 실행되려면 먼저
			// CMap 에 CActor 들이 인스턴싱 되어있어야한다
			// SGRefl_Map 이 CMap 에 액터 생성을 시킨 후 연결해야한다 - 일단은 이렇게 구현해보자!

			CString actorClassRTTIName = ReflectionUtils::GetClassRTTIName(variableSplitted);
			CWString actorAssetPath = ReflectionUtils::GetReflectionFilePath(variableSplitted);

			if (Path::IsValidPath(actorAssetPath) == true)
			{
				// 실제 액터의 스폰이 리플렉션 단계에서 일어나게 된다... 구조상 좀 아쉬운 부분이다!
				// 이런 것 때문에 언리얼의 레벨도 특수한 방법이 들어가 있다는게 아닌가 싶은??

				// 1. 애셋을 이용하여 맵에 액터 스폰하기 - RTTI
				//CSharPtr<CAsset<CActor>>& actorTemplate = CAssetManager::LoadAsset<CActor>(Globals::GetGameAssetPath(actorAssetPath));
				//CActor& actorA = outerMap_.SpawnDefaultActor(actorClassRTTIName, false, &outerMap_, actorTemplate->getContentClass());

				// 1. 디폴트로 생성하고
				CActor& actorA = outerMap_.SpawnActorDEFAULT(layer, actorClassRTTIName, false, &outerMap_);

				// 2. 애셋 덮어씌우고
				auto actorTemplate = CAssetManager::LoadAsset<CActor>(Globals::GetGameAssetPath(actorAssetPath));
				actorA.CopyFromTemplate(actorTemplate->getContentClass());

				// 3. 맵에 저장된 값으로 한번 더 덮어쓴다 - 위치나 맵에서 박은 콤포넌트 등등
				actorA.getReflection() = variableSplitted;

				outerMap_.FinishSpawnActor(actorA);

				// 여기선 아직 this->actorLayers_ 에는 등록이 안되었다, 저 밑에 3단계에서 처리한다
			}
			else
			{
				throw SMGEException(wtext("CMap : FuncSpawnActor : bad filepath - ") + actorAssetPath);
			}
		};

		ReflectionUtils::FromCVector(actorLayersREFL_[EActorLayer::System], variableSplitted, FuncSpawnActor, EActorLayer::System);
		ReflectionUtils::FromCVector(actorLayersREFL_[EActorLayer::Game], variableSplitted, FuncSpawnActor, EActorLayer::Game);

		// 3단계 - 맵과 나를 레퍼런스로 연결한다
		linkINST2REFL();

		return *this;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CMap::CMap(CObject* outer) : 
		actorLayers_(etoi(EActorLayer::Max)),	// #cvcvuptr_resize - unique_ptr 을 CVector<CVector< 의 T 로 넣을 경우 CVector< 의 resize 가 컴파일 오류가 나서 일단 이렇게 한다
		CObject(outer)
	{
		Ctor();
	}

	void CMap::Ctor()
	{
		Super::Ctor();

		actorLayers_[EActorLayer::System].reserve(20);
		actorLayers_[EActorLayer::Game].reserve(100);
	}

	void CMap::ProcessPendingKills()
	{
		auto& actors = actorLayers_[EActorLayer::Game];
		if (actors.size() == 0)
			return;
		
		for (size_t i = actors.size() - 1; i > 0;)
		{
			auto& actor = actors[i];
			if (actor->IsPendingKill())
			{
				actorOctree_.RemoveByPoint(actor.get(), actor.get()->getLocation());
				actor->EndPlay();

				auto it = std::find(oldActorsInFrustum_.begin(), oldActorsInFrustum_.end(), actor.get());
				if(it != oldActorsInFrustum_.end())
					*it = nullptr;

				actor.reset();
				actors.erase(actors.begin() + i);
			}

			if (i == 0)
				break;
			else
				--i;
		}
	}

	void CMap::Tick(float timeDelta)
	{
		cameraFrustumCulling();

		glm::vec3 oldLoc, newLoc;
		for (auto& actors : actorLayers_)
		{
			for (auto& actor : actors)
			{
				oldLoc = actor->getLocation();

				actor->Tick(timeDelta);

				newLoc = actor->getLocation();
				if (oldLoc != newLoc)	// 여기 - 위치만 가지고 하면 안된다, 차후에 dirty 를 이용해서 업데이트 하도록 하자 / 옥트리 업데이트 말고도 aabb 업데이트도 해야한다
				{	// 여기 - 일단 무식하게 한다
					actorOctree_.RemoveByPoint(actor.get(), oldLoc);
					actorOctree_.AddByPoint(actor.get(), newLoc);

					// 역시 일단 무식하게 한다
					if (actor->GetMainBound() != nullptr)
					{
						actor->GetMainBound()->CacheAABB();
					}
				}
			}
		}
	}

	SGReflection& CMap::getReflection()
	{
		if (reflMap_.get() == nullptr)
		{
			reflMap_ = MakeUniqPtr<TReflectionStruct>(*this);
		}
		else
		{	// 차후 this->actorLayers_ 에 변경이 있는 경우에만 바뀌어야한다
			//reflMap_->SGRefl_Map::SGRefl_Map(*this);	// 201003 이거 왜 이렇게 했더라? 구조적으로 이렇게 하면 안되게 되어서 주석으로 막음
		}

		return *reflMap_.get();
	}

	TActorKey CMap::DynamicActorKey = 3332;	// 디버깅할 때 티나라고 이상한 값에서 시작하게 해둠

	//CActor& CMap::SpawnDefaultActor(const CActor& templateActor, bool isDynamic)
	//{
	//	auto newActor = MakeSharPtr<CActor>(this, templateActor);

	//	if (isDynamic == true)
	//	{	// DynamicActorKey
	//		newActor->actorKey_ = DynamicActorKey++;
	//	}

	//	auto rb = actorLayers_[EActorLayer::Game].emplace_back(std::move(newActor));

	//	rb->OnSpawnStarted(this, isDynamic);
	//	return *rb;
	//}

	CActor& CMap::FinishSpawnActor(CActor& targetActor)
	{
		targetActor.OnSpawnFinished(this);

		if (isBeganPlay_ == true)
		{
			actorOctree_.AddByPoint(&targetActor, targetActor.getLocation());
			targetActor.BeginPlay();
		}

		return targetActor;
	}

	CActor* CMap::FindActor(TActorKey ak)
	{
		return nullptr;
	}

	CUniqPtr<CActor>&& CMap::RemoveActor(TActorKey ak)
	{
		assert(false && "구현하라");
		return MakeUniqPtr<CActor>(nullptr);
		//return std::move(actorLayers_[EActorLayer::Game][0]);
	}

	const CVector<CUniqPtr<CActor>>& CMap::GetActors(EActorLayer layer) const
	{
		return actorLayers_[layer];
	}

	CVector<CActor*> CMap::QueryActors(const SAABB& aabb) const
	{
#undef min	// 테스트 코드 - for VK_LEFT ...
#undef max	// 테스트 코드 - for VK_LEFT ...

		return actorOctree_.QueryValuesByCube(aabb.min(), aabb.max());
	}

	void CMap::changeCurrentCamera(CCameraActor* camA)
	{
		if (currentCamera_ == camA)
			return;

		if (currentCamera_)
			currentCamera_->onChangedCurrent(false);

		currentCamera_ = camA;
		cullingCamera_ = camA;
 
		if(currentCamera_)
			currentCamera_->onChangedCurrent(true);
	}

	void CMap::OnPostBeginPlay()
	{
		currentCamera_ = nullptr;

		// 현재 카메라 설정
		for (auto& sysActor : actorLayers_[EActorLayer::System])
		{
			auto camActor = DCast<CCameraActor*>(sysActor.get());
			if (camActor)
			{
				changeCurrentCamera(camActor);

#if IS_EDITOR
				auto InputForCameraActor = [this](const nsGE::CUserInput& userInput)
				{
					if (currentCamera_)
					{
						constexpr float deltaTime = 1000.f / 60.f;	// 테스트 코드 - 이거 하드코딩이고 다른 데 또 있으니 검색해라

						auto& camTransform = currentCamera_->getTransform();
						
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
				GetEngine()->AddProcessUserInputs(InputForCameraActor);
#endif
				break;
			}
		}

		// 테스트 코드 - 프러스텀 컬링
		for (auto& sysActor : actorLayers_[EActorLayer::System])
		{
			auto camActor = DCast<CCameraActor*>(sysActor.get());
			if (camActor->getActorStaticTag() == "testCamera")
				cullingCamera_ = camActor;
		}

		//isFrustumCulling_ = false;	// 테스트 코드 - 프러스텀 컬링

		if (isFrustumCulling_)
		{	// 카메라 설정이 끝났으니 초기 프러스텀 컬링
			for (auto& gamActor : actorLayers_[EActorLayer::Game])	// 모두 끄고 시작
				gamActor->SetRendering(false);

			cameraFrustumCulling();
		}
	}

	void CMap::cameraFrustumCulling()
	{
		if (isFrustumCulling_ == false || cullingCamera_ == nullptr)
			return;

		for (auto actor : oldActorsInFrustum_)
		{
			if (actor)
				actor->SetRendering(false);
		}
		
		oldActorsInFrustum_ = QueryActors(cullingCamera_->GetFrustumAABB());
		for (auto& actorPtr : oldActorsInFrustum_)
		{	// 여기 - 메인 바운드 말고 비기스트 바운드 등을 활용하거나 아예 컬링용 바운드를 따로 해야할 것 같으며, 바운드만 있는 기즈모들의 경우에는 자기 자신을 내보내든가 해야할 듯
			const auto mainBound = actorPtr->GetMainBound();
			if (mainBound == nullptr)
			{
				actorPtr->SetRendering(true);	// 컬링이 불가능하므로 무조건 그린다
				actorPtr = nullptr;
			}
			else
				actorPtr->SetRendering(cullingCamera_->IsInOrIntersectWithFrustum(mainBound));
		}
	}

	void CMap::BeginPlay()
	{
		if (isBeganPlay_ == true)
			throw SMGEException(wtext("CMap already activated"));

		isBeginningPlay_ = true;
		{
			actorOctree_.Create("actorOctree_", MapConst::MaxX, MapConst::MaxY, MapConst::MaxZ, MapConst::OctreeLeafWidth);

			for (auto& actors : actorLayers_)
			{
				for (auto& actor : actors)
				{
					actorOctree_.AddByPoint(actor.get(), actor->getLocation());
					actor->BeginPlay();
				}
			}
		}
		isBeginningPlay_ = false;

		OnPostBeginPlay();

		isBeganPlay_ = true;
	}

	void CMap::FinishPlaying()
	{
		if (isBeganPlay_ == false)
			return;

		for (auto& actors : actorLayers_)
		{
			for (auto& actor : actors)
			{
				actorOctree_.RemoveByPoint(actor.get(), actor->getLocation());
				actor->EndPlay();
			}
		}

		actorLayers_.clear();
		isBeganPlay_ = false;
	}

	class CCameraActor* CMap::GetCurrentCamera() const
	{
		return currentCamera_;
	}

	CActor& CMap::SpawnActorINTERNAL(EActorLayer layer, CObject* newObj, bool isDynamic)
	{
		CUniqPtr<CActor> newActor(DCast<CActor*>(newObj));

		if (isDynamic == true)
		{	// DynamicActorKey
			newActor->actorKey_ = DynamicActorKey++;
		}

		auto& rb = actorLayers_[layer].emplace_back(std::move(newActor));
		rb->OnSpawnStarted(this, isDynamic);

		return static_cast<CActor&>(*rb.get());
	}
};
