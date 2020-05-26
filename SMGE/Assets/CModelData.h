#pragma once

#include "../Objects/CObject.h"
#include "CAsset.h"
#include "../Interfaces/CInt_Reflection.h"

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

	class CModelData : public CObject, public CInt_Reflection
	{
	public:
		using TReflectionStruct = SGRefl_ModelData;
		friend struct TReflectionStruct;

		CModelData();

	public:
		virtual SGReflection& getReflection() override;

		CWString vertShaderPath_;
		CWString fragShaderPath_;
		int32 vertexAttribNumber_;
		CVector<float> vertices_;

	protected:
		CUniqPtr<TReflectionStruct> reflData_;
	};
};
