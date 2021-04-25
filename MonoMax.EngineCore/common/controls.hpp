#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include "../common.h"

#define CAMERA_QUATERNION
// 쿼터니언 관련 문서
// http://chanhaeng.blogspot.com/2018/09/quaternion-camera-implementation.html
// http://graphics.stanford.edu/courses/cs348a-17-winter/Papers/quaternion.pdf

namespace SMGE
{
	namespace nsRE
	{
		class CRenderingEngine;

		class CRenderingCamera
		{
		public:
			struct SFrustum
			{
				glm::vec3 center_;
				glm::vec3 nearPlanePoints_[4];
				glm::vec3 farPlanePoints_[4];
			};

			friend CRenderingEngine;

		protected:
			// runtime
			double lastProcessInputTime_ = 0;

			glm::mat4 viewMatrix_;
			glm::mat4 projectionMatrix_, orthoProjectionMatrix_;
			glm::vec3 cameraDir_, cameraLeft_, cameraUp_;	// 왜 Left 냐면 오른손 좌표계 기준으로 +Z가 앞이므로 +X는 Left 가 되기 때문이다

#ifdef CAMERA_QUATERNION
			glm::quat orientation_;
			void updateOrientation(float hAngl, float vAngl);
#endif

			float horizontalAngle_ = 3.14f;
			float verticalAngle_ = 0.0f;
			float moveSpeed_ = 12.0f / 1000.f;
			float angleSpeed_ = 2.f / 1000.f;

		protected:
			glm::vec3 worldPosition_ = glm::vec3(0, 0, 0);
			float fovDegrees_ = 90.0f, zFar_ = 100.f, zNear_ = 1.f;
			float windowWidth_, windowHeight_;

		public:
			CRenderingCamera();

			const glm::mat4& GetViewMatrix() const { return viewMatrix_; }
			const glm::mat4& GetProjectionMatrix() const { return projectionMatrix_; }
			const glm::mat4& GetOrthoProjectionMatrix() const { return orthoProjectionMatrix_; }
			
			SFrustum CalculateFrustumWorld(float fovDegrees = 0.f, float n = 0.f, float f = 0.f) const;
			SFrustum CalculateFrustumModel(float fovDegrees = 0.f, float n = 0.f, float f = 0.f) const;

			const glm::vec3& GetCameraPos() const;
			void SetCameraPos(const glm::vec3& worldPos);

			const glm::vec3& GetCameraFront() const;
			void SetCameraLookAt(const glm::vec3& lookAtWorldPos);

			const glm::vec3& GetCameraLeft() const;
			void SetCameraLeft(const glm::vec3& cr);

			const glm::vec3& GetCameraUp() const;
			void SetCameraUp(const glm::vec3& cu);

			glm::vec3 GetCameraFrontFromQuat(const glm::quat& orien) const;
			void SetCameraFront(const glm::vec3& dir);

			void SetFOV(float fovDegrees);
			float GetFOV() const { return fovDegrees_; }
			float GetFOV_Horizontal() const;

			void SetZNearFar(float n, float f);
			float GetZFar() const { return zFar_; }
			float GetZNear() const { return zNear_; }

			void RotateCamera(const glm::vec2& moved);
			void MoveCamera(bool isLeft, bool isRight, bool isTop, bool isDown);

		protected:
			void ComputeMatricesFromInputs(bool isInitialize, int windowWidth, int windowHeight);
		};
	}
}

#endif