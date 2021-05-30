
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
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"


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
    window = glfwCreateWindow(800, 600, "OpenGL test application", NULL, NULL);
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
    // These are the vertex data we need to draw a rectangle from two triangles
        float positions[] = {
            -100.0f, -75.0f, 0.0f, 0.0f,// 0 positions, texture coords
             100.0f, -75.0f, 1.0f, 0.0f,// 1
             100.0f,  75.0f, 1.0f, 1.0f,// 2
            -100.0f,  75.0f, 0.0f, 1.0f,// 3
        };

        // These are the indices of our positions in the order we want to use to draw a rectangle
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCallV(glEnable(GL_BLEND));
        GLCallV(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // Vertex Array object
        VertexArray va;

        // Vertex buffer object
        VertexBuffer vb(positions, 4 * 4 * sizeof(float)); // 4 vertices with 4 floats per vertex

        // Creating the layout for our data
        VertexBufferLayout layout;
        layout.Push<float>(2); // Our data consists of 2 floats
        layout.Push<float>(2); // And 2 more floats
        va.AddBuffer(vb, layout);

        // Index buffer object
        IndexBuffer ib(indices, 6);

        //                          left  right  bottom  top   near   far
        glm::mat4 proj = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        // Shaders
        Shader shader("res/shader/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        Texture texture("res/textures/NIKE.png");
        texture.Bind(0);
        shader.SetUniform1i("u_Texture", 0);

        // Unbinding our buffers by clearing them
        va.Unbind();
        vb.UnBind();
        ib.UnBind();
        shader.Unbind();

        Renderer renderer;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        glm::vec3 translation(200, 200, 0); // Moving the quad

        const char* glsl_version = "#version 330";
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
            glm::mat4 mvp = proj * view * model; // Matrix multiplication in reverse order because of column major ordering in memory

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
            shader.SetUniformMat4f("u_MVP", mvp);

            va.Bind();
            ib.Bind();

            renderer.Draw(va, ib, shader);

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;


            { // Creating a window to modify the positon of our quad

                ImGui::Begin("Hello, world!");

                ImGui::SliderFloat2("Translation", &translation.x, 0.0f, 800.0f); 
              
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

    } // OpenGL comedy

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}