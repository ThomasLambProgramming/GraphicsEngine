#pragma once
#include "glm/ext.hpp"
#include <GLFW/glfw3.h>
#include <limits.h>

class Camera
{
public:
	Camera();
	~Camera() = default;

    void Update(float a_deltaTime, GLFWwindow* glfwWindow);

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix(float a_width, float a_height);
	glm::mat4 MakeTransform();
	glm::vec3 forwardDirection = { 0,0,1 };
    
    float turnSpeed = 10;
    float movementSpeed = 300;
	glm::vec3 position;

	float theta; //in degrees X 
	float phi; //in degrees Y

	int id;
	bool isStationary = false;
    
private:
	float m_lastMouseX, m_lastMouseY = FLT_MAX;

    
    //On init the mouse moves / gives bad input so we wait for approx 1 second before accepting rotation
    //input;
    int frameWaitCount = 60;
    int currentFrame = 0;
};
