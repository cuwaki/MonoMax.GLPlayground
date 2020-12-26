#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include "../common.h"

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
			glm::vec3 cameraDir_, cameraRight_, cameraUp_;

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

			const glm::vec3& GetCameraDir() const;
			void SetCameraLookAt(const glm::vec3& lookAtWorldPos);

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