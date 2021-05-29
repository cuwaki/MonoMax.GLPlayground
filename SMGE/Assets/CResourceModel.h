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

		virtual const SGReflection& operator>>(CWString& out) const override;
		virtual SGReflection& operator<<(const CVector<TupleVarName_VarType_Value>& in) override;

		CWString& vertShaderPath_;
		CWString& fragShaderPath_;
		CWString& objFilePath_;
		CWString& textureFilePath_;

		CVector<glm::vec3>& vertices_;
		CVector<glm::vec2>& uvs_;
		CVector<glm::vec3>& normals_;
		CVector<glm::vec3>& vertexColors_;
	};

	class CResourceModel : public CObject, public CInt_Reflection, public nsRE::ResourceModel
	{
		DECLARE_RTTI_CObject(CResourceModel)

	public:
		using This = CResourceModel;
		using TReflectionStruct = SGRefl_ResourceModel;
		friend struct TReflectionStruct;

		CResourceModel(CObject* outer);

	public:
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

		//virtual void OnAfterDeserialized() override;	// 지금은 뭔가 이상한 vftbl 관련 버그가 있어서 못쓰고 있다
		void OnAfterDeserialized2();	// 테스트 코드 - vftbl 버그 때문에

	protected:
		UPtr<TReflectionStruct> reflData_;

	public:
		//CWString vertShaderPath_;
		//CWString fragShaderPath_;
		CWString objFilePath_;
		CWString textureFilePath_;

		CVector<glm::vec3> vertices_;
		CVector<glm::vec2> uvs_;
		CVector<glm::vec3> normals_;
		CVector<glm::vec3> vertexColors_;
	};
};
