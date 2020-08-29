#pragma once

#include "../Objects/CObject.h"
#include "CAsset.h"
#include "../Interfaces/CInt_Reflection.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	class CResourceModel;

	struct SGRefl_ResourceModel : public SGReflection
	{
		using Super = SGReflection;
		using ReflectionTarget = CResourceModel;

		SGRefl_ResourceModel(CResourceModel& md);

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

	class CResourceModel : virtual public nsRE::ResourceModel, public CInt_Reflection
	{
	public:
		using TReflectionStruct = SGRefl_ResourceModel;
		friend struct TReflectionStruct;

		CResourceModel(void *outer);

	public:
		virtual const CWString& getClassName() override { return className_; }
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
