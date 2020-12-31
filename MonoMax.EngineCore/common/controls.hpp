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
			friend CRenderingEngine;

		protected:
			// runtime
			double lastProcessInputTime_ = 0;

			glm::mat4 viewMatrix_;
			glm::mat4 projectionMatrix_, orthoProjectionMatrix_;
			glm::vec3 cameraDir_, cameraLeft_, cameraUp_;	// GL 오른손 좌표계에서 +z를 FRONT로 보면 +X는 LEFT 임, 그래서 LEFT라고 칭함

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
			float fov_ = 45.0f, zFar_ = 100.f, zNear_ = 1.f;

		public:
			CRenderingCamera();

			const glm::mat4& GetViewMatrix() const { return viewMatrix_; }
			const glm::mat4& GetProjectionMatrix() const { return projectionMatrix_; }
			const glm::mat4& GetOrthoProjectionMatrix() const { return orthoProjectionMatrix_; }

			const glm::vec3& GetCameraPos() const;
			void SetCameraPos(const glm::vec3& worldPos);

			const glm::vec3& GetCameraFront() const;
			void SetCameraLookAt(const glm::vec3& lookAtWorldPos);

			const glm::vec3& GetCameraLeft() const;
			void SetCameraLeft(const glm::vec3& cr);

			const glm::vec3& GetCameraUp() const;
			void SetCameraUp(const glm::vec3& cu);

			void SetFOV(float fov);
			float GetFOV() const { return fov_; }

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