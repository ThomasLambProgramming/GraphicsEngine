#include <GL/glew.h>
#include "Application.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>
#include "Camera.h"
#include "Mesh.h"
#include "Windows.h"

#include <GLFW/glfw3native.h>

Application::Application()
{
}

void Application::InitApplication()
{
    CreateGlfwWindow();

    if (glewInit() != GLEW_OK)
        std::cout << "Glew did not init" << std::endl;
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    // displays the full version of opengl / the manufacturer (or person who did the implementation)
    std::cout << glGetString(GL_VERSION) << std::endl;
    // shows what is actually rendering the window (Intel hd graphics / 1080ti etc.)
    std::cout << glGetString(GL_RENDERER) << std::endl;
    
    SetupImgui();

    glEnable(GL_DEPTH_TEST);

    mainCamera = new Camera();
	mainCamera->id = 0;
	mainCamera->position = glm::vec3(230.0f,160.0f,165.0f);
    //Set Rotation to 0;
	mainCamera->phi = 220;
    mainCamera->theta = 20;

    testShader = Shader("D:/PersonalProjects/GraphicsEngine/OpenGL/shaders/simple.vert", "D:/PersonalProjects/GraphicsEngine/OpenGL/shaders/simple.frag", nullptr);
    testModel.LoadFile("D:/PersonalProjects/GraphicsEngine/SampleAssets/Models/Duck/glTF/duck.gltf");

    //Getting time to avoid 0 values;
    previousTime = glfwGetTime();
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,  // first Triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}

void Application::SetupImgui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    
}  

bool Application::CreateGlfwWindow()
{
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindow = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
    glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);  
    // if the window did not open terminate
    if (!glfwWindow)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1);
    
    return true;
}

void Application::RenderImgui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Render
    ImGui::Begin("Test");
    //ImGui::SliderFloat("Example", &floatToAdjust, -100, 100);
    ImGui::SliderFloat("Camera Turn Speed", &mainCamera->turnSpeed, 0.1, 100);
    ImGui::SliderFloat("Camera Movement Speed", &mainCamera->movementSpeed, 1, 1000);

    float cameraPosition[3];
    cameraPosition[0] = floor(mainCamera->position.x * 100) / 100;
    cameraPosition[1] = floor(mainCamera->position.y * 100) / 100;
    cameraPosition[2] = floor(mainCamera->position.z * 100) / 100;
    ImGui::SliderFloat3("Camera Position", cameraPosition, -1000, 1000);

    float cameraRotation[2];
    cameraRotation[0] = floor(mainCamera->theta * 100) / 100;
    cameraRotation[1] = floor(mainCamera->phi * 100) / 100;
    ImGui::SliderFloat2("Camera Rotation", cameraRotation, -1000, 1000);
    
    ImGui::End();
    
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::RenderLoop()
{
	glClearColor(213 / 255.0f,248 / 255.0f,255 / 255.0f,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Main Render Loop -------------------

    testShader.Use();
    glBindVertexArray(VAO);
    testShader.setVec4("triangleColor", 0,0,1,1 );
    testShader.setMat4("worldMatrix", glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    glm::mat4 viewMatrix = mainCamera->GetViewMatrix();
    testShader.setMat4("viewMatrix", viewMatrix);
    testShader.setMat4("projectionMatrix",  glm::perspective(glm::radians(45.0f), 1280 / 720.0f, 0.1f, 10000.0f));
    testModel.Draw(testShader);
    
    //End Main Render Loop -------------------

    RenderImgui();
    /* Swap front and back buffers */
    glfwSwapBuffers(glfwWindow);
    /* Poll for and process events */
    glfwPollEvents();
}

bool Application::UpdateLoop()
{
    while (!glfwWindowShouldClose(glfwWindow))
    {
        currentTime = glfwGetTime();
        deltaTime = (float)(currentTime - previousTime);
        previousTime = currentTime;


        mainCamera->Update(deltaTime, glfwWindow);
        RenderLoop();

        if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            break;
    }
    return true;
}

void Application::ShutDown()
{
    // Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
