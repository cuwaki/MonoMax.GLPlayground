#pragma once

#include "CActor.h"

namespace SMGE
{
	class CCameraActor;

	struct SGRefl_CameraActor : public SGRefl_Actor
	{
		using Super = SGRefl_Actor;

		SGRefl_CameraActor(CCameraActor& actor);
		SGRefl_CameraActor(const CSharPtr<CCameraActor>& actorPtr);

		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		CString& cameraName_;
		float& fov_;
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

		virtual void BeginPlay() override;

	public:
		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		CUniqPtr<TReflectionStruct> reflCameraActor_;

		CString cameraName_;
		float fov_;
		float zNear_, zFar_;
	};
}
