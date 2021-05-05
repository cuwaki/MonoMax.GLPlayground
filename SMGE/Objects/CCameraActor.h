#pragma once

#include "CActor.h"
#include <array>

namespace SMGE
{
	class CCameraActor;

	struct SGRefl_CameraActor : public SGRefl_Actor
	{
		using Super = SGRefl_Actor;

		SGRefl_CameraActor(CCameraActor& actor);
		SGRefl_CameraActor(const std::shared_ptr<CCameraActor>& actorPtr);

		virtual const SGReflection& operator>>(CWString& out) const override;
		virtual SGReflection& operator<<(const CVector<TupleVarName_VarType_Value>& in) override;

		CString& cameraName_;
		float& fovDegrees_;
		float& zNear_, & zFar_;
	};

	class CCameraActor : public CActor
	{
		DECLARE_RTTI_CObject(CCameraActor)

	public:
		using This = CCameraActor;
		using Super = CActor;
		using TReflectionStruct = SGRefl_CameraActor;

		friend struct TReflectionStruct;

	public:
		CCameraActor(CObject* outer);
		void Ctor();

		virtual void Tick(float) override;
		virtual void BeginPlay() override;

		bool isCurrentCamera() const;
		void onChangedCurrent(bool isCurrent);

		SAABB GetFrustumAABB() const;
		bool IsInOrIntersectWithFrustum(CBoundComponent* mainBound) const;

	public:
		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		std::unique_ptr<TReflectionStruct> reflCameraActor_;

		CString cameraName_;
		float fovDegrees_;
		float zNear_, zFar_;

		class CCubeComponent* frustumAABBCube_;
		class CPlaneComponent* frustumNearPlane_;
		class CPlaneComponent* frustumFarPlane_;
		class CPlaneComponent* frustumUpPlane_;
		class CPlaneComponent* frustumBottomPlane_;
		class CPlaneComponent* frustumLeftPlane_;
		class CPlaneComponent* frustumRightPlane_;

	private:
		bool isCurrent_ = false;
	};
}
