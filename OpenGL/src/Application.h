#include "Model.h"

#include <glfw/glfw3.h>

class Camera;

class Application
{
public:
    Application();
    void InitApplication();
    bool UpdateLoop();
    void ShutDown();
    
private:
    void SetupImgui();
    bool CreateGlfwWindow();

    void RenderImgui();
    void RenderLoop();
    
public:
    GLFWwindow* glfwWindow;
    
private:
    int windowWidth = 1280;
    int windowHeight = 720;
    const char* windowTitle = "Hello World";

    Camera* mainCamera;
    Model testModel;
    Shader testShader;

    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    float movementSpeed = 5;
    glm::mat4 worldMatrix;
    glm::vec3 offset;

    float deltaTime = 0;
    double previousTime = 0;
    double currentTime = 0;
};


