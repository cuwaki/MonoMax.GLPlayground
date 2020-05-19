#include "CGmap.h"
#include "../Assets/CGAssetManager.h"

#include <optional>

namespace MonoMaxGraphics
{
	template<typename T, typename U>
	CVector<T>& clearAndAssign(CVector<T>& left, const CVector<U>& right)
	{
		left.clear();
		left.reserve(right.size());

		for (int i = 0; i < right.size(); ++i)
		{
			left.emplace_back(right[i]);
		}

		return left;
	};

	SGRefl_Map::SGRefl_Map(CGMap& map) :
		SGReflection(map)
	{
		outerMap_ = &map;
		linkActorReferences();
	}

	void SGRefl_Map::linkActorReferences()
	{
		actorReflLayers_.resize(outerMap_->actorLayers_.size());

		clearAndAssign(actorReflLayers_[EActorLayer::System], outerMap_->actorLayers_[EActorLayer::System]);
		clearAndAssign(actorReflLayers_[EActorLayer::Game], outerMap_->actorLayers_[EActorLayer::Game]);
	}


	// �ϴ� �� ����
	// ���� �����̳ʸ� �ٷ� �� �ְ� �ؾ��Ѵ� L"CVector<CVector<SGRefl_Actor>>";
	// template ���� ���� runtime �� ���� ����... dimension �� �̿��Ͽ� runtime �� �ϴ� �� ���� �� ����
	// �׸��� �װ� ������ ��ũ�η� ��������Ѵ� ret += _TO_REFL_CONTAINER(CVector<SGRefl_Actor>, actorLayers_[1]);
	// �翬�� ���� �����̳� �д� �κе� �����Ǿ���Ѵ�
	template<typename CT, typename KT>
	CWString ToCWString(const CT& cont, const CWString& contTypeName, const CWString& contVarName, std::optional<KT> contKeyForParent)
	{
		CWString ret;
		ret += contVarName;
		if (contKeyForParent.has_value())
			ret += L"[" + ToTCHAR(*contKeyForParent) + L"]";
		ret += SGReflection::META_DELIM;
		
		ret += contTypeName;
		ret += SGReflection::META_DELIM;

		ret += ToTCHAR(cont.size());
		ret += SGReflection::VARIABLE_DELIM;

		size_t dimension = std::count(contTypeName.begin(), contTypeName.end(), '<');

		if (dimension > 1)
		{
			// ���� �����̳�
			//for (const auto& child : cont)
			//{
			//	CWString childContTypeName = L"CVector<SGRefl_Actor>";	// �ڵ����� ��������
			//	CWString childContVarName = L"[i]";	// �ڵ����� ��������, �� ���� ��쿡�� ���� ó������߰ڴ�

			//	// ���⼭ �̷��� �ڽĵ鿡 ���ؼ� �� ���������Ѵ�
			//	//ret += ToCWString(child, childContTypeName, childContVarName, 0);
			//}
		}
		else
		{	// �������̴�
			for (const auto& it : cont)
			{
				ret += SCast<CWString>(it);
			}
		}

		return ret;
	}

	SGRefl_Map::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += ToCWString(actorReflLayers_[0], L"CVector<SGRefl_Actor>", L"actorLayers_[0]", std::optional<size_t>{});
		ret += ToCWString(actorReflLayers_[1], L"CVector<SGRefl_Actor>", L"actorLayers_[1]", std::optional<size_t>{});

		return ret;
	}


	template<typename CT, typename VS, typename FUNC>
	void FromCWString(CT& cont, VS& variableSplitted, FUNC& func)
	{
		size_t contSize = 0;
		CWString& size_str = std::get<Tuple_Value>(*variableSplitted.cursor());
		ReflectionUtils::FromREFL(contSize, size_str);

		variableSplitted.cursorNext();	// [2] = (L"actorLayers_[1]", L"CVector<SGRefl_Actor>", L"2") �� �Ծ�ġ���

		if (contSize == 0)
			return;

		cont.clear();
		cont.reserve(contSize);

		for (size_t i = 0; i < contSize; i++)
		{
			func(variableSplitted, i);
		}
	}

	SGReflection& SGRefl_Map::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		//// ���� �����̳� ���� �� 
		//// actorLayers_.resize
		//	// actorLayers_[0
		//		// actorLayers_[0.resize
		//			// CGActors
		//	// actorLayers_[1
		//		// actorLayers_[0.resize
		//			// CGActors
		//
		////_FROM_REFL(actorKey_, variableSplitted);
		actorReflLayers_.resize(2);	// �׽�Ʈ �ڵ�

		auto FuncSpawnActor = [this](auto& variableSplitted, size_t childKey)
		{
			// ���⼭ SGRefl_Actor �� �����Ƿ��� CGActor & �� �ʿ��ϴ�

			// �׷��Ƿ� SGRefl_Map �� = �� ����Ƿ��� ����
			// CGMap �� CGActor ���� �ν��Ͻ� �Ǿ��־���Ѵ�
			// SGRefl_Map �� CGMap �� ���� ������ ��Ų �� �����ؾ��Ѵ� - �ϴ��� �̷��� �����غ���!
			CGActor loader;

			// ��Ȯ�� ������ Ŭ�������� ��´�
			auto backupCursor = variableSplitted.cursor();
			loader.getReflection() = variableSplitted;

			CWString assetRoot = wtext("e:/dev_project/Assets/");
			CWString actorAssetPath = CGAssetManager::FindAssetFilePathByClassName(loader.getClassName());

			// 1. �ּ��� �̿��Ͽ� �ʿ� ���� �����ϱ�
			CSharPtr<CGAsset<CGActor>>& actorTemplate = CGAssetManager::LoadAsset<CGActor>(assetRoot + actorAssetPath);

			// ���� ������ ������ ���÷��� �ܰ迡�� �Ͼ�� �ȴ�... ������ �� �ƽ��� �κ��̴�!
			// �̷� �� ������ �𸮾��� ������ Ư���� ����� �� �ִٴ°� �ƴѰ� ����??

			CGActor& actorA = outerMap_->SpawnDefaultActor(*actorTemplate->getContentClass(), false);	// ��ġ

			// 2 �ܰ� - �ʿ� ����� ������ �������̵��Ų��
			variableSplitted.setCursor(backupCursor);
			actorA.getReflection() = variableSplitted;

			// ���⼱ ���� this->actorLayers_ ���� ����� �ȵǾ���, �� �ؿ� 3�ܰ迡�� ó���Ѵ�
		};

		FromCWString(actorReflLayers_[0], variableSplitted, FuncSpawnActor);
		FromCWString(actorReflLayers_[1], variableSplitted, FuncSpawnActor);

		// 3�ܰ� - �ʰ� ���� ���۷����� �����Ѵ�
		linkActorReferences();

		return *this;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CGMap::CGMap() : CGObject()
	{
		className_ = wtext("SMGE_Game::CGMap");
		CGCtor();
	}

	CGMap::CGMap(const CGMap& templateInst) : CGMap()
	{
		CopyFromTemplate(templateInst);
	}

	void CGMap::CGCtor()
	{
		Super::CGCtor();

		actorLayers_.resize(etoi(EActorLayer::Max));
		actorLayers_[EActorLayer::System].reserve(20);
		actorLayers_[EActorLayer::Game].reserve(100);
	}

	SGReflection& CGMap::getReflection()
	{
		if (reflMap_.get() == nullptr)
		{
			reflMap_ = MakeUniqPtr<TReflectionStruct>(*this);
		}
		else
		{	// ���� this->actorLayers_ �� ������ �ִ� ��쿡�� �ٲ����Ѵ�
			reflMap_->SGRefl_Map::SGRefl_Map(*this);
		}

		return *reflMap_.get();
	}

	CGActor& CGMap::SpawnDefaultActor(const CGActor& templateActor, bool isDynamic)
	{
		auto newActor = MakeSharPtr<CGActor>(templateActor);

		if (isDynamic == true)
		{	// ActorKeyGenerator
			static TActorKey ActorKeyGenerator = 0;
			newActor->actorKey_ = ++ActorKeyGenerator;
		}

		auto rb = actorLayers_[EActorLayer::Game].emplace_back(std::move(newActor));
		return *rb;
	}

	// �ϴ� �Ⱦ�
	//CGActor& CGMap::OverrideActor(CGActor& arrangedActor)
	//{
	//	// �ʿ� ��ġ�� ���Ϳ� �����Ϳ��� �����ߴ� ���� �����Ѵ�
	//	TReflectionStruct& mapReflStruct = SCast<TReflectionStruct&>(this->getReflection());

	//	const auto& cont = mapReflStruct.actorReflLayers_[EActorLayer::Game];
	//	auto act = GlobalUtils::FindIt(cont, arrangedActor.actorKey_);
	//	if (GlobalUtils::IsFound(cont, act))
	//	{
	//		SGStringStreamIn strIn(*act);
	//		strIn >> arrangedActor.getReflection();
	//	}
	//	return arrangedActor;
	//}

	void CGMap::Activate()
	{
		// ���͵� ����/�������̵� ���� ������ ���� ���� �۵���Ų��
	}
};
