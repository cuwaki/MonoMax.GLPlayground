#include "RenderingEngine.h"
#include "../SuperPompoko/SPPKGame.h"
#include "../SMGE/CEngineBase.h"

#include "common/objloader.hpp"
#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/quaternion_utils.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

namespace SMGE
{
	namespace nsRE
	{
		CHashMap<CString, std::shared_ptr<nsRE::ResourceModelBase>> CResourceModelProvider::ResourceModels;

		std::shared_ptr<nsRE::ResourceModelBase> CResourceModelProvider::AddResourceModel(const CString& key, std::shared_ptr<ResourceModelBase> am)
		{
			auto already = FindResourceModel(key);
			if (already == nullptr)
				ResourceModels.insert(std::make_pair(key, std::move(am)));

			return FindResourceModel(key);
		}

		bool CResourceModelProvider::RemoveResourceModel(const CString& key)
		{
			auto found = ResourceModels.find(key);
			if (found == ResourceModels.end())
				return false;

			ResourceModels.erase(found);
			return true;
		}

		bool CResourceModelProvider::RemoveResourceModel(ResourceModelBase* am)
		{
			auto found = std::find_if(ResourceModels.begin(), ResourceModels.end(),
				[am](const auto& pair)
				{
					return pair.second.get() == am;
				});

			if (found == ResourceModels.end())
				return false;

			ResourceModels.erase(found);
			return true;
		}

		std::shared_ptr<ResourceModelBase> CResourceModelProvider::FindResourceModel(const CString& key)
		{
			auto clIt = ResourceModels.find(key);
			return clIt != ResourceModels.end() ? clIt->second : nullptr;
		}

		const CHashMap<CString, std::shared_ptr<ResourceModelBase>>& CResourceModelProvider::GetResourceModels()
		{
			return ResourceModels;
		}

		namespace TransformConst
		{
			/*
				이 함수의 의미는
				
				SMGE 에서 모델의 최초 기준 방향이 월드 +z축이라고 놓고 각종 회전 작업을 한다는 뜻이며				
				모델이 로드가 완료된 시점에서 월드의 +z축을 따라서 앞으로 바라보고 있게 설정된 상태로 되어야한다는 것이다.

				nsRE::Transform 에서 회전 처리를 할 때 DefaultModelFrontAxis() 를 default 로 이용하여 처리하기 때문이다.
			*/
			glm::vec3 DefaultModelFrontAxis()
			{
				static_assert(DefaultAxis_Front == ETypeAxis::Z);	// 이거 바꾸면 asset 도 그렇고 바꿀 곳이 많음! 건들지 말 것!
				return WorldZAxis;
			}
			glm::vec3 GetFrontAxis(const glm::mat3& currentRotMat)
			{
				return currentRotMat[DefaultAxis_Front];
			}
			glm::vec3 GetFrontAxis(const glm::mat4& currentRotMat)
			{
				return currentRotMat[DefaultAxis_Front];
			}

			glm::vec3 GetUpAxis(const glm::mat3& currentRotMat)
			{
				return currentRotMat[DefaultAxis_Up];
			}
			glm::vec3 GetUpAxis(const glm::mat4& currentRotMat)
			{
				return currentRotMat[DefaultAxis_Up];
			}

			glm::vec3 GetLeftAxis(const glm::mat3& currentRotMat)
			{
				return currentRotMat[DefaultAxis_Left];
			}
			glm::vec3 GetLeftAxis(const glm::mat4& currentRotMat)
			{
				return currentRotMat[DefaultAxis_Left];
			}

			glm::vec3 GetXAxis(const glm::mat4& currentRotMat)
			{
				return currentRotMat[ETypeAxis::X];
			}
			glm::vec3 GetYAxis(const glm::mat4& currentRotMat)
			{
				return currentRotMat[ETypeAxis::Y];
			}
			glm::vec3 GetZAxis(const glm::mat4& currentRotMat)
			{
				return currentRotMat[ETypeAxis::Z];
			}
		}
		using namespace TransformConst;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VertFragShaderSet::VertFragShaderSet(const CWString& vertShadPath, const CWString& fragShadPath)
		{
			Invalidate();

			programID_ = LoadShaders(vertShadPath.c_str(), fragShadPath.c_str());

			// 모든 셰이더가 공통으로 가져야할 것
			unif_MVPMatrixID_ = glGetUniformLocation(programID_, "MVP");
			unif_ViewMatrixID_ = glGetUniformLocation(programID_, "V");
			unif_ModelMatrixID_ = glGetUniformLocation(programID_, "M");
			unif_LightWorldPosition_ = glGetUniformLocation(programID_, "LightPosition_worldspace");

			unif_TextureSampleI_ = glGetUniformLocation(programID_, "myTextureSampler");

			// 일단 공통 셰이더 용으로 하드코딩
			vertAttrArray_ = 0;
			uvAttrArray_ = 1;
			normAttrArray_ = 2;
			vertexColorAttrArray_ = 3;
		}

		VertFragShaderSet::~VertFragShaderSet()
		{
			Destroy();
		}

		void VertFragShaderSet::Destroy()
		{
			if (programID_ != 0)
			{
				glDeleteProgram(programID_);
				programID_ = 0;
			}
		}

		void VertFragShaderSet::Invalidate()
		{
			programID_ = 0;
			unif_MVPMatrixID_ = -1;	// glGetUniform 여기서 에러나면 여기에 -1 들어가길래 이렇게 한다
			unif_ViewMatrixID_ = -1;
			unif_ModelMatrixID_ = -1;
			unif_TextureSampleI_ = -1;
			unif_LightWorldPosition_ = -1;
			vertAttrArray_ = -1, uvAttrArray_ = -1, normAttrArray_ = -1, vertexColorAttrArray_ = -1;
		}

		void VertFragShaderSet::Use() const
		{
			assert(programID_ != 0);
			glUseProgram(programID_);
		}

		std::map<CWString, std::unique_ptr<VertFragShaderSet>> VertFragShaderSet::Cache;

		VertFragShaderSet::VertFragShaderSet(VertFragShaderSet&& c) noexcept
		{
			operator=(std::move(c));
		}

		VertFragShaderSet& VertFragShaderSet::operator=(VertFragShaderSet&& c) noexcept
		{
			programID_ = c.programID_;
			unif_MVPMatrixID_ = c.unif_MVPMatrixID_;
			unif_ViewMatrixID_ = c.unif_ViewMatrixID_;
			unif_ModelMatrixID_ = c.unif_ModelMatrixID_;
			unif_TextureSampleI_ = c.unif_TextureSampleI_;
			unif_LightWorldPosition_ = c.unif_LightWorldPosition_;
			vertAttrArray_ = c.vertAttrArray_;
			uvAttrArray_ = c.uvAttrArray_;
			normAttrArray_ = c.normAttrArray_;
			vertexColorAttrArray_ = c.vertexColorAttrArray_;

			c.Invalidate();

			return *this;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		TextureData::TextureData(const CWString& texPath)
		{
			loadFromFile(texPath);
		}

		TextureData::~TextureData()
		{
			Destroy();
		}

		bool TextureData::loadFromFile(const CWString& texPath)
		{
			if (texPath.length() > 0)
				return loadDDS(texPath.c_str(), format_, mipMapCount_, width_, height_, image_);
			return false;
		}

		void TextureData::Destroy()
		{
			if (image_ != nullptr)
				free(image_);
		}

		TextureData::TextureData(TextureData&& c) noexcept
		{
			operator=(std::move(c));
		}

		TextureData& TextureData::operator=(TextureData&& c) noexcept
		{
			format_ = c.format_;
			mipMapCount_ = c.mipMapCount_;
			width_ = c.width_;
			height_ = c.height_;
			
			image_ = c.image_;	c.image_ = nullptr;

			return *this;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MeshData::MeshData(const CWString& objPath) : MeshData()
		{
			loadFromFile(objPath);
		}

		bool MeshData::loadFromFile(const CWString& objPath)
		{
			bool ret = false;

			if (objPath.length() > 0)
			{
				ret = loadOBJ(objPath.c_str(), vertices_, uvs_, normals_);

				vertexColors_.resize(vertices_.size());	// 일단 버컬은 000 으로 채워놓자
			}

			return ret;
		}

		bool MeshData::loadFromPlainData(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals)
		{
			if (vertices.size() == 0)
				return false;

			vertices_ = vertices;
			uvs_ = uvs;
			normals_ = normals;

			// 없으면 말고로
			//vertexColors_.resize(vertices.size());	// 일단 버컬은 000 으로 채워놓자
			//assert(vertices_.size() == uvs_.size());
			//assert(uvs_.size() == normals_.size());
			//assert(normals_.size() == vertexColors_.size());

			return true;
		}

		bool MeshData::setVertexColors(const std::vector<glm::vec3>& vertexColors)
		{
			if (vertexColors.size() == 0 || vertices_.size() != vertexColors.size())
				return false;

			vertexColors_ = vertexColors;
			return true;
		}

		void MeshData::Destroy()
		{
			Invalidate();
		}

		void MeshData::Invalidate()
		{
			vertices_.clear();
			uvs_.clear();
			normals_.clear();
			vertexColors_.clear();
		}

		MeshData::MeshData(MeshData&& c) noexcept
		{
			operator=(std::move(c));
		}

		MeshData& MeshData::operator=(MeshData&& c) noexcept
		{
			vertices_ = std::move(c.vertices_);
			uvs_ = std::move(c.uvs_);
			normals_ = std::move(c.normals_);
			vertexColors_ = std::move(c.vertexColors_);
			c.Invalidate();

			return *this;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Transform::Transform()
		{
			pendingPosition_ = Vec3_Zero;
			pendingScales_ = Vec3_One;

#ifdef REFACTORING_TRNASFORM
			pendingRotationMatrix_ = Mat4_Identity;
#else
			rotationRadianEuler_ = Vec3_Zero;
			directionForQuat_ = Vec3_Zero;
#endif
			Dirty();
			RecalcFinal();
		}

		void Transform::Translate(glm::vec3 worldPos)
		{
			pendingPosition_ = worldPos;

			Dirty();
		}
		void Transform::Scale(float scale)
		{
			pendingScales_ = glm::vec3(scale);

			Dirty();
		}
		void Transform::Scale(glm::vec3 scale)
		{
			pendingScales_ = scale;

			Dirty();
		}
		void Transform::Scale(ETypeAxis aType, float scale)
		{
			assert(aType >= 0 && aType < ETypeAxis::ETypeAxis_MAX);
			pendingScales_[aType] = scale;

			Dirty();
		}

#ifdef REFACTORING_TRNASFORM
		void Transform::Rotate(const glm::mat4& newRotMat)
		{
			pendingRotationMatrix_ = newRotMat;

			Dirty();
		}

		void Transform::Rotate(const glm::vec3& basisAxis, float degrees)
		{
			const auto halfRadians = glm::radians(degrees) * 0.5f;

			const auto sinHR = glm::sin(halfRadians);

			glm::quat q;
			q.w = glm::cos(halfRadians);
			q.x = basisAxis.x * sinHR;
			q.y = basisAxis.y * sinHR;
			q.z = basisAxis.z * sinHR;

			pendingRotationMatrix_ = glm::mat4_cast(q);

			Dirty();
		}

		void Transform::RotateEuler(glm::vec3 rotateDegrees, bool isWorld)
		{
			// 여기 - RotateEuler 등의 통합 함수로 처리해야겠다, 나중에 여기저기서 회전 적용 순서 다른 문제 생길 수 있음, 다른 곳도 회전 적용 순서를 통합 처리해라
			//
			// 20210328 - 여기저기서 오일러 회전의 기본 순서에 대한 말이 분분한 것 같다 - 아래의 문서들을 보면 그렇다
			// 일단 현재 glm 을 보니 eulerAngleXYZ 와 eulerAngleYXZ 두가지가 준비되어있다.
			// 그러므로 그냥 glm에서 지원하는 순서 중 가장 디폴트고 직관적인 XYZ 순서를 따르기로 한다
			//
			// 디폴트가 YZX 순서라는 문서 - http://www.opengl-tutorial.org/kr/intermediate-tutorials/tutorial-17-quaternions/
			// 디폴트가 ZYX 순서라는 문서 - https://kr.mathworks.com/help/uav/ref/eul2rotm.html
			//
			if (isWorld)
			{
				pendingRotationMatrix_ = Mat4_Identity;
				pendingRotationMatrix_ = glm::rotate(pendingRotationMatrix_, glm::radians(rotateDegrees.x), WorldAxis[ETypeAxis::X]);
				pendingRotationMatrix_ = glm::rotate(pendingRotationMatrix_, glm::radians(rotateDegrees.y), WorldAxis[ETypeAxis::Y]);
				pendingRotationMatrix_ = glm::rotate(pendingRotationMatrix_, glm::radians(rotateDegrees.z), WorldAxis[ETypeAxis::Z]);
			}
			else
			{	// 모델 축 기준으로 - 왠만하면 안써야한다, 짐벌락 일어남...
				pendingRotationMatrix_ = glm::rotate(pendingRotationMatrix_, glm::radians(rotateDegrees.x), GetXAxis(pendingRotationMatrix_));
				pendingRotationMatrix_ = glm::rotate(pendingRotationMatrix_, glm::radians(rotateDegrees.y), GetYAxis(pendingRotationMatrix_));
				pendingRotationMatrix_ = glm::rotate(pendingRotationMatrix_, glm::radians(rotateDegrees.z), GetZAxis(pendingRotationMatrix_));
			}

			Dirty();
		}

		void Transform::RotateDirection(const glm::vec3& newDirNormalized, glm::vec3 newUpNormalized, bool isSecurePerp)
		{
			if (isSecurePerp == true)
			{	// newUp과 newDir 의 직각을 보장해준다
				const auto right = glm::cross(newDirNormalized, newUpNormalized);
				newUpNormalized = glm::cross(right, newDirNormalized);
			}

			const auto currentDir = GetFrontAxis(pendingRotationMatrix_);
			const auto newDirQ = MathUtils::RotationBetweenVectors(currentDir, newDirNormalized);

			//auto currentUp = GetUpAxis(pendingRotationMatrix_);
			auto rotatedDefaultUp = newDirQ * WorldAxis[DefaultAxis_Up];	// newDir 때문에 Up축이 바뀔테니 바뀌게 해서
			const auto newUpQ = MathUtils::RotationBetweenVectors(rotatedDefaultUp, newUpNormalized);	// 바뀐 Up 축을 기준으로 newUp으로 돌아야할 Quat을 계산
			// 이렇게 해야 newUp 이 제대로 계산된다

			pendingRotationMatrix_ = glm::mat4_cast(newUpQ * newDirQ);

			Dirty();
		}
#else
		void Transform::RotateEuler(glm::vec3 rotateDegrees)
		{
			rotationRadianEuler_.x = glm::radians(rotateDegrees.x);
			rotationRadianEuler_.y = glm::radians(rotateDegrees.y);
			rotationRadianEuler_.z = glm::radians(rotateDegrees.z);

			Dirty();
		}
		void Transform::RotateEuler(ETypeRot rType, float degrees)
		{
			assert(rType >= 0 && rType < ETypeRot::ETypeRot_MAX);
			rotationRadianEuler_[rType] = glm::radians(degrees);

			Dirty();
		}
		void Transform::RotateQuat(const glm::vec3& dirForQuat)
		{
			directionForQuat_ = dirForQuat;

			Dirty();
		}
		const glm::vec3& Transform::GetRotationEuler() const
		{
			return rotationRadianEuler_;
		}
		const glm::vec3& Transform::GetDirectionQuat() const
		{
			return directionForQuat_;
		}
#endif

		const glm::mat4& Transform::FinalMatrix(bool isRecalc)
		{
			if(isRecalc == true)
				RecalcFinal();

			return finalMatrix_;
		}
		const glm::mat4& Transform::FinalMatrixNoRecalc() const
		{
			return finalMatrix_;
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		const glm::vec3& Transform::GetPendingPosition() const
		{
			return pendingPosition_;
		}
		const glm::vec3& Transform::GetPendingScales() const
		{
			return pendingScales_;
		}
		float Transform::GetPendingScale(TransformConst::ETypeAxis aType) const
		{
			assert(aType >= ETypeAxis::X && aType <= ETypeAxis::Z);
			return pendingScales_[aType];
		}
		glm::vec3 Transform::GetPendingAxis(TransformConst::ETypeAxis aType) const
		{
			const auto ret = pendingRotationMatrix_[aType] / pendingScales_[aType];
			return glm::normalize(ret);
		}
		glm::vec3 Transform::GetPendingFront() const
		{
			return GetPendingAxis(DefaultAxis_Front);
		}
		glm::vec3 Transform::GetPendingUp() const
		{
			return GetPendingAxis(DefaultAxis_Up);
		}
		glm::vec3 Transform::GetPendingLeft() const
		{
			return GetPendingAxis(DefaultAxis_Left);
		}
		glm::vec3 Transform::GetPendingRotationEulerDegreesWorld() const
		{
#ifdef REFACTORING_TRNASFORM
			return MathUtils::Mat2EulerWorld(pendingRotationMatrix_);
#else
			return glm::degrees(rotationRadianEuler_);
#endif
		}

		///////////////////////////////////////////////////////////////////////////////////////////
		glm::vec3 Transform::GetFinalPosition() const
		{
			return finalMatrix_[3];
		}
		float Transform::GetFinalScale(TransformConst::ETypeAxis aType) const
		{
			return finalScales_[aType];
		}
		glm::vec3 Transform::GetFinalScales() const
		{
			return finalScales_;
		}
		glm::vec3 Transform::GetFinalAxis(TransformConst::ETypeAxis aType) const
		{
			const auto ret = finalMatrix_[aType] / GetFinalScale(aType);
			return glm::normalize(ret);
		}
		glm::vec3 Transform::GetFinalFront() const
		{
			return GetFinalAxis(DefaultAxis_Front);
		}
		glm::vec3 Transform::GetFinalUp() const
		{
			return GetFinalAxis(DefaultAxis_Up);
		}
		glm::vec3 Transform::GetFinalLeft() const	// 왜 Left 냐면 오른손 좌표계 기준으로 +Z가 앞이므로 +X는 Left 가 되기 때문이다
		{
			return GetFinalAxis(DefaultAxis_Left);
		}
		glm::vec3 Transform::GetFinalRotationEulerDegreesWorld() const
		{
#ifdef REFACTORING_TRNASFORM
			return MathUtils::Mat2EulerWorld(finalMatrix_);
#else
			return glm::degrees(rotationRadianEuler_);	// 임시 - 그냥 대강 작동하도록
#endif
		}

		void Transform::Dirty(bool isForce)
		{
			if (isForce == false && isDirty_ == true)
				return;

			isDirty_ = true;

			// 부모가 더티될 때 자식들도 더티가 되어야한다 - 그래야 RecalcFinal 가 제대로 작동함
			std::for_each(children_.begin(), children_.end(),
				[isForce](auto child)
				{
					child->Dirty(isForce);
				}
			);
		}

		bool Transform::IsDirty() const
		{
			if (isDirty_)
				return true;

			// 내 부모들 중 하나라도 더티면 나도 더티다 - 이건 부모의 Dirty( 에서 보증되므로 여기서 안해도 된다

			return false;
		}

		void Transform::OnBeforeRendering()
		{
			RecalcFinal();	// 최적화 - 여기서 매번 모든 트랜스폼에 대해서 처리를 시도할 필요가 없다, 탑패런트로 가서 1회 처리하면 싸그리 처리되므로, 현재는 좀 낭비가 있는 상태이다
		}

		void Transform::ChangeParent(Transform* p)
		{
			if (parent_ == p)
				return;

			auto oldParent = parent_;

			parent_ = p; // this에게 부모 연결

			if (parent_ != nullptr)
			{	// 새 부모에게 this 연결
				parent_->children_.push_front(this);
			}

			// 가능성 - 미래에는 oldParent 가 무효한 포인터가 되어있을 가능성이 있음 - 차후 케이스가 발견되겠지?!!
			if (oldParent != nullptr)
			{	// 옛 부모에서는 제거
				auto found = std::find(oldParent->children_.begin(), oldParent->children_.end(), this);
				if(found != oldParent->children_.end())
					oldParent->children_.remove(*found);
			}

			Dirty(true);
		}

		bool Transform::HasParent() const
		{
			return GetParentConst<Transform>() != nullptr;
		}

		bool Transform::IsTop() const
		{
			return HasParent() == false;
		}

		void Transform::RecalcFinal_Internal(const Transform* parent)
		{
			if (IsDirty())
			{	// 나의 트랜스폼을 계산
#ifdef REFACTORING_TRNASFORM
				if (parent_ == nullptr || isAbsoluteTransform_)
				{
					const auto scaledMat = glm::scale(Mat4_Identity, pendingScales_);
					const auto translMat = glm::translate(Mat4_Identity, pendingPosition_);

					// S->R->T 순서로
					finalMatrix_ = translMat * pendingRotationMatrix_ * scaledMat;	// 최적화 - 인라인 처리해서 임시 객체 없애자
					finalScales_ = pendingScales_;
				}
				else
				{
					finalMatrix_ = parent_->FinalMatrixNoRecalc();

					// S->R->T 순서로
					finalMatrix_ = glm::translate(finalMatrix_, pendingPosition_);
					finalMatrix_ *= pendingRotationMatrix_;
					finalMatrix_ = glm::scale(finalMatrix_, pendingScales_);

					finalScales_ = parent_->pendingScales_ * pendingScales_;
				}

				//// 1.  내 변환 반영
				//const auto translMat = glm::translate(Mat4_Identity, pendingPosition_);
				//const auto scaledMat = glm::scale(Mat4_Identity, pendingScales_);

				//finalMatrix_ = translMat * pendingRotationMatrix_ * scaledMat;	// 최적화 - 인라인 처리해서 임시 객체 없애자
				//finalScales_ = pendingScales_;
#else
				//if (parent)
				//{	// 자식일 경우 - 부모의 트랜스폼 먼저 반영해야 의도대로 작동한다 / 이 코드에는 부모의 부모 것이 적용안되는 버그가 있다
				//	//finalMatrix_ = glm::rotate(Mat4_Identity, glm::radians(parent->rotationRadianEuler_[ETypeRot::PITCH]), WorldAxis[ETypeRot::PITCH]);
				//	//finalMatrix_ = glm::rotate(finalMatrix_, glm::radians(parent->rotationRadianEuler_[ETypeRot::YAW]), WorldAxis[ETypeRot::YAW]);
				//	//finalMatrix_ = glm::rotate(finalMatrix_, glm::radians(parent->rotationRadianEuler_[ETypeRot::ROLL]), WorldAxis[ETypeRot::ROLL]);

				//	finalMatrix_ = glm::translate(Mat4_Identity, parent->translation_);
				//	finalMatrix_ = glm::rotate(finalMatrix_, glm::radians(parent->rotationRadianEuler_[ETypeRot::PITCH]), WorldAxis[ETypeRot::PITCH]);
				//	finalMatrix_ = glm::rotate(finalMatrix_, glm::radians(parent->rotationRadianEuler_[ETypeRot::YAW]), WorldAxis[ETypeRot::YAW]);
				//	finalMatrix_ = glm::rotate(finalMatrix_, glm::radians(parent->rotationRadianEuler_[ETypeRot::ROLL]), WorldAxis[ETypeRot::ROLL]);
				//	finalMatrix_ = glm::scale(finalMatrix_, parent->scale_);
				//}
				//else
				//{
				//	finalMatrix_ = Mat4_Identity;
				//}

				// 풀어서 조합하는 경우는 아래와 같이 되는데
				// 매트릭스로 합쳐서 하는 경우에는
				// mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix;
				// 이렇게 된다고 한다
				// http://www.opengl-tutorial.org/kr/intermediate-tutorials/tutorial-17-quaternions/
				//finalMatrix_ = glm::translate(finalMatrix_, translation_);

				// 일반적으로 Y X Z순서로 한다고 한다 - 요 피치 롤  흠...
				//if(rotationRadianEuler_[ETypeRot::PITCH] != 0.f)
				//	finalMatrix_ = glm::rotate(finalMatrix_, glm::radians(rotationRadianEuler_[ETypeRot::PITCH]), WorldAxis[ETypeAxis::X]);
				//if (rotationRadianEuler_[ETypeRot::YAW] != 0.f)
				//	finalMatrix_ = glm::rotate(finalMatrix_, glm::radians(rotationRadianEuler_[ETypeRot::YAW]), WorldAxis[ETypeAxis::Y]);
				//if (rotationRadianEuler_[ETypeRot::ROLL] != 0.f)
				//	finalMatrix_ = glm::rotate(finalMatrix_, glm::radians(rotationRadianEuler_[ETypeRot::ROLL]), WorldAxis[ETypeAxis::Z]);

				//finalMatrix_ = glm::scale(finalMatrix_, scale_);

				const auto translMat = glm::translate(Mat4_Identity, pendingPosition_);

				// 1. 오일러각 조절로 기본 각도를 맞추고
				// https://gamedev.stackexchange.com/questions/13436/glm-euler-angles-to-quaternion
				const glm::quat euler2Quat(rotationRadianEuler_);
				const auto eulerRotMat = glm::toMat4(euler2Quat);

				// 여기 - 쿼터 회전 후 오일러 rotationRadianEuler_ 이것들의 값이 의미가 없어지는 문제가 있다, 어떻게 해야할까??
				// 쿼터니언 기본 정보
				// http://www.opengl-tutorial.org/kr/intermediate-tutorials/tutorial-17-quaternions/
				// 쿼터니언 2 오일러 와 그의 반대 - https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

				// 2. 방향지정으로 회전
				const auto modelDirAxis = GetFrontAxis(eulerRotMat);
				const auto modelUpAxis = GetUpAxis(eulerRotMat);
				//const glm::quat qX = glm::angleAxis(glm::degrees(rotationRadianQuat_.x), glm::vec3(eulerRotMat[ETypeAxis::X]));
				const glm::quat lookAt = glm::normalize(MathUtils::LookAt(directionForQuat_, modelUpAxis, modelDirAxis, WorldYAxis));
				//const glm::quat lookAt = MathUtils::LookAt(directionForQuat_, modelUpAxis, WorldZAxis, WorldYAxis);
				const auto quatRotMat = glm::mat4_cast(lookAt);

				// 나의 스케일 처리
				const auto scalMat = glm::scale(Mat4_Identity, pendingScales_);
				finalMatrix_ = translMat * quatRotMat * eulerRotMat * scalMat;

				// 나에게 부모 트랜스폼을 적용 - 아래와 같이 하면 이동에 스케일이 반영되어서 더 적게 움직이는 거나 회전값이 자식에게 그대로 적용되는 등의 문제가 생긴다
				//finalMatrix_ = finalMatrix_ * parentMatrix;

				if (parent && isAbsoluteTransform_ == false)
				{	// 2. 부모 변환 반영
					assert(parent == parent_);

					finalMatrix_ *= parent->finalMatrix_;
					finalScales_ *= parent->finalScales_;
				}
#endif

				isDirty_ = false;
			}

			// 나의 자식들에게 전파
			std::for_each(children_.begin(), children_.end(),
				[this](auto child)
				{
					child->RecalcFinal_Internal(this);
				}
			);
		}

		// DEPRECATED
		//glm::vec3 Transform::GetFinalWorldScales() const
		//{
		//	auto topParent = GetTopParentConst();
		//	topParent->RecalcFinalWorldScales_Internal(nullptr);

		//	return finalScales_;
		//}

		//void Transform::RecalcFinalWorldScales_Internal(const Transform* parent) const
		//{
		//	if (parent != nullptr)
		//		finalScales_ = scale_ * parent->finalScales_;
		//	else
		//		finalScales_ = scale_;

		//	// 나의 자식들에게 전파
		//	std::for_each(children_.begin(), children_.end(),
		//		[this](auto child)
		//		{
		//			child->RecalcFinalWorldScales_Internal(this);
		//		}
		//	);
		//}

		void Transform::RecalcFinal()
		{
			// this 가 트랜스폼 부모자식 체인의 어디에 위치해있든
			// topParent 까지 올라가서 RecalcFinal_Internal 하여 부모->자식으로 딱 1회 전파해나가면
			// 거의 낭비 없이 Recalc 가 처리된다
			// 단 현재는 매번 RecalcFinal_Internal( 에서 자식들을 의미없이 한번씩 돌아주는 처리가 있긴하다 - 아마도 짧을 forward_list의 순회 - 최적화 고려
			
			auto topParent = GetTopParent<Transform>();	// 최적화 - 탑패런트 이거 캐시해두자
			topParent->RecalcFinal_Internal(nullptr);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_DEBUG) || defined(DEBUG)
		ResourceModelBase::~ResourceModelBase()
		{
			return;	// 테스트 코드 ㅡ 디버깅용
		}
#endif

		void ResourceModelBase::NewAndRegisterRenderModel(const GLFWwindow* contextWindow) const
		{
			auto newOne = std::make_unique<RenderModel>(*this, 0);
			newOne->CreateFromResource();
			renderModelsPerContext_.insert( std::make_pair(contextWindow, std::move(newOne)) );
		}

		class RenderModel* ResourceModelBase::GetRenderModel(const GLFWwindow* contextWindow) const
		{
			auto found = renderModelsPerContext_.find(contextWindow);
			if (found == renderModelsPerContext_.end())
			{
				NewAndRegisterRenderModel(contextWindow);	// 없으면 만들고
				// 여기 - 중요 - 렌더링이랑 틱 사이에 펜스 쳐야한다 - GLControl::Tick( vs ::Rendering(
				return GetRenderModel(contextWindow);
			}

			return found->second.get();
		}

		const MeshData& ResourceModelBase::GetMesh() const
		{
			MeshData temp;
			return temp;	// 일부러 - 여기가 호출되면 안되기 때문이다
		}

		const TextureData& ResourceModelBase::GetTexture() const
		{
			TextureData temp;
			return temp;	// 일부러 - 여기가 호출되면 안되기 때문이다
		}

		void ResourceModelBase::Invalidate()
		{
			renderModelsPerContext_.clear();
		}

		ResourceModelBase::ResourceModelBase(ResourceModelBase&& c) noexcept
		{
			operator=(std::move(c));
		}

		ResourceModelBase& ResourceModelBase::operator=(ResourceModelBase&& c) noexcept
		{
			renderModelsPerContext_ = std::move(c.renderModelsPerContext_);
			c.Invalidate();

			return *this;
		}

		void ResourceModel::LoadFromFiles(const CWString& textureFilePath, const CWString& objPath, const CWString& vertShadPath, const CWString& fragShadPath)
		{
			texture_.loadFromFile(textureFilePath);
			mesh_.loadFromFile(objPath);

			vertShaderPath_ = vertShadPath;
			fragShaderPath_ = fragShadPath;
		}

		ResourceModel::ResourceModel(ResourceModel&& c) noexcept
		{
			operator=(std::move(c));
		}

		ResourceModel& ResourceModel::operator=(ResourceModel&& c) noexcept
		{
			texture_ = std::move(c.texture_);
			mesh_ = std::move(c.mesh_);
			vertShaderPath_ = std::move(vertShaderPath_);
			fragShaderPath_ = std::move(fragShaderPath_);

			ResourceModelBase::operator=(std::move(c));

			return *this;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		GLuint RenderModel::GetTextureID(int texSamp) const
		{
			return usingTextureID_;
		}

		const VertFragShaderSet* RenderModel::GetShaderSet() const
		{
			return vfShaderSet_;
		}

		GLuint RenderModel::GetShaderID() const
		{
			return GetShaderSet() ? GetShaderSet()->programID_ : 0;
		}

		void RenderModel::CallGLDraw(size_t verticesSize) const
		{
			glDrawArrays(GL_TRIANGLES, 0, verticesSize);
		}

		void RenderModel::AddWorldObject(WorldObject* wm)
		{
			worldObjectsW_.push_back(wm);

			assert(wm->renderModel_ == nullptr);	// 이렇지 않으면 이전 거에서 빼주는 처리가 필요하다

			if (wm->renderModel_ == nullptr)
				wm->renderModel_ = this;
		}

		void RenderModel::RemoveWorldObject(WorldObject* wm) const
		{
			auto it = std::find(worldObjectsW_.begin(), worldObjectsW_.end(), wm);
			if (it != worldObjectsW_.end())
				worldObjectsW_.erase(it);
		}

		const std::vector<WorldObject*>& RenderModel::WorldObjects() const
		{
			return worldObjectsW_;
		}

		RenderModel::~RenderModel()
		{
			Destroy();
		}

		RenderModel::RenderModel(const ResourceModelBase& resModelBase, GLuint texSamp) : resourceModel_(resModelBase), usingTextureSampleI_(texSamp)
		{
			Invalidate();
		}

		RenderModel::RenderModel(RenderModel&& c) noexcept : resourceModel_(c.resourceModel_)
		{
			verticesSize_ = c.verticesSize_;

			vao_ = c.vao_;
			vertexBuffer_ = c.vertexBuffer_;
			uvBuffer_ = c.uvBuffer_;
			normalBuffer_ = c.normalBuffer_;
			vertexColorBuffer_ = c.vertexColorBuffer_;
			glDrawType_ = c.glDrawType_;

			vfShaderSet_ = c.vfShaderSet_;
			usingTextureID_ = c.usingTextureID_;
			usingTextureSampleI_ = c.usingTextureSampleI_;

			c.Invalidate();
		}

		void RenderModel::CreateFromResource()
		{
			const auto& resModel = static_cast<const ResourceModel&>(resourceModel_);
			vfShaderSet_ = VertFragShaderSet::FindOrLoadShaderSet<VertFragShaderSet>(resModel.GetVertShaderPath(), resModel.GetFragShaderPath());

			// 임시 코드 - 렌더모델이 메시 관련일 경우에만 아래 코드가 실행되어야한다, 고민해봐라
			const auto& textureData = resModel.GetTexture();
			if (textureData.image_ != nullptr)
			{
				usingTextureID_ = GLCreateTextureDDS(textureData.format_, textureData.mipMapCount_, textureData.width_, textureData.height_, textureData.image_);
			}

			const auto& mesh = resModel.GetMesh();
			GenGLMeshDatas(mesh.vertices_, mesh.uvs_, mesh.normals_, mesh.vertexColors_);

			// 최적화 - 게임의 경우 바인드가 끝나고 나면 resourceModel_ 의 내용을 비워도 된다???
		}

		bool RenderModel::GenGLMeshDatas(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& vertexColors)
		{
			if (vertices.size() == 0)
				return false;

			verticesSize_ = vertices.size();

			glGenVertexArrays(1, &vao_);
			glBindVertexArray(vao_);

			glGenBuffers(1, &vertexBuffer_);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], glDrawType_);

			if (uvs.size() > 0)
			{
				glGenBuffers(1, &uvBuffer_);
				glBindBuffer(GL_ARRAY_BUFFER, uvBuffer_);
				glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], glDrawType_);
			}

			if (normals.size() > 0)
			{
				glGenBuffers(1, &normalBuffer_);
				glBindBuffer(GL_ARRAY_BUFFER, normalBuffer_);
				glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], glDrawType_);
			}

			if (vertexColors.size() > 0)
			{
				glGenBuffers(1, &vertexColorBuffer_);
				glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer_);
				glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), &vertexColors[0], glDrawType_);
			}

			glBindVertexArray(0);

			return true;
		}

		void RenderModel::Destroy()
		{
			if (vao_ != 0)
			{
				glDeleteBuffers(1, &vertexBuffer_);
				glDeleteBuffers(1, &uvBuffer_);
				glDeleteBuffers(1, &normalBuffer_);
				glDeleteBuffers(1, &vertexColorBuffer_);
				glDeleteVertexArrays(1, &vao_);

				vao_ = 0;
			}
		}

		void RenderModel::Invalidate()
		{
			verticesSize_ = 0;

			vao_ = 0;
			vertexBuffer_ = 0;
			uvBuffer_ = 0;
			normalBuffer_ = 0;
			vertexColorBuffer_ = 0;
			glDrawType_ = GL_STATIC_DRAW;

			usingTextureID_ = 0;
			usingTextureSampleI_ = 0;
		}

		// CRenderingPass 와의 엮인 처리로 좀 낭비가 있다 - ##renderingpasswith03
		void RenderModel::Render(const glm::mat4& VP)
		{
			for (auto& wmPtr : WorldObjects())
			{	// 나를 사용하는 모든 월드 오브젝트을 찍는다
				if (wmPtr->IsRendering() == false)
					continue;

				wmPtr->OnBeforeRendering();

				//glm::mat4 ModelMatrix = glm::mat4(1);
				//ModelMatrix = glm::translate(ModelMatrix, worldPos);

				const auto MVP = VP * wmPtr->FinalMatrix(false);

				// 이걸 유니폼으로 하지말고 VP를 프레임당 한번 고정해두고 셰이더 안에서 만드는 게 나을지도?? 프레임 비교 필요하겠다
				if(GetShaderSet()->unif_MVPMatrixID_ != -1)
					glUniformMatrix4fv(GetShaderSet()->unif_MVPMatrixID_, 1, GL_FALSE, &MVP[0][0]);
				if (GetShaderSet()->unif_ModelMatrixID_ != -1)
					glUniformMatrix4fv(GetShaderSet()->unif_ModelMatrixID_, 1, GL_FALSE, &wmPtr->FinalMatrix(false)[0][0]);

				CallGLDraw(verticesSize_);
			}
		}

		void RenderModel::BeginRender()
		{
			glBindVertexArray(vao_);

			if (usingTextureID_ != 0)
			{
				glActiveTexture(GL_TEXTURE0 + usingTextureSampleI_);
				glBindTexture(GL_TEXTURE_2D, usingTextureID_);
				// Set our "myTextureSampler" sampler to user Texture Unit 0
				glUniform1i(GetShaderSet()->unif_TextureSampleI_, usingTextureSampleI_);
			}

			// 1rst attribute buffer : vertices
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);

			glEnableVertexAttribArray(GetShaderSet()->vertAttrArray_);
			glVertexAttribPointer(
				GetShaderSet()->vertAttrArray_,                  // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			if (uvBuffer_ != 0)
			{	// 2nd attribute buffer : UVs
				glBindBuffer(GL_ARRAY_BUFFER, uvBuffer_);
				glEnableVertexAttribArray(GetShaderSet()->uvAttrArray_);
				glVertexAttribPointer(
					GetShaderSet()->uvAttrArray_,                                // attribute
					2,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);
			}

			if (normalBuffer_ != 0)
			{	// 3rd attribute buffer : normals
				glBindBuffer(GL_ARRAY_BUFFER, normalBuffer_);
				glEnableVertexAttribArray(GetShaderSet()->normAttrArray_);
				glVertexAttribPointer(
					GetShaderSet()->normAttrArray_,                                // attribute
					3,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);
			}

			if (vertexColorBuffer_ != 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer_);
				glEnableVertexAttribArray(GetShaderSet()->vertexColorAttrArray_);
				glVertexAttribPointer(
					GetShaderSet()->vertexColorAttrArray_,                                // attribute
					3,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);
			}
		}

		void RenderModel::EndRender()
		{
			glDisableVertexAttribArray(GetShaderSet()->vertAttrArray_);
			if (uvBuffer_ != 0)
				glDisableVertexAttribArray(GetShaderSet()->uvAttrArray_);
			if (normalBuffer_ != 0)
				glDisableVertexAttribArray(GetShaderSet()->normAttrArray_);
			if (vertexColorBuffer_ != 0)
				glDisableVertexAttribArray(GetShaderSet()->vertexColorAttrArray_);

			glBindVertexArray(0);
		}

		void RenderModel::UseShader(const glm::mat4& V, const glm::vec3& lightPos)
		{
			if (GetShaderID() == 0)
				return;

			GetShaderSet()->Use();

			if(GetShaderSet()->unif_ViewMatrixID_ != -1)
				glUniformMatrix4fv(GetShaderSet()->unif_ViewMatrixID_, 1, GL_FALSE, &V[0][0]);
			if(GetShaderSet()->unif_LightWorldPosition_ != -1)
				glUniform3f(GetShaderSet()->unif_LightWorldPosition_, lightPos.x, lightPos.y, lightPos.z);
		}

		WorldObject::WorldObject(RenderModel* rm) : renderModel_(nullptr)
		{
			if(rm != nullptr)
				rm->AddWorldObject(this);
		}
		WorldObject::~WorldObject()
		{
			if (renderModel_ != nullptr)
				renderModel_->RemoveWorldObject(this);

			renderModel_ = nullptr;
		}
		WorldObject::WorldObject(const WorldObject& c) : WorldObject(c.renderModel_)
		{
		}
		WorldObject::WorldObject(WorldObject&& c) noexcept : WorldObject(c.renderModel_)
		{
			c.~WorldObject();
		}
		void WorldObject::SetRendering(bool isv, bool propagate)
		{
			if (isRendering_ == isv)
				return;

			isRendering_ = isv;

			if (propagate)
			{
				for (auto child : children_)
				{
					static_cast<WorldObject*>(child)->SetRendering(isv, propagate);	// 여기 - 현재는 항상 WorldObject* 지만 차후 아닐 수도 있다
				}
			}
		}
		bool WorldObject::IsRendering() const
		{
			return isRendering_;
		}

		class RenderModel* WorldObject::GetRenderModel() const
		{
			return renderModel_;
		}
	}

	namespace nsRE
	{
		CRenderingEngine::CRenderingEngine(int widthOriginal, int heightOriginal, float dpiRate) :
			m_clearColor(0.f, 0.f, 0.f, 1.0f)
		{
			m_dpiRate = dpiRate;

			m_widthOriginal = widthOriginal;
			m_heightOriginal = heightOriginal;
			m_widthWindowDPI = widthOriginal * m_dpiRate;
			m_heightWindowDPI = heightOriginal * m_dpiRate;
		}
		
		CRenderingEngine::~CRenderingEngine()
		{
		}

		void CRenderingEngine::initWindow()
		{
			if (!glfwInit())
				throw std::exception();

			glfwWindowHint(GLFW_SAMPLES, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			//glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);

#if IS_EDITOR
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

			//auto monitor = glfwGetPrimaryMonitor();
			//const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			//auto window_width = mode->width;
			//auto window_height = mode->height;

			m_window = glfwCreateWindow(m_widthWindowDPI, m_heightWindowDPI, "Hidden OpenGL m_window", NULL, NULL);
#else
			glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

			m_window = glfwCreateWindow(m_widthWindowDPI, m_heightWindowDPI, "OpenGL m_window", NULL, NULL);
#endif

			if (!m_window)
			{
				glfwTerminate();
				throw std::exception();
			}

			glfwMakeContextCurrent(m_window);

			// glew 때문에 막음, 서로 인클루드 빼라며 충돌남
			//if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			//{
			//	glfwTerminate();
			//	throw std::exception();
			//}

			// But on MacOS X with a retina screen it'll be m_widthOriginal*2 and m_heightOriginal*2, so we get the actual framebuffer size:
			glfwGetFramebufferSize(m_window, &m_framebufferWidth, &m_framebufferHeight);

			// Initialize GLEW
			glewExperimental = true; // Needed for core profile
			if (glewInit() != GLEW_OK) {
				fprintf(stderr, "Failed to initialize GLEW\n");
				getchar();
				glfwTerminate();
				return;
			}

#if IS_EDITOR
#else
			// Ensure we can capture the escape key being pressed below
			glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
			// Hide the mouse and enable unlimited mouvement
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			// Set the mouse at the center of the screen
			glfwPollEvents();
			glfwSetCursorPos(m_window, GetWidth() / 2, GetHeight() / 2);
#endif

			glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			glFrontFace(GL_CCW);
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);

			//glEnable(GL_LINE_SMOOTH);
			//GLfloat lineWidthRange[2] = { 0.0f, 0.0f };
			//glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
			//glEnable(GL_PROGRAM_POINT_SIZE);

			//glEnable(GL_MULTISAMPLE);

			lastRenderingPass_ = std::make_unique<CPostEffectPass>(L"renderTargetNDC.vert", L"renderTargetOverwrite.frag");
		}

		HWND CRenderingEngine::GetHwndGLFWWindow() const
		{
			return glfwGetWin32Window(m_window);
		}

		// 여기 - 실제 width, height 를 정확히 맞추려면 .xaml 에서 w = 16, h = 39 더 추가해줘야한다
		void CRenderingEngine::Resize(int width, int height)
		{
			m_widthOriginal = width;
			m_heightOriginal = height;
			m_widthWindowDPI = width * m_dpiRate;
			m_heightWindowDPI = height * m_dpiRate;

			m_bufferLengthW = GetWidth() * GetHeight() * m_colorDepth;

			// 체크 사항 - 이거 다시 체크해야하려나??
			//glfwGetFramebufferSize(m_window, &m_framebufferWidth, &m_framebufferHeight);
			m_bufferLengthF = m_framebufferWidth * m_framebufferHeight * m_colorDepth;

			// 체크 사항 - 이거 없어도 되는데 왜 하는걸까?
			//free(m_glRenderHandle);
			//m_glRenderHandle = (char*)malloc(m_bufferLengthW);

			glfwSetWindowSize(m_window, m_widthWindowDPI, m_heightWindowDPI);
			glViewport(0, 0, GetWidth(), GetHeight());

			GetRenderingCamera().ComputeMatricesFromInputs(false, GetWidth(), GetHeight());

			ResizeRenderTargets();
		}

		void CRenderingEngine::ResizeRenderTargets()
		{
			const SRect2D ndc = { { -1.f, -1.f }, { 1.f, 1.f } };

			// 1920 * 1080 * RGBA 32BIT + 1920 * 1080 * D24_S8
			// 이렇게 되니까 RT 하나당 16메가 정도 먹는다
			renderTarget0_ = std::make_unique<CRenderTarget>(glm::vec2(GetWidth(), GetHeight()), GL_RGBA, GL_DEPTH24_STENCIL8, ndc,
				glm::vec3{ 0.f, 1.f, 0.f });	// 테스트 코드 - 일부러 빨간 색

			// 현재 포스트 프로세스가 없어서 renderTarget1_ 는 쓰일 일이 없지만 일단 만들어둠
			renderTarget1_ = std::make_unique<CRenderTarget>(glm::vec2(GetWidth(), GetHeight()), GL_RGBA, GL_DEPTH24_STENCIL8, ndc,
				glm::vec3{ 1.f, 0.f, 0.f });	// 테스트 코드 - 일부러 빨간 색

			// 기본 렌더타겟으로 생성하면 백버퍼를 가리키게 된다
			renderTargetBackBuffer_ = std::make_unique<CRenderTarget>(glm::vec2(0, 0), GL_ColorType, GL_DEPTH24_STENCIL8, ndc,
				m_clearColor);
		}

		void CRenderingEngine::Init()
		{
			if (m_window != nullptr)
				throw std::exception("it is trying reinit");

			initWindow();

			//////////////////////////////////////////////////////////////////////////////////////////
			// 테스트 코드 - 초기 카메라 처리
			//float cameraInitialDist = 20;
			GetRenderingCamera().SetCameraPos({ 0,0,0 });
			GetRenderingCamera().SetCameraLookAt({ 0,0,1 });
			GetRenderingCamera().ComputeMatricesFromInputs(true, GetWidth(), GetHeight());

			//////////////////////////////////////////////////////////////////////////////////////////
			// 게임 처리
			gameBase_ = std::make_unique<SMGE::SPPKGame>(nullptr);
			gameBase_->GetEngine()->SetRenderingEngine(this);
		}

		void CRenderingEngine::DeInit()
		{
		}

		void CRenderingEngine::Tick()
		{
			// 테스트 코드 - 0.01f - 대강 느낌만 맞춰둠
			gameBase_->Tick(0.01f);
			//double currentTime = glfwGetTime();
			//float theta = currentTime * 2.f;

			//int i = 0;
			//for (auto& wm : WorldObjects_)
			//{
			//	wm.RotateEuler(ETypeRot::YAW, glm::degrees(-theta * ((++i % 3) + 1)));
			//}
		}

//#define OLD_RENDER_PASS

#if IS_EDITOR
		void CRenderingEngine::Render(char* imgBuffer)
#else
		void CRenderingEngine::Render()
#endif
		{
			const auto& renderCam = GetRenderingCamera();
			const auto V = renderCam.GetViewMatrix();
			const auto VP = renderCam.GetProjectionMatrix() * renderCam.GetViewMatrix();

#ifdef OLD_RENDER_PASS
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// 테스트 코드 - 라이트를 회전시키자
			const float lightRotateRadius = 4;
			glm::vec4 lightPos = glm::vec4(0, 8, lightRotateRadius, 1);
			static double lastTime = glfwGetTime();
			float currentTime = glfwGetTime();
			float theta = currentTime * 0.5f;
			glm::mat4 lightRotateMat(1);
			lightRotateMat = glm::rotate(lightRotateMat, theta, glm::vec3(0, 1, 0));
			lightPos = lightRotateMat * lightPos;
			
			// 리소스 모델 -> 월드 오브젝트들
			for (const auto& it : CResourceModelProvider::GetResourceModels())
			{
				auto rm = it.second.get()->GetRenderModel(nullptr);
				if (rm != nullptr && rm->GetShaderID() > 0)
				{
					rm->UseShader(V, glm::vec3(lightPos));	// 셰이더 마다 1회
					rm->BeginRender();
					rm->Render(VP);
					rm->EndRender();
				}
			}
#else
			auto writeRT = renderTarget0_.get(), readRT = renderTarget1_.get();
			for (auto& rp : renderingPasses_)
			{
				rp->RenderTo(V, VP, OUT writeRT, OUT readRT);
			}
			
			auto finalResult = writeRT;

			auto backBuffer = renderTargetBackBuffer_.get();
			glDisable(GL_DEPTH_TEST);	// CGLState 적용 필요
			lastRenderingPass_->RenderTo(V, VP, backBuffer, finalResult);
			glEnable(GL_DEPTH_TEST);	// CGLState 적용 필요
#endif

#if IS_EDITOR
			if (imgBuffer != nullptr)
			{	// 개느려!
				glFlush();
				glFinish();
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				glReadPixels(0, 0, GetWidth(), GetHeight(), GL_ColorType, GL_UNSIGNED_BYTE, imgBuffer);
			}
#endif

			glfwSwapBuffers(m_window);
			glfwPollEvents();
		}

		void CRenderingEngine::Stop()
		{
			m_isRunning = false;
		}

#if IS_EDITOR
		void CRenderingEngine::getWriteableBitmapInfo(double& outDpiX, double& outDpiY, int& outColorDepth)
		{
			outDpiX = 96;
			outDpiY = 96;
			outColorDepth = m_colorDepth;	// 4 for PixelFormats::Pbgr32
		}
#endif

		void CRenderingEngine::ScreenPosToWorld(const glm::vec2& mousePos, glm::vec3& outWorldPos, glm::vec3& outWorldDir)
		{
			const auto& renderCam = GetRenderingCamera();

			auto glY = GetHeight() - mousePos.y;	// 스크린좌표를 gl좌표로 취급해야하므로 뒤집어줘야함

			auto ndcX = (mousePos.x / (float)GetWidth() - 0.5f) * 2.0f; // [0,1024] -> [-1,1]
			auto ndcY = (glY / (float)GetHeight() - 0.5f) * 2.0f; // [0, 768] -> [-1,1]

			// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
			glm::vec4 lRayStart_NDC(
				ndcX, ndcY,
				-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates	// - GL 이라서 -1 일걸? DirectX 는 0이었던 듯
				1.0f
			);
			glm::vec4 lRayEnd_NDC(
				ndcX, ndcY,
				1.0f,
				1.0f
			);

			// The Projection matrix goes from Camera Space to NDC.
			// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
			glm::mat4 toCameraSpace = glm::inverse(renderCam.GetProjectionMatrix());

			// The View Matrix goes from World Space to Camera Space.
			// So inverse(ViewMatrix) goes from Camera Space to World Space.
			glm::mat4 toWorldSpace = glm::inverse(renderCam.GetViewMatrix());

			// ndc -> camera
			glm::vec4 lRayStart_camera = toCameraSpace * lRayStart_NDC;
			lRayStart_camera /= lRayStart_camera.w;

			// camera -> world
			glm::vec4 lRayStart_world = toWorldSpace * lRayStart_camera;
			lRayStart_world /= lRayStart_world.w;

			glm::vec4 lRayEnd_camera = toCameraSpace * lRayEnd_NDC;
			lRayEnd_camera /= lRayEnd_camera.w;

			glm::vec4 lRayEnd_world = toWorldSpace * lRayEnd_camera;
			lRayEnd_world /= lRayEnd_world.w;

			// Faster way (just one inverse)
			//glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
			//glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
			//glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

			outWorldPos = glm::vec3(lRayStart_world);

			//glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
			//lRayDir_world = glm::normalize(lRayDir_world);
			//outWorldDir = glm::normalize(lRayDir_world);
			//outWorldDir = GetRenderingCamera().GetCameraFront();
			outWorldDir = glm::normalize(outWorldPos - renderCam.GetCameraPos());
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::stack<GLint> CGLState::fboStack_;

		void CGLState::PushState(std::string_view glState)
		{
			if (glState == "glst:fbo")
			{
				GLint cur;
				glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &cur);	// 최적화 - 성능에 안좋다고 한다, 초기값이 0이므로 그냥 외부에서 관리해도 될 듯
				fboStack_.push(cur);
			}
		}
		void CGLState::PopState(std::string_view glState)
		{
			if (glState == "glst:fbo")
			{
				if (fboStack_.size() > 0)
				{
					fboStack_.pop();
				}
			}
		}
		void CGLState::TopState(std::string_view glState, std::any& out)
		{
			out = std::any();

			if (glState == "glst:fbo")
			{
				if (fboStack_.size() > 0)
					out = fboStack_.top();
				else
					out = 0;	// using back buffer
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CRenderTarget::CRenderTarget(glm::vec2 size, GLuint colorFormat, GLuint depthStencil, SRect2D viewportNDC, glm::vec3 clearColor)
		{
			size_ = size;
			colorFormat_ = colorFormat;
			depthStencilFormat_ = depthStencil;
			SetClearColor(clearColor);

			///////////////////////////////////////////////////////////////////////////////////////////////
			// ndc vertices
			glGenVertexArrays(1, &quadVAO_);
			glBindVertexArray(quadVAO_);

			glGenBuffers(1, &quadVBO_);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);

			viewportNDC_ = viewportNDC;

			const auto left = viewportNDC_.lb_.x, top = viewportNDC_.rt_.y, right = viewportNDC_.rt_.x, bottom = viewportNDC_.lb_.y;
			std::array<SVF_V2F_T2F, QuadVertexNumber> quadVertices =
			{				// positions   // texCoords
				SVF_V2F_T2F{ left, top, 0.0f, 1.0f, },	// 좌상
				SVF_V2F_T2F{ left, bottom, 0.0f, 0.0f, },	// 좌하
				SVF_V2F_T2F{ right, bottom, 1.0f, 0.0f, },	// 우하

				SVF_V2F_T2F{ right, top, 1.0f, 1.0f, },	// 우상
				SVF_V2F_T2F{ left, top,  0.0f, 1.0f, },		// 좌상
				SVF_V2F_T2F{ right, bottom, 1.0f, 0.0f	 }	// 우하
			};

			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SVF_V2F_T2F), (void*)offsetof(SVF_V2F_T2F, vertexX_));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SVF_V2F_T2F), (void*)offsetof(SVF_V2F_T2F, textureU_));
			glBindVertexArray(0);

			///////////////////////////////////////////////////////////////////////////////////////////////
			// frameBuffer
			if (size_ != glm::vec2(0.f, 0.f))
			{
				glGenFramebuffers(1, &fbo_);
				BindFrameBuffer();
				{
					glGenTextures(1, &colorTextureName_);
					glBindTexture(GL_TEXTURE_2D, colorTextureName_);
					glTexImage2D(GL_TEXTURE_2D, 0, colorFormat_, size_.x, size_.y, 0, colorFormat_, GL_UNSIGNED_BYTE, NULL);
					//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// 여기 - 리니어? 니얼스트? 생각해볼 것
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureName_, 0);

					if (depthStencilFormat_ != 0)
					{	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
						glGenRenderbuffers(1, &rbo_);
						glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
						glRenderbufferStorage(GL_RENDERBUFFER, depthStencilFormat_, size_.x, size_.y); // use a single renderbuffer object for both a depth AND stencil buffer.
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_); // now actually attach it
					}

					// now that we actually created the fbo_ and added all attachments we want to check if it is actually complete now
					assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
				}
				UnbindFrameBuffer();
			}
			else
			{
				// then it's using back buffer!
			}
		}

		bool CRenderTarget::IsUsingBackBuffer() const
		{
			return fbo_ == 0;
		}

		CRenderTarget::~CRenderTarget()
		{
			SAFE_DELETE_GL(Renderbuffers, 1, &rbo_);
			SAFE_DELETE_GL(Framebuffers, 1, &fbo_);
			SAFE_DELETE_GL(Buffers, 1, &quadVBO_);
			SAFE_DELETE_GL(VertexArrays, 1, &quadVAO_);
			SAFE_DELETE_GL(Textures, 1, &colorTextureName_);
		}
		
		CRenderTarget::CRenderTarget(CRenderTarget&& other) noexcept
		{
			operator=(std::move(other));
		}

		CRenderTarget& CRenderTarget::operator=(CRenderTarget&& other) noexcept
		{
			*this = other;	// 일단 복사로 넘기고

			// other 를 invalidate
			other.renderingCameraW_ = nullptr;
			other.quadVAO_ = 0;
			other.quadVBO_ = 0;
			other.fbo_ = 0;
			other.rbo_ = 0;
			other.colorTextureName_ = 0;

			return *this;
		}

		void CRenderTarget::BindFrameBuffer() const
		{
			//assert(fbo_ != 0);	// 일부러 - 0번인 백버퍼를 바인드해야할 때가 있으므로

			// 하면 안된다, 나중에 팝할 때 조건 체크가 어려워짐
			//std::any currentFbo;
			//CGLState::TopState("glst:fbo", currentFbo);
			//if (std::any_cast<GLint>(currentFbo) == fbo_)
			//	return;	// already

			CGLState::PushState("glst:fbo");
			glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
		}
		void CRenderTarget::UnbindFrameBuffer() const
		{
			//assert(fbo_ != 0);	// 일부러 - 0번인 백버퍼를 바인드해야할 때가 있으므로

			std::any prevFbo;
			CGLState::TopState("glst:fbo", prevFbo);

			CGLState::PopState("glst:fbo");
			glBindFramebuffer(GL_FRAMEBUFFER, std::any_cast<GLint>(prevFbo));
		}
		void CRenderTarget::ClearFrameBuffer(GLuint flags) const
		{
			glClearColor(clearColor_.r, clearColor_.g, clearColor_.b, 1.0f);

			if(flags == 0)
				glClear(GL_COLOR_BUFFER_BIT | (depthStencilFormat_ == 0 ? 0 : (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)));
			else
				glClear(flags);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CRenderingPass::CRenderingPass()
		{
		}

		CPostEffectPass::CPostEffectPass(std::wstring_view vsFilePath, std::wstring_view fsFilePath) : CRenderingPass()
		{
			auto peVert= SMGE::Globals::GetEngineAssetPath(vsFilePath.data()),
				peFrag = SMGE::Globals::GetEngineAssetPath(fsFilePath.data());

			posteffectShader_ = VertFragShaderSet::FindOrLoadShaderSet<VertFragShaderSet>(peVert, peFrag);
		}

		void CPostEffectPass::RenderTo(const glm::mat4& V, const glm::mat4& VP, CRenderTarget*& writeRT, CRenderTarget*& readRT)
		{
			posteffectShader_->Use();

			constexpr auto TexSampleIndex = 0;
			posteffectShader_->SetUniform_Mat4("V", V);
			posteffectShader_->SetUniform_Mat4("VP", VP);
			posteffectShader_->SetUniform_Int("readColorTexture", TexSampleIndex);	// 여기 - 현재 0번으로 고정

			writeRT->BindFrameBuffer();
			{
				//writeRT->SetClearColor(glm::vec3(1.f, 0.f, 0.f));	// 테스트 코드 - 한눈에 잘 보이라고
				writeRT->ClearFrameBuffer();	// 테스트 코드 ㅡ 현재 백버퍼로 오버라이트 하는 것만 구현되어있기 때문

				glBindVertexArray(writeRT->GetQuadVAO());
				glActiveTexture(GL_TEXTURE0 + TexSampleIndex);
				glBindTexture(GL_TEXTURE_2D, readRT->GetColorTextureName());
				glDrawArrays(GL_TRIANGLES, 0, writeRT->GetQuadVertexNumber());
				glBindVertexArray(0);
			}
			writeRT->UnbindFrameBuffer();

			// write 와 read 를 스왑해서 리턴한다 - for 핑퐁 렌더링
			std::swap<CRenderTarget*>(writeRT, readRT);
		}
	}
}
