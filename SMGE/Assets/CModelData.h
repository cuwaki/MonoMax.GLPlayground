#pragma once

#include "../Objects/CObject.h"
#include "CAsset.h"
#include "../Interfaces/CInt_Reflection.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	class CModelData;

	struct SGRefl_ModelData : public SGReflection
	{
		using Super = SGReflection;
		using ReflectionTarget = CModelData;

		SGRefl_ModelData(CModelData& md);

		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		CWString& vertShaderPath_;
		CWString& fragShaderPath_;
		int32& vertexAttribNumber_;
		CVector<float>& vertices_;
	};

	class CModelData : public nsRE::OldModelAsset, public CInt_Reflection
	{
	public:
		using TReflectionStruct = SGRefl_ModelData;
		friend struct TReflectionStruct;

		CModelData(void *outer);

	public:
		virtual CWString getClassName() override { return className_; }
		virtual SGReflection& getReflection() override;
		virtual void OnAfterDeserialized() override;

	protected:
		CWString className_;
		CUniqPtr<TReflectionStruct> reflData_;
	};
};
