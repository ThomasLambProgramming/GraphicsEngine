#include "Application.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>
#include <GL/glew.h>

void Application::InitApplication()
{
    CreateGlfwWindow();

    if (glewInit() != GLEW_OK)
        std::cout << "Glew did not init" << std::endl;
    
    SetupImgui();
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    // displays the full version of opengl / the manufacturer (or person who did the implementation)
    std::cout << glGetString(GL_VERSION) << std::endl;
    // shows what is actually rendering the window (Intel hd graphics / 1080ti etc.)
    std::cout << glGetString(GL_RENDERER) << std::endl;
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

bool Application::CreateGlfwWindow()
{
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindow = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);

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
      // Render
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::RenderLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //Do main render loop here.

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
