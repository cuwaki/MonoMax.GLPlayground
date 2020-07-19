#include "../common.h"

// Include GLFW
//#include <glfw3.h>
//extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3(0, 0, 5);
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

void computeMatricesFromInputs(GLFWwindow *window)
{
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Compute new orientation
	float xzAngle = horizontalAngle, yzAngle = verticalAngle;

	xzAngle += mouseSpeed * float(1024 / 2 - xpos);
	yzAngle += mouseSpeed * float(768 / 2 - ypos);

	//glm::mat4 rotateMat(1);
	//rotateMat = glm::rotate(rotateMat, xzAngle, glm::vec3(0, 1, 0));
	//rotateMat = glm::rotate(rotateMat, yzAngle, glm::vec3(1, 0, 0));

	//glm::vec4 direction4(0, 0, 1, 0), right4(1, 0, 0, 0);
	//
	//glm::vec3 direction = glm::vec3(rotateMat * direction4);
	//glm::vec3 right = glm::vec3(rotateMat * right4);
	// 이렇게 하면 기본 상태에서는 되는데 좌우로 움직이고 나면 안된다 ㅠㅠㅠㅠ -> 이유는 right 벡터가 회전을 안했기 때문이다, rotateMat 를 다시 생각해봐라

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(yzAngle) * sin(xzAngle),
		sin(yzAngle),
		cos(yzAngle) * cos(xzAngle)
	);

	// 정면 각도를 기반으로 항상 새롭게 만든다
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(xzAngle - 3.14f / 2.0f),
		0,
		cos(xzAngle - 3.14f / 2.0f)
	);

	horizontalAngle = xzAngle;
	verticalAngle = yzAngle;

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}