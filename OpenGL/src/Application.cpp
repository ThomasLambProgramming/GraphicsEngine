#include <GL/glew.h>
#include "Application.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>
#include "Camera.h"
#include "Mesh.h"

Application::Application()
{
}

void Application::InitApplication()
{
    CreateGlfwWindow();

    if (glewInit() != GLEW_OK)
        std::cout << "Glew did not init" << std::endl;
    
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    // displays the full version of opengl / the manufacturer (or person who did the implementation)
    std::cout << glGetString(GL_VERSION) << std::endl;
    // shows what is actually rendering the window (Intel hd graphics / 1080ti etc.)
    std::cout << glGetString(GL_RENDERER) << std::endl;
    
    SetupImgui();

    glEnable(GL_DEPTH_TEST);

    mainCamera = new Camera();
	mainCamera->SetID(0);
	mainCamera->SetStationary(true);
	mainCamera->SetPosition({ 0.0f,0.0f,-10.0f });
	mainCamera->SetRotation(0.0f, 0.0f);

    testShader = Shader("D:/PersonalProjects/GraphicsEngine/OpenGL/shaders/simple.vert", "D:/PersonalProjects/GraphicsEngine/OpenGL/shaders/simple.frag", nullptr);
    testModel.LoadFile("D:/PersonalProjects/GraphicsEngine/SampleAssets/Models/Duck/glTF/duck.gltf");

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

    ImGui::SliderFloat("X", &offset.x, -100, 100);
    ImGui::SliderFloat("Y", &offset.y, -100, 100);
    ImGui::SliderFloat("Z", &offset.z, -100, 100);
    
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
    testShader.setMat4("worldMatrix", glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    testShader.setMat4("viewMatrix", glm::translate(glm::mat4(1.0f), offset));
    testShader.setMat4("projectionMatrix",  glm::perspective(glm::radians(45.0f), 1280 / 720.0f, 0.1f, 100.0f));
    
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
        RenderLoop();
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
