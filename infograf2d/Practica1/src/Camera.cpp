#include "Camera.h"



Camera::Camera()
{
}


Camera::~Camera()
{
}

Camera::Camera(vec3 position, vec3 direction, GLfloat sensitivity, GLfloat fov) {

	cameraPos = position;
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	vec3 cameraRight = glm::normalize(glm::cross(up, direction));
	cameraUp = glm::cross(direction, cameraRight);
	//camera thing
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	

	Deltatime = 0.0f;	// Time between current frame and last frame
	Lastframe = 0.0f;  	// Time of last frame
	
	YAW = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
	PITCH = 0.0f;
	FOV = fov;
	Sensitivity = sensitivity;
	LastMx = 800;
	LastMy = 600;

	firstMouse = true;
}
void Camera::DoMovement(GLFWwindow * window, GLint dir) {
	// Camera controls
	
	GLfloat cameraSpeed = 2.0f * Deltatime;
	if (dir == GLFW_KEY_W) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (dir == GLFW_KEY_S)
		cameraPos -= cameraSpeed * cameraFront;
	if (dir == GLFW_KEY_A)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (dir == GLFW_KEY_D)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (dir == GLFW_KEY_Q)
		cameraPos += cameraSpeed * cameraUp;
	if (dir == GLFW_KEY_E)
		cameraPos -= cameraSpeed * cameraUp;

}
void Camera::MouseMove(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouse)
	{
		LastMx = xpos;
		LastMy = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - LastMx;
	GLfloat yoffset = LastMy - ypos;
	LastMx = xpos;
	LastMy = ypos;

	xoffset *= Sensitivity;
	yoffset *= Sensitivity;

	YAW += xoffset;
	PITCH += yoffset;

	if (PITCH > 89.0f)
		PITCH = 89.0f;
	if (PITCH < -89.0f)
		PITCH = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(YAW)) * cos(glm::radians(PITCH));
	front.y = sin(glm::radians(PITCH));
	front.z = sin(glm::radians(YAW)) * cos(glm::radians(PITCH));
	cameraFront = glm::normalize(front);
}
void Camera::MouseScroll(GLFWwindow* window, double xScroll, double yScroll) {
	if (FOV >= 1.0f && FOV <= 180.0f)
		FOV -= yScroll;
	if (FOV <= 1.0f)
		FOV = 1.0f;
	if (FOV >= 180.0f)
		FOV = 180.0f;
}
mat4 Camera::LookAt() {
	//we calculate the deltatime here for no reason lol
	GLfloat currentFrame = glfwGetTime();
	Deltatime = currentFrame - Lastframe;
	Lastframe = currentFrame;
	mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return view;
}
GLfloat Camera::GetFOV() {
	return FOV;
}
vec3 Camera::GetPos() {
	return cameraPos;
}