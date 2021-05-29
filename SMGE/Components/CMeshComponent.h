#pragma once

#include "../GECommonIncludes.h"
#include "CDrawComponent.h"
#include "../Assets/CResourceModel.h"

namespace SMGE
{
	class CMeshComponent;

	struct SGRefl_MeshComponent : public SGRefl_DrawComponent
	{
		using Super = SGRefl_DrawComponent;
		using TReflectionClass = CMeshComponent;

		SGRefl_MeshComponent(TReflectionClass& meshc);

		virtual const SGReflection& operator>>(CWString& out) const override;
		virtual SGReflection& operator<<(const CVector<TupleVarName_VarType_Value>& in) override;
		
		CWString& resourceModelAssetPath_;
	};

	class CMeshComponent : public CDrawComponent
	{
		DECLARE_RTTI_CObject(CMeshComponent)

	public:
		using This = CMeshComponent;
		using Super = CDrawComponent;
		using TReflectionStruct = SGRefl_MeshComponent;

		friend struct TReflectionStruct;

	public:
		CMeshComponent(CObject* outer);
		CMeshComponent(CObject* outer, const CWString& modelAssetPath);

		void Ctor();

		virtual void ReadyToDrawing() override;

		void SetDrawingModelAsset(const CWString& modelAssetPath);

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		UPtr<TReflectionStruct> reflMeshCompo_;
		CWString resourceModelAssetPath_;

	protected:
		SPtr<CAsset<CResourceModel>> resourceModelAsset_;
	};
};
