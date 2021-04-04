#include "controls.hpp"
#include "quaternion_utils.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;

namespace SMGE
{
	namespace nsRE
	{
		// 여기 - 상수들 통합 필요
		static constexpr glm::vec3 WorldZAxis(0, 0, 1);				// WorldZAxis() 와 같아야한다
		static constexpr glm::vec3 WorldYAxis(0, 1, 0);					// WorldYAxis() 와 같아야한다
		static constexpr glm::vec3 WorldXAxis(1, 0, 0);				// WorldXAxis() 와 같아야한다
		static constexpr float BoundCheckEpsilon = 0.0001f;	// 통합해라
		static constexpr size_t GL_LB = 0, GL_RB = 1, GL_RT = 2, GL_LT = 3;		// TransformConst::GL_LB 와 같아야한다

		inline bool IsNearlyEqual(float l, float r, float epsilon = /*Configs::BoundCheckEpsilon*/ 0.001f)	// 여기 - 통합 필요
		{
			return std::fabsf(l - r) < epsilon;
		}

		CRenderingCamera::CRenderingCamera()
		{
			cameraDir_ = WorldZAxis;
			cameraLeft_ = { 1.f, 0.f, 0.f };
			cameraUp_ = { 0.f, 1.f, 0.f };

			moveSpeed_ = 24.0f / 1000.f;
			angleSpeed_ = 4.f / 1000.f;

#ifdef CAMERA_QUATERNION
			orientation_ = glm::quat(0, WorldZAxis);
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
			//rotateMat = glm::rotate(rotateMat, xzAngle, WorldYAxis);
			//rotateMat = glm::rotate(rotateMat, yzAngle, WorldXAxis);

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

			//float FoV = fovDegrees_;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

			windowWidth_ = windowWidth;
			windowHeight_ = windowHeight;

			// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
			projectionMatrix_ = glm::perspective(fovDegrees_ / 2.f, windowWidth / (float)windowHeight, zNear_, zFar_);

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

		glm::vec3 CRenderingCamera::GetCameraFrontFromQuat(const glm::quat& orien) const
		{
			auto qf = orien * glm::quat(0, WorldZAxis) * glm::conjugate(orien);
			return glm::vec3(qf.x, qf.y, qf.z);
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
			SetCameraFront(GetCameraFrontFromQuat(orientation_));
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

		void CRenderingCamera::SetFOV(float fovDegrees)
		{
			fovDegrees_ = fovDegrees;
		}

		float CRenderingCamera::GetFOV_Horizontal() const
		{
			// https://community.khronos.org/t/how-to-get-the-horizontal-fov/58010
			const auto aspect = windowWidth_ / windowHeight_;
			return glm::degrees(std::atanf(aspect * std::tan(glm::radians(fovDegrees_ / 2.f))));
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
			SetCameraFront(dir);
#else
			cameraDir_ = glm::normalize(lookAtWorldPos - worldPosition_);

			auto tempLeft = glm::normalize(glm::cross(cameraUp_, cameraDir_));
			SetCameraUp(glm::normalize(glm::cross(cameraDir_, tempLeft)));

			SetCameraLeft(glm::normalize(glm::cross(cameraUp_, cameraDir_)));

			viewMatrix_ = glm::lookAt(worldPosition_, worldPosition_ + cameraDir_, cameraUp_);
#endif
		}

		void CRenderingCamera::SetCameraFront(const glm::vec3& dir)
		{
#if defined(_DEBUG) || defined(DEBUG)
			assert(IsNearlyEqual(glm::length(dir), 1.f) && "must be normalized!");
#endif
			cameraDir_ = dir;

			auto left = glm::cross(WorldYAxis, cameraDir_);
			if (glm::length2(left) < BoundCheckEpsilon)
				left = glm::cross(WorldZAxis, cameraDir_);
			if (glm::length2(left) < BoundCheckEpsilon)
				left = glm::cross(WorldXAxis, cameraDir_);
			cameraLeft_ = glm::normalize(left);
			cameraUp_ = glm::cross(cameraDir_, cameraLeft_);

			viewMatrix_ = glm::lookAt(
				worldPosition_,
				worldPosition_ + cameraDir_,
				cameraUp_);
		}

#ifdef CAMERA_QUATERNION
		void CRenderingCamera::updateOrientation(float hAngl, float vAngl)
		{
			glm::quat yaw = glm::angleAxis(glm::radians(hAngl), WorldYAxis);
			glm::quat pitch = glm::angleAxis(glm::radians(vAngl), WorldXAxis);

			orientation_ = yaw * pitch;
		}
#endif

		// license - https://gist.github.com/terryjsmith/7196d8c099e1478ea276bd751925a2f1
		CRenderingCamera::SFrustum CRenderingCamera::CalculateFrustumWorld(float fovDegrees, float n, float f) const
		{
			if (IsNearlyEqual(fovDegrees, 0.f))
				fovDegrees = fovDegrees_;
			if (IsNearlyEqual(n, 0.f))
				n = zNear_;
			if (IsNearlyEqual(f, 0.f))
				f = zFar_;

			const float apectR = windowWidth_ / windowHeight_;
			const float tanFov = std::tanf(glm::radians(fovDegrees / 2.f));

			const float nearHalfHeight = tanFov * n;
			const float nearHalfWidth = nearHalfHeight * apectR;

			const float farHalfHeight = tanFov * f;
			const float farHalfWidth = farHalfHeight * apectR;

			const auto nearCenter = worldPosition_ + (cameraDir_ * n);
			const auto farCenter = worldPosition_ + (cameraDir_ * f);

			SFrustum returnFrustum;
			returnFrustum.center_ = worldPosition_ + (cameraDir_ * n) + (cameraDir_ * ((f - n) / 2.0f));

			const auto ncuhh = cameraUp_ * nearHalfHeight;
			const auto nclhw = cameraLeft_ * nearHalfWidth;

			// 카메라가 +z 를 보고 있을 때를 기준으로 평면 점들을 반시계로 내보낸다
			returnFrustum.nearPlane_[GL_LB] = nearCenter - ncuhh - nclhw;	// lb
			returnFrustum.nearPlane_[GL_RB] = nearCenter - ncuhh + nclhw;	// rb
			returnFrustum.nearPlane_[GL_RT] = nearCenter + ncuhh + nclhw;	// rt
			returnFrustum.nearPlane_[GL_LT] = nearCenter + ncuhh - nclhw;	// lt

			const auto fcuhh = cameraUp_ * farHalfHeight;
			const auto fclhw = cameraLeft_ * farHalfWidth;

			returnFrustum.farPlane_[GL_LB] = farCenter - fcuhh - fclhw;
			returnFrustum.farPlane_[GL_RB] = farCenter - fcuhh + fclhw;
			returnFrustum.farPlane_[GL_RT] = farCenter + fcuhh + fclhw;
			returnFrustum.farPlane_[GL_LT] = farCenter + fcuhh - fclhw;

			return returnFrustum;
		}

		CRenderingCamera::SFrustum CRenderingCamera::CalculateFrustumModel(float fovDegrees, float n, float f) const
		{
			if (IsNearlyEqual(fovDegrees, 0.f))
				fovDegrees = fovDegrees_;
			if (IsNearlyEqual(n, 0.f))
				n = zNear_;
			if (IsNearlyEqual(f, 0.f))
				f = zFar_;

			const float apectR = windowWidth_ / windowHeight_;
			const float tanFov = std::tanf(glm::radians(fovDegrees / 2.f));

			const float nearHalfHeight = tanFov * n;
			const float nearHalfWidth = nearHalfHeight * apectR;

			const float farHalfHeight = tanFov * f;
			const float farHalfWidth = farHalfHeight * apectR;

			const auto nearCenter = (WorldZAxis * n);
			const auto farCenter = (WorldZAxis * f);

			SFrustum returnFrustum;
			returnFrustum.center_ = (WorldZAxis * n) + (WorldZAxis * ((f - n) / 2.0f));

			const auto ncuhh = WorldYAxis * nearHalfHeight;
			const auto nclhw = WorldXAxis * nearHalfWidth;

			// 카메라가 +z 를 보고 있을 때를 기준으로 평면 점들을 GL좌표계 기준으로 반시계로 내보낸다
			returnFrustum.nearPlane_[GL_LB] = nearCenter - ncuhh - nclhw;
			returnFrustum.nearPlane_[GL_RB] = nearCenter - ncuhh + nclhw;
			returnFrustum.nearPlane_[GL_RT] = nearCenter + ncuhh + nclhw;
			returnFrustum.nearPlane_[GL_LT] = nearCenter + ncuhh - nclhw;

			const auto fcuhh = WorldYAxis * farHalfHeight;
			const auto fclhw = WorldXAxis * farHalfWidth;

			returnFrustum.farPlane_[GL_LB] = farCenter - fcuhh - fclhw;
			returnFrustum.farPlane_[GL_RB] = farCenter - fcuhh + fclhw;
			returnFrustum.farPlane_[GL_RT] = farCenter + fcuhh + fclhw;
			returnFrustum.farPlane_[GL_LT] = farCenter + fcuhh - fclhw;

			return returnFrustum;
		}
	}
}
