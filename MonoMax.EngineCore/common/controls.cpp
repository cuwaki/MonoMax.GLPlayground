#include "controls.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

CCamera::CCamera()
{
}

void CCamera::ComputeMatricesFromInputs(bool isInitialize, int windowWidth, int windowHeight)
{
	// Compute time difference between current and last frame
	double currentTime = 0;
	float deltaTime = 0;

	// Get mouse position
	double xMovedFromCenter = 0, yMovedFromCenter = 0;
#if IS_EDITOR
#else
	if (isInitialize == false)
	{
		glfwGetCursorPos(m_window, &xMovedFromCenter, &yMovedFromCenter);
		// Reset mouse position for next frame
		glfwSetCursorPos(m_window, windowWidth / 2, windowHeight / 2);

		// glfwGetTime is called only once, the first time this function is called
		if(lastProcessInputTime_ == 0)
			lastProcessInputTime_ = glfwGetTime();

		currentTime = glfwGetTime();
		deltaTime = float(currentTime - lastProcessInputTime_);
	}
#endif

	// Compute new orientation
	float xzAngle = horizontalAngle_, yzAngle = verticalAngle_;

	// ������ ��������
	xzAngle += angleSpeed_ * float(windowWidth / 2 - xMovedFromCenter);	// y�� ���� ȸ���� - ��
	yzAngle += angleSpeed_ * float(windowHeight / 2 - yMovedFromCenter);	// x�� ���� ȸ���� - ��ġ

	//glm::mat4 rotateMat(1);
	//rotateMat = glm::rotate(rotateMat, xzAngle, glm::vec3(0, 1, 0));
	//rotateMat = glm::rotate(rotateMat, yzAngle, glm::vec3(1, 0, 0));

	//glm::vec4 direction4(0, 0, 1, 0), right4(1, 0, 0, 0);
	//
	//glm::vec3 direction = glm::vec3(rotateMat * direction4);
	//glm::vec3 right = glm::vec3(rotateMat * right4);
	// �̷��� �ϸ� �⺻ ���¿����� �Ǵµ� �¿�� �����̰� ���� �ȵȴ� �ФФФ� -> ������ right ���Ͱ� ȸ���� ���߱� �����̴�, rotateMat �� �ٽ� �����غ���

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	cameraDir_ = {
		cos(yzAngle) * sin(xzAngle),
		sin(yzAngle),
		cos(yzAngle) * cos(xzAngle)
	};

	// ���� ������ ������� �׻� ���Ӱ� �����
	// Right vector
	cameraRight_ = glm::vec3(
		sin(xzAngle - 3.14f / 2.0f),
		0,
		cos(xzAngle - 3.14f / 2.0f)
	);

	horizontalAngle_ = xzAngle;
	verticalAngle_ = yzAngle;

	// Up vector
	cameraUp_ = glm::cross(cameraRight_, cameraDir_);

#if IS_EDITOR

#else
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		worldPosition_ += cameraDir_ * deltaTime * moveSpeed_;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		worldPosition_ -= cameraDir_ * deltaTime * moveSpeed_;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		worldPosition_ += cameraRight_ * deltaTime * moveSpeed_;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		worldPosition_ -= cameraRight_ * deltaTime * moveSpeed_;
	}
#endif

	float FoV = fov_;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	projectionMatrix_ = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	viewMatrix_ = glm::lookAt(
		worldPosition_,           // Camera is here
		worldPosition_ + cameraDir_, // and looks here : at the same worldPosition_, plus "direction"
		cameraUp_                  // Head is up (set to 0,-1,0 to look upside-down)
	);

#if IS_EDITOR
#else
	if (isInitialize == false)
	{	// For the next frame, the "last time" will be "now"
		lastProcessInputTime_ = currentTime;
	}
#endif
}

void CCamera::SetCameraPos(const glm::vec3& worldPos)
{
	worldPosition_ = worldPos;
}

void CCamera::SetCameraLookAt(const glm::vec3& lookAtWorldPos)
{
	glm::vec3 oldDir = cameraDir_;
	cameraDir_ = lookAtWorldPos - worldPosition_;

	cameraRight_ = glm::normalize(glm::cross(oldDir, cameraDir_));
	cameraUp_ = glm::normalize(glm::cross(cameraDir_, cameraRight_));

	viewMatrix_ = glm::lookAt(
		worldPosition_,
		worldPosition_ + cameraDir_,
		cameraUp_);
}
