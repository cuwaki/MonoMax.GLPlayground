#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include "../common.h"

class CCamera
{
protected:
	glm::mat4 viewMatrix_;
	glm::mat4 projectionMatrix_;
	glm::vec3 worldPosition_ = glm::vec3(0, 0, 0);
	glm::vec3 cameraDir_, cameraRight_, cameraUp_;

	float horizontalAngle_ = 3.14f;
	float verticalAngle_ = 0.0f;
	float fov_ = 45.0f;
	float moveSpeed_ = 12.0f / 1000.f;
	float angleSpeed_ = 2.f / 1000.f;

	// runtime
	double lastProcessInputTime_ = 0;

public:
	CCamera();

	const glm::mat4& GetViewMatrix() { return viewMatrix_; }
	const glm::mat4& GetProjectionMatrix() { return projectionMatrix_; }

	void ComputeMatricesFromInputs(bool isInitialize, int windowWidth, int windowHeight);

	const glm::vec3& GetCameraPos();
	const glm::vec3& GetCameraDir();

	void SetCameraPos(const glm::vec3& worldPos);
	void SetCameraLookAt(const glm::vec3& lookAtWorldPos);

	void RotateCamera(const glm::vec2& moved);
	void MoveCamera(bool isLeft, bool isRight, bool isTop, bool isDown);
};

#endif