
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
#include "MouseInput.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h" // DockBuilder
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "tests/TestTriangle.h"
#include "tests/TestShaderToy.h"


void ShowDockSpaces()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGuiWindowFlags dockspace_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Main Dockspace", nullptr, dockspace_flags);
    ImGui::PopStyleVar();

    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);

    static bool first_time = true;
    if (first_time)
    {
        first_time = false;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_left, dock_right, dock_bottom, dock_main;
        dock_main = dockspace_id;

        ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.2f, &dock_left, &dock_main);
        ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.3f, &dock_right, &dock_main);
        ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.25f, &dock_bottom, &dock_main);

        // Dock windows in correct areas
        ImGui::DockBuilderDockWindow("Test", dock_right);
        ImGui::DockBuilderDockWindow("Scene", dock_main);

        // Optional: Hide tab bar if only one window in the dock
        ImGui::DockBuilderGetNode(dock_main)->LocalFlags |= ImGuiDockNodeFlags_AutoHideTabBar;

        ImGui::DockBuilderFinish(dockspace_id);
    }

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

    int width = 1400;
    int height = 800;
    window = glfwCreateWindow(width, height, "OpenGL Test Application", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // 1 means v-sync on

    if (glewInit() != GLEW_OK)
        std::cout << "Failed to initialize GLEW!" << std::endl;

    // Printing the OpenGL version we have
    std::cout << glGetString(GL_VERSION) << std::endl;

    { //OpenGL comedy
   
        GLCallV(glEnable(GL_BLEND));
        GLCallV(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        Framebuffer framebuffer(1400, 800);  // Create an instance
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

        //glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

        testMenu->RegisterTest<test::TestClearColor>("Clear Color");
        testMenu->RegisterTest<test::TestTexture2D>("2D Texture");
        testMenu->RegisterTest<test::TestTriangle>("Triangle");
        testMenu->RegisterTest<test::TestShaderToy>("ShaderToy");

        const char* glsl_version = "#version 330";
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        float lastFrameTime = 0.0f;
        int frameCount = 0;

        MouseInput mouse;

        while (!glfwWindowShouldClose(window))
        {
            float currentTime = glfwGetTime();
            float deltaTime = currentTime -  lastFrameTime;
            lastFrameTime = currentTime;

            framebuffer.Bind();  // Render to framebuffe

            GLCallV(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            GLCallV(glClear(GL_COLOR_BUFFER_BIT));
            //renderer.Clear(); // Clearing color buffer and depth buffer

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ShowDockSpaces();  // Create the dockable areas

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
                currentTest->OnImGuiRender();
                ImGui::End();
            }

            framebuffer.Unbind();  // Back to default framebuffer

            // Render ImGui window
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Scene");
            // Get the starting position of the viewport (before drawing the image)
            ImVec2 viewportPanelPos = ImGui::GetCursorScreenPos();

            // Retrieve the available size inside the ImGui viewport
            ImVec2 viewportSize = ImGui::GetContentRegionAvail();

            // Update projection and viewport size based on the available region
            currentTest->OnWindowResize(viewportSize.x, viewportSize.y);
            framebuffer.Resize(viewportSize.x, viewportSize.y);
            GLCallV(glViewport(0, 0, (int)viewportSize.x, (int)viewportSize.y));

            // Draw the OpenGL rendered texture (the viewport image)
            ImGui::Image((ImTextureID)(intptr_t)framebuffer.GetTextureID(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));

            // Now, get the global mouse position
            ImVec2 mousePosGlobal = ImGui::GetIO().MousePos;

            // Calculate the mouse position relative to the viewport
            ImVec2 mousePosInViewport(mousePosGlobal.x - viewportPanelPos.x,
                mousePosGlobal.y - viewportPanelPos.y);

            // Check if the mouse is within the viewport bounds and print if it is
            if (mousePosInViewport.x >= 0 && mousePosInViewport.y >= 0 &&
                mousePosInViewport.x <= viewportSize.x && mousePosInViewport.y <= viewportSize.y)
            {
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                    mouse.leftPressed = ImGui::IsMouseDown(ImGuiMouseButton_Left);
                    mouse.rightPressed = ImGui::IsMouseDown(ImGuiMouseButton_Right);
                    mouse.x = mousePosInViewport.x;
                    mouse.y = mousePosInViewport.y;
                    if (currentTest) {
                        currentTest->OnMouseEvent(mouse);
                    }
                }
                if (currentTest) {
                     currentTest->OnMouseMove(mousePosInViewport.x, mousePosInViewport.y);
                }
                //printf("Mouse in viewport: (%.1f, %.1f)\n", mousePosInViewport.x, mousePosInViewport.y);
                
            }

            ImGui::End();
            ImGui::PopStyleVar();

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
            renderer.Clear(); // Fixes issue with docking and color lingering on edges of glfw_window

            glfwPollEvents();

            frameCount++;
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