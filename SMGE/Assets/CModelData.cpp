#include "CModelData.h"

namespace SMGE
{
	SGRefl_ModelData::SGRefl_ModelData(CModelData& md) : SGReflection(md),
		vertShaderPath_(md.vertShaderPath_),
		fragShaderPath_(md.fragShaderPath_),
		vertexAttribNumber_(md.vertexAttribNumber_),
		vertices_(md.vertices_)
	{
	}

	SGRefl_ModelData::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += _TO_REFL(CWString, vertShaderPath_);
		ret += _TO_REFL(CWString, fragShaderPath_);
		ret += _TO_REFL(int32, vertexAttribNumber_);
		ret += ReflectionUtils::ToCVector(vertices_, L"CVector<float>", L"vertices_", std::optional<size_t>{});

		return ret;
	}

	SGReflection& SGRefl_ModelData::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(vertShaderPath_, variableSplitted);
		_FROM_REFL(fragShaderPath_, variableSplitted);
		_FROM_REFL(vertexAttribNumber_, variableSplitted);

		vertices_.clear();
		ReflectionUtils::FromCVector(vertices_, variableSplitted, ReflectionUtils::FromRefl_PushBack(this->vertices_));
		return *this;
	}

	CModelData::CModelData(void* outer) : nsRE::OldModelAsset()
	{
		className_ = wtext("SMGE::CModelData");
	}

	SGReflection& CModelData::getReflection()
	{
		if (reflData_.get() == nullptr)
			reflData_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflData_.get();
	}

	void CModelData::OnAfterDeserialized()
	{
		initShaders();
	}
};
