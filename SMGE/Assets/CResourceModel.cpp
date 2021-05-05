#include "CResourceModel.h"
#include "../CGameBase.h"

namespace SMGE
{
	SGRefl_ResourceModel::SGRefl_ResourceModel(CResourceModel& md) : SGReflection(&md),
		vertShaderPath_(md.vertShaderPath_),
		fragShaderPath_(md.fragShaderPath_),
		objFilePath_(md.objFilePath_),
		textureFilePath_(md.textureFilePath_),
		vertices_(md.vertices_),
		uvs_(md.uvs_),
		normals_(md.normals_),
		vertexColors_(md.vertexColors_)
	{
	}

	const SGReflection& SGRefl_ResourceModel::operator>>(CWString& out) const
	{
		Super::operator>>(out);

		out += _TO_REFL(CWString, vertShaderPath_);
		out += _TO_REFL(CWString, fragShaderPath_);
		out += _TO_REFL(CWString, objFilePath_);
		out += _TO_REFL(CWString, textureFilePath_);

		out += ReflectionUtils::ToCVector(vertices_, L"CVector<glm::vec3>", L"vertices_", std::optional<size_t>{});
		out += ReflectionUtils::ToCVector(uvs_, L"CVector<glm::vec2>", L"uvs_", std::optional<size_t>{});
		out += ReflectionUtils::ToCVector(normals_, L"CVector<glm::vec3>", L"normals_", std::optional<size_t>{});
		out += ReflectionUtils::ToCVector(vertexColors_, L"CVector<glm::vec3>", L"vertexColors_", std::optional<size_t>{});

		return *this;
	}

	SGReflection& SGRefl_ResourceModel::operator<<(const CVector<TupleVarName_VarType_Value>& in)
	{
		Super::operator<<(in);

		_FROM_REFL(vertShaderPath_, in);
		_FROM_REFL(fragShaderPath_, in);
		_FROM_REFL(objFilePath_, in);
		_FROM_REFL(textureFilePath_, in);

		vertices_.clear();
		ReflectionUtils::FromCVector(vertices_, in, ReflectionUtils::FromRefl_PushBack(this->vertices_));
		uvs_.clear();
		ReflectionUtils::FromCVector(uvs_, in, ReflectionUtils::FromRefl_PushBack(this->uvs_));
		normals_.clear();
		ReflectionUtils::FromCVector(normals_, in, ReflectionUtils::FromRefl_PushBack(this->normals_));
		vertexColors_.clear();
		ReflectionUtils::FromCVector(vertexColors_, in, ReflectionUtils::FromRefl_PushBack(this->vertexColors_));

		return *this;
	}

	CResourceModel::CResourceModel(CObject* outer) : nsRE::ResourceModel(), CObject(outer)
	{
	}

	SGReflection& CResourceModel::getReflection()
	{
		if (reflData_.get() == nullptr)
			reflData_ = std::make_unique<TReflectionStruct>(*this);
		return *reflData_.get();
	}

	void CResourceModel::OnAfterDeserialized2()
	{
		bool isEmpty = textureFilePath_.length() == 0 && objFilePath_.length() == 0 && vertShaderPath_.length() == 0 && fragShaderPath_.length() == 0;
		if (isEmpty)
			return;

		this->LoadFromFiles(Globals::GetGameAssetPath(textureFilePath_),
							Globals::GetGameAssetPath(objFilePath_),
							Globals::GetGameAssetPath(vertShaderPath_),
							Globals::GetGameAssetPath(fragShaderPath_));

		if (objFilePath_.length() == 0 && vertices_.size() > 0)
		{	// obj파일이 아닌 직접 지정 방식이다
			this->mesh_.loadFromPlainData(vertices_, uvs_, normals_);
			this->mesh_.setVertexColors(vertexColors_);
		}
	}
};
