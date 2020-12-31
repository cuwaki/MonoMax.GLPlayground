#include "controls.hpp"
#include "quaternion_utils.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;

namespace SMGE
{
	namespace nsRE
	{
		CRenderingCamera::CRenderingCamera()
		{
			cameraDir_ = { 0.f, 0.f, 1.f };
			cameraLeft_ = { 1.f, 0.f, 0.f };
			cameraUp_ = { 0.f, 1.f, 0.f };

			moveSpeed_ = 24.0f / 1000.f;
			angleSpeed_ = 4.f / 1000.f;

#ifdef CAMERA_QUATERNION
			orientation_ = glm::quat(0, 0, 0, 1);
			horizontalAngle_ = 0.f;
			verticalAngle_ = 0.f;
			angleSpeed_ = 0.2f;
#endif
		}

		void CRenderingCamera::ComputeMatricesFromInputs(bool isInitialize, int windowWidth, int windowHeight)
		{
			// Compute time difference between current and last frame
			double currentTime = 0;
			float deltaTime = 0;

			// Get mouse position
			double xMovedFromCenter = 0, yMovedFromCenter = 0;
			// Compute new orientation
			float xzAngle = horizontalAngle_, yzAngle = verticalAngle_;

#ifdef CAMERA_QUATERNION
			RotateCamera({ 0.f, 0.f });
#else
#if IS_EDITOR
#else
			if (isInitialize == false)
			{
				glfwGetCursorPos(m_window, &xMovedFromCenter, &yMovedFromCenter);
				// Reset mouse position for next frame
				glfwSetCursorPos(m_window, windowWidth / 2, windowHeight / 2);

				// glfwGetTime is called only once, the first time this function is called
				if (lastProcessInputTime_ == 0)
					lastProcessInputTime_ = glfwGetTime();

				currentTime = glfwGetTime();
				deltaTime = float(currentTime - lastProcessInputTime_);

				// 중점을 기준으로
				xzAngle += angleSpeed_ * float(windowWidth / 2 - xMovedFromCenter);	// y축 기준 회전량 - 요
				yzAngle += angleSpeed_ * float(windowHeight / 2 - yMovedFromCenter);	// x축 기준 회전량 - 피치
			}
#endif

			//glm::mat4 rotateMat(1);
			//rotateMat = glm::rotate(rotateMat, xzAngle, glm::vec3(0, 1, 0));
			//rotateMat = glm::rotate(rotateMat, yzAngle, glm::vec3(1, 0, 0));

			//glm::vec4 direction4(0, 0, 1, 0), right4(1, 0, 0, 0);
			//
			//glm::vec3 direction = glm::vec3(rotateMat * direction4);
			//glm::vec3 right = glm::vec3(rotateMat * right4);
			// 이렇게 하면 기본 상태에서는 되는데 좌우로 움직이고 나면 안된다 ㅠㅠㅠㅠ -> 이유는 right 벡터가 회전을 안했기 때문이다, rotateMat 를 다시 생각해봐라

			// Direction : Spherical coordinates to Cartesian coordinates conversion
			cameraDir_ = {
				cos(yzAngle) * sin(xzAngle),
				sin(yzAngle),
				cos(yzAngle) * cos(xzAngle)
			};

			cameraDir_ = glm::normalize(cameraDir_);

			// 정면 각도를 기반으로 항상 새롭게 만든다 - 왼쪽으로 만들기 위하여 -1 곱함
			cameraLeft_ = -1.f * glm::vec3(
				sin(xzAngle - 3.14f / 2.0f),
				0,
				cos(xzAngle - 3.14f / 2.0f)
			);

			horizontalAngle_ = xzAngle;
			verticalAngle_ = yzAngle;

			// Up vector
			cameraUp_ = glm::cross(cameraDir_, cameraLeft_);

			// Camera matrix
			viewMatrix_ = glm::lookAt(
				worldPosition_,           // Camera is here
				worldPosition_ + cameraDir_, // and looks here : at the same worldPosition_, plus "direction"
				cameraUp_                  // Head is up (set to 0,-1,0 to look upside-down)
			);
#endif

			float FoV = fov_;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

			// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
			projectionMatrix_ = glm::perspective(FoV, windowWidth / (float)windowHeight, zNear_, zFar_);

			//orthoProjectionMatrix_ = glm::ortho(windowWidth / -2.f, windowWidth / 2.f, windowHeight / -2.f, windowHeight / 2.f, 0.f, 500.f);
			//orthoProjectionMatrix_ = glm::ortho(-1.f, 1.f, -1.f, 1.f, 0.f, 500.f);

#if IS_EDITOR
#else
			if (isInitialize == false)
			{	// For the next frame, the "last time" will be "now"
				lastProcessInputTime_ = currentTime;
			}
#endif
		}

		void CRenderingCamera::RotateCamera(const glm::vec2& moved)
		{
			// 테스트 코드 - 이거 하드코딩이고 다른 데 또 있으니 검색해라
			constexpr float deltaTime = 1000.f / 60.f;

			// Compute new orientation
			float xzAngle = horizontalAngle_, yzAngle = verticalAngle_;

			xzAngle += angleSpeed_ * moved.x;	// y축 기준 회전량 - 요
			yzAngle += angleSpeed_ * moved.y;	// x축 기준 회전량 - 피치

#ifdef CAMERA_QUATERNION
			updateOrientation(xzAngle, yzAngle * -1.f);	// 화면 상하 조절을 반대로 하기 위하여 * -1

			glm::quat quatFront = orientation_ * glm::quat(0, 0, 0, 1) * glm::conjugate(orientation_);
			cameraDir_ = glm::vec3(quatFront.x, quatFront.y, quatFront.z);
			auto left = glm::cross({ 0, 1, 0 }, cameraDir_);
			if (glm::length2(left) < 0.0001f)
				left = glm::cross({ 0, 0, 1 }, cameraDir_);
			if (glm::length2(left) < 0.0001f)
				left = glm::cross({ 1, 0, 0 }, cameraDir_);
			cameraLeft_ = glm::normalize(left);
			cameraUp_ = glm::cross(cameraDir_, cameraLeft_);
#else
			cameraDir_ = {
				cos(yzAngle) * sin(xzAngle),
				sin(yzAngle),
				cos(yzAngle) * cos(xzAngle)
			};

			cameraDir_ = glm::normalize(cameraDir_);

			// 정면 각도를 기반으로 항상 새롭게 만든다 - 왼쪽으로 만들기 위하여 -1 곱함
			cameraLeft_ = -1.f * glm::vec3(
				sin(xzAngle - 3.14f / 2.0f),
				0,
				cos(xzAngle - 3.14f / 2.0f)
			);

			cameraUp_ = glm::cross(cameraDir_, cameraLeft_);
#endif
			horizontalAngle_ = xzAngle;
			verticalAngle_ = yzAngle;

			viewMatrix_ = glm::lookAt(
				worldPosition_,
				worldPosition_ + cameraDir_,
				cameraUp_);
		}

		void CRenderingCamera::MoveCamera(bool isLeft, bool isRight, bool isTop, bool isDown)
		{
			// 테스트 코드 - 이거 하드코딩이고 다른 데 또 있으니 검색해라
			constexpr float deltaTime = 1000.f / 60.f;

			if (isLeft)
				worldPosition_ += cameraLeft_ * deltaTime * moveSpeed_;
			if (isRight)
				worldPosition_ -= cameraLeft_ * deltaTime * moveSpeed_;
			if (isTop)
				worldPosition_ += cameraDir_ * deltaTime * moveSpeed_;
			if (isDown)
				worldPosition_ -= cameraDir_ * deltaTime * moveSpeed_;

			//// Move forward
			//if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			//	worldPosition_ += cameraDir_ * deltaTime * moveSpeed_;
			//}
			//// Move backward
			//if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			//	worldPosition_ -= cameraDir_ * deltaTime * moveSpeed_;
			//}
			//// Strafe right
			//if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			//	worldPosition_ += cameraLeft_ * deltaTime * moveSpeed_;
			//}
			//// Strafe left
			//if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			//	worldPosition_ -= cameraLeft_ * deltaTime * moveSpeed_;
			//}

			if (isLeft || isRight || isTop || isDown)
				SetCameraPos(worldPosition_);
		}

		void CRenderingCamera::SetFOV(float fov)
		{
			fov_ = fov;
		}

		void CRenderingCamera::SetZNearFar(float n, float f)
		{
			zNear_ = n;
			zFar_ = f;
		}

		const glm::vec3& CRenderingCamera::GetCameraPos() const
		{
			return worldPosition_;
		}

		void CRenderingCamera::SetCameraPos(const glm::vec3& worldPos)
		{
			worldPosition_ = worldPos;

			viewMatrix_ = glm::lookAt(
				worldPosition_,
				worldPosition_ + cameraDir_,
				cameraUp_);
		}

		const glm::vec3& CRenderingCamera::GetCameraFront() const
		{
			return cameraDir_;
		}
		const glm::vec3& CRenderingCamera::GetCameraLeft() const
		{
			return cameraLeft_;
		}
		const glm::vec3& CRenderingCamera::GetCameraUp() const
		{
			return cameraUp_;
		}

		void CRenderingCamera::SetCameraLeft(const glm::vec3& cr)
		{
			cameraLeft_ = glm::normalize(cr);
		}
		void CRenderingCamera::SetCameraUp(const glm::vec3& cu)
		{
			cameraUp_ = glm::normalize(cu);
		}

		void CRenderingCamera::SetCameraLookAt(const glm::vec3& lookAtWorldPos)
		{
#ifdef CAMERA_QUATERNION
			const auto dir = glm::normalize(lookAtWorldPos - worldPosition_);
			orientation_ = OpenGL_Tutorials::LookAt(dir, cameraUp_, { 0, 0, 1 }, { 0, 1, 0 });

			glm::quat quatFront = orientation_ * glm::quat(0, 0, 0, 1) * glm::conjugate(orientation_);
			cameraDir_ = glm::vec3(quatFront.x, quatFront.y, quatFront.z);
			auto left = glm::cross({ 0, 1, 0 }, cameraDir_);
			if (glm::length2(left) < 0.0001f)
				left = glm::cross({ 0, 0, 1 }, cameraDir_);
			if (glm::length2(left) < 0.0001f)
				left = glm::cross({ 1, 0, 0 }, cameraDir_);
			cameraLeft_ = glm::normalize(left);
			cameraUp_ = glm::cross(cameraDir_, cameraLeft_);

			viewMatrix_ = glm::lookAt(
				worldPosition_,
				worldPosition_ + cameraDir_,
				cameraUp_);
#else
			cameraDir_ = glm::normalize(lookAtWorldPos - worldPosition_);

			auto tempLeft = glm::normalize(glm::cross(cameraUp_, cameraDir_));
			SetCameraUp(glm::normalize(glm::cross(cameraDir_, tempLeft)));

			SetCameraLeft(glm::normalize(glm::cross(cameraUp_, cameraDir_)));

			viewMatrix_ = glm::lookAt(worldPosition_, worldPosition_ + cameraDir_, cameraUp_);
#endif
		}

#ifdef CAMERA_QUATERNION
		void CRenderingCamera::updateOrientation(float hAngl, float vAngl)
		{
			glm::quat yaw = glm::angleAxis(glm::radians(hAngl), glm::vec3(0.f, 1.f, 0.f));
			glm::quat pitch = glm::angleAxis(glm::radians(vAngl), glm::vec3(1.f, 0.f, 0.f));

			orientation_ = yaw * pitch;
		}
#endif
	}
}
