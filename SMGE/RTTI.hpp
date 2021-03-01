#pragma once

#include <string>
#include <map>
#include <functional>
#include <any>

namespace SMGE
{
	// 가독성을 위하여 Args... 의 시작을 표시함
	#define Args_START

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct CRttiNewFunctorVarietyBase
	{
	};

	template<class BaseClass, class TargetClass, typename... Args>
	struct CRttiNewFunctorVariety : public CRttiNewFunctorVarietyBase
	{
		BaseClass* operator()(Args&&... args)
		{
			return new TargetClass(std::forward<Args>(args)...);
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// static 클래스용
	template<class T>
	const std::string& GetClassRTTIName()
	{
		return T(nullptr).GetClassRTTIName();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class BaseClass, typename NewFunctor>
	class CRtti
	{
		using NewFunctorDefaultT = std::map<std::string, std::function<NewFunctor>>;
		using NewFunctorVarietyT = std::map<std::string, CRttiNewFunctorVarietyBase>;

		static NewFunctorDefaultT NewClassDefaults_;
		static NewFunctorVarietyT NewClassVarieties_;

	public:
		CRtti(std::string&& rttiName, std::function<NewFunctor>&& func)	// 일부러 정적인 입력들만 받기위하여 오른값들만 받음 - 즉 동적으로 작성 불가!
		{
			NewClassDefaults_[rttiName] = std::move(func);
		}

		CRtti(std::string&& rttiName, CRttiNewFunctorVarietyBase&& func)
		{
			NewClassVarieties_[rttiName] = std::move(func);
		}

		template<typename... Args>
		static BaseClass* NewDefault(const std::string& classRTTIName, Args&&... args)
		{
			assert(NewClassDefaults_.find(classRTTIName) != NewClassDefaults_.end());
			return NewClassDefaults_[classRTTIName](std::forward<Args>(args)...);
		}

		template<class TargetClass, typename... Args>
		static TargetClass* NewDefault(Args&&... args)
		{
			const auto& classRTTIName = SMGE::GetClassRTTIName<TargetClass>();
			return static_cast<TargetClass *>(NewDefault(classRTTIName, std::forward<Args>(args)...));
		}

		template<class TargetClass, typename... Args>
		static TargetClass* NewVariety(Args&&... args)
		{
			const auto& classRTTIName = SMGE::GetClassRTTIName<TargetClass>();

			assert(NewClassVarieties_.find(classRTTIName) != NewClassVarieties_.end());

			using NewFunctorT = CRttiNewFunctorVariety<BaseClass, TargetClass, Args...>;
			
			auto newOne = static_cast<NewFunctorT&>(NewClassVarieties_[classRTTIName])(std::forward<Args>(args)...);
			return static_cast<TargetClass*>(newOne);
		}
	};
}
