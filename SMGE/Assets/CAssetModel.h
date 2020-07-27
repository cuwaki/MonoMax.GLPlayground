#pragma once

#include "../Objects/CObject.h"
#include "CAsset.h"
#include "../Interfaces/CInt_Reflection.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	class CAssetModel;

	struct SGRefl_AssetModel : public SGReflection
	{
		using Super = SGReflection;
		using ReflectionTarget = CAssetModel;

		SGRefl_AssetModel(CAssetModel& md);

		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		CWString& vertShaderPath_;
		CWString& fragShaderPath_;
		CWString& objFilePath_;
		CWString& textureFilePath_;

		CVector<glm::vec3>& vertices_;
		CVector<glm::vec2>& uvs_;
		CVector<glm::vec3>& normals_;
		CVector<glm::vec3>& vertexColors_;
	};

	class CAssetModel : public nsRE::AssetModel, public CInt_Reflection
	{
	public:
		using TReflectionStruct = SGRefl_AssetModel;
		friend struct TReflectionStruct;

		CAssetModel(void *outer);

	public:
		virtual CWString getClassName() override { return className_; }
		virtual SGReflection& getReflection() override;
		virtual void OnAfterDeserialized() override;

	protected:
		CWString className_;
		CUniqPtr<TReflectionStruct> reflData_;

	public:
		CWString vertShaderPath_;
		CWString fragShaderPath_;
		CWString objFilePath_;
		CWString textureFilePath_;

		CVector<glm::vec3> vertices_;
		CVector<glm::vec2> uvs_;
		CVector<glm::vec3> normals_;
		CVector<glm::vec3> vertexColors_;
	};
};
