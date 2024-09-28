#include "Camera.h"

#include <iostream>

Camera::Camera()
{
	position = glm::vec3(0, 5, -10);
	phi = 0;
	theta = 0;
}

void Camera::Update(float a_deltaTime, GLFWwindow* glfwWindow)
{
	//dont let the camera do update function if stationary
	if (isStationary)
		return;

    if (currentFrame < frameWaitCount)
    {
        currentFrame++;
        return;
    }
	//get rotation in radians
	float thetaR = glm::radians(theta);
	float phiR = glm::radians(phi);
	
	//calculate the forwards and right axes and the up axis for the camera
	glm::vec3 forward(glm::cos(phiR) * glm::cos(thetaR), glm::sin(phiR), glm::cos(phiR) * glm::sin(thetaR));
	glm::vec3 right(-glm::sin(thetaR), 0, glm::cos(thetaR));
	glm::vec3 up(0, 1, 0);

	//Movement 
	#pragma region InputMovement
    if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += forward * a_deltaTime * movementSpeed;
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        position -= right * a_deltaTime * movementSpeed;
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        position -= forward * a_deltaTime * movementSpeed;
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        position += right * a_deltaTime * movementSpeed;
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        position += up * a_deltaTime * movementSpeed;
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        position -= up * a_deltaTime * movementSpeed;
    }

	//getting the mouse input for this frame 
	double mx = 0;//input->getMouseX();
	double my = 0;//input->getMouseY();
	glfwGetCursorPos(glfwWindow, &mx, &my);
	//if the right button is down, increment the theta and phi

    if (m_lastMouseX == FLT_MAX || m_lastMouseY == FLT_MAX)
    {
        std::cout << "Did max" << std::endl;
        m_lastMouseX = mx;
        m_lastMouseY = my;
    }
    else
    {
        theta += turnSpeed * (mx - m_lastMouseX) * a_deltaTime;
        phi -= turnSpeed * (my - m_lastMouseY) * a_deltaTime;
    }

    phi = glm::clamp<float>(phi, -89, 89);
    theta = glm::mod<float>(theta, 360);

	//now store the frames last values for the next
	m_lastMouseX = mx;
	m_lastMouseY = my;

	#pragma endregion 
}

glm::mat4 Camera::GetViewMatrix()
{
	//Get the view matrix to transform models to view space for rendering
	float thetaR = glm::radians(theta);
	float phiR = glm::radians(phi);
	forwardDirection = glm::vec3(glm::cos(phiR) * glm::cos(thetaR),
		glm::sin(phiR), glm::cos(phiR) * glm::sin(thetaR));

	
	return glm::lookAt(position, position + forwardDirection, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::GetProjectionMatrix(float a_width, float a_height)
{	
	//use the glm fuctions to calculate the projection matrix for transforming
	//into correct space
	return glm::perspective(glm::pi<float>() * 0.25f, a_width/ a_height, 0.1f, 1000.0f);
}

//make a transform with the position and a identity matrix
glm::mat4 Camera::MakeTransform()
{
	return glm::translate(glm::mat4(1), position);
}


