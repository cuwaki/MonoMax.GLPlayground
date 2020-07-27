#include "CAssetModel.h"

namespace SMGE
{
	SGRefl_AssetModel::SGRefl_AssetModel(CAssetModel& md) : SGReflection(md),
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

	SGRefl_AssetModel::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += _TO_REFL(CWString, vertShaderPath_);
		ret += _TO_REFL(CWString, fragShaderPath_);
		ret += _TO_REFL(CWString, objFilePath_);
		ret += _TO_REFL(CWString, textureFilePath_);

		ret += ReflectionUtils::ToCVector(vertices_, L"CVector<glm::vec3>", L"vertices_", std::optional<size_t>{});
		ret += ReflectionUtils::ToCVector(uvs_, L"CVector<glm::vec2>", L"uvs_", std::optional<size_t>{});
		ret += ReflectionUtils::ToCVector(normals_, L"CVector<glm::vec3>", L"normals_", std::optional<size_t>{});
		ret += ReflectionUtils::ToCVector(vertexColors_, L"CVector<glm::vec3>", L"vertexColors_", std::optional<size_t>{});

		return ret;
	}

	SGReflection& SGRefl_AssetModel::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(vertShaderPath_, variableSplitted);
		_FROM_REFL(fragShaderPath_, variableSplitted);
		_FROM_REFL(objFilePath_, variableSplitted);
		_FROM_REFL(textureFilePath_, variableSplitted);

		vertices_.clear();
		ReflectionUtils::FromCVector(vertices_, variableSplitted, ReflectionUtils::FromRefl_PushBack(this->vertices_));
		uvs_.clear();
		ReflectionUtils::FromCVector(uvs_, variableSplitted, ReflectionUtils::FromRefl_PushBack(this->uvs_));
		normals_.clear();
		ReflectionUtils::FromCVector(normals_, variableSplitted, ReflectionUtils::FromRefl_PushBack(this->normals_));
		vertexColors_.clear();
		ReflectionUtils::FromCVector(vertexColors_, variableSplitted, ReflectionUtils::FromRefl_PushBack(this->vertexColors_));

		return *this;
	}

	CAssetModel::CAssetModel(void* outer) : nsRE::AssetModel()
	{
		className_ = wtext("SMGE::CAssetModel");
	}

	SGReflection& CAssetModel::getReflection()
	{
		if (reflData_.get() == nullptr)
			reflData_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflData_.get();
	}

	void CAssetModel::OnAfterDeserialized()
	{
		this->AssetModel::AssetModel(textureFilePath_, vertShaderPath_, fragShaderPath_, objFilePath_);

		if (objFilePath_.length() == 0 && vertices_.size() > 0)
		{	// obj파일이 아닌 직접 지정 방식이다
			this->mesh_.loadFromPlainData(vertices_, uvs_, normals_);
			this->mesh_.setVertexColors(vertexColors_);
		}

		this->ReadyToRender();
	}
};
