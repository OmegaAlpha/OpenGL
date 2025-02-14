
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <tuple>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h" // DockBuilder
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "tests/TestTriangle.h"

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    test::Test* currentTest = static_cast<test::Test*>(glfwGetWindowUserPointer(window));
    if (currentTest) {
        currentTest->OnWindowResize(width, height);
    }
}

void ShowMainDockSpace(){
    ImGuiWindowFlags window_flags =  ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::Begin("DockSpace Window", nullptr, window_flags);
    ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");

    ImGui::DockSpace(dockspace_id, ImVec2(0, 0),
        ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //Set the GLFW window creation hints - only modern OpenGL (will use compatibility profile if not set)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version  
    glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing  
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  

    /* Create a windowed mode window and its OpenGL context */

    int width = 800;
    int height = 600;
    window = glfwCreateWindow(width, height, "OpenGL Test Application", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(0); // 1 means v-sync on

    if (glewInit() != GLEW_OK)
        std::cout << "Failed to initialize GLEW!" << std::endl;

    // Printing the OpenGL version we have
    std::cout << glGetString(GL_VERSION) << std::endl;

    { //OpenGL comedy
   
        GLCallV(glEnable(GL_BLEND));
        GLCallV(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        Renderer renderer;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable multi-viewport
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // (Optional) Enable docking
        io.ConfigViewportsNoDecoration = true; //Ingen vanlig Windows chrome

        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = new test::TestMenu(currentTest);
        currentTest = testMenu;

        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

        testMenu->RegisterTest<test::TestClearColor>("Clear Color");
        testMenu->RegisterTest<test::TestTexture2D>("2D Texture");
        testMenu->RegisterTest<test::TestTriangle>("Triangle");

        const char* glsl_version = "#version 330";
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        float lastFrameTime = 0.0f;

        while (!glfwWindowShouldClose(window))
        {
            float currentTime = glfwGetTime();
            float deltaTime = currentTime -  lastFrameTime;
            lastFrameTime = currentTime;

            GLCallV(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            renderer.Clear();


            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ShowMainDockSpace();  // Create the dockable areas

            if (currentTest)
            {
                currentTest->OnUpdate(deltaTime);
                currentTest->OnRender();
                ImGui::Begin("Test");
                if (currentTest != testMenu && ImGui::Button("<-"))
                {
                    delete currentTest;
                    currentTest = testMenu;
                }
                glfwSetWindowUserPointer(window, currentTest); // setter en pointer til den testen vi er på
                currentTest->OnImGuiRender();
                ImGui::End();
            }
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Handle multiple viewports
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
            glfwSwapBuffers(window);

            glfwPollEvents();
        }

        if (currentTest != testMenu)
            delete testMenu;
        delete currentTest;

    } // OpenGL comedy

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}