#pragma once

#include "../Objects/CGObject.h"
#include "CGAsset.h"
#include "../Interfaces/CGInterf_Reflection.h"

namespace SMGE
{
	class CGModelData;

	struct SGRefl_ModelData : public SGReflection
	{
		using Super = SGReflection;
		using ReflectionTarget = CGModelData;

		SGRefl_ModelData(CGModelData& md);

		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		CWString& vertShaderPath_;
		CWString& fragShaderPath_;
		int32& vertexAttribNumber_;
		CVector<float>& vertices_;
	};

	class CGModelData : public CGObject, public CGInterf_Reflection
	{
	public:
		using TReflectionStruct = SGRefl_ModelData;
		friend struct TReflectionStruct;

		CGModelData();

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
