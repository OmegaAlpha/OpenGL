cmake_minimum_required(VERSION 3.20)
project(OpenGLTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include FetchContent
include(FetchContent)

# Fetch dependencies
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8  # Latest stable as of 2025-03-30
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.2.0  # Latest as of 2025-03-30
)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8 # Latest as of 2025-03-30
)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b-docking # Latest as of 2025-03-30
)
# Declare the dependency with the patch
FetchContent_Declare(
    imguicolortextedit
    GIT_REPOSITORY https://github.com/BalazsJako/ImGuiColorTextEdit.git
    GIT_TAG 0a88824  # Latest as of 2025-03-30 but 6 years old at this point
	UPDATE_DISCONNECTED true # makes sure that the patch works on reruns, see https://gitlab.kitware.com/cmake/cmake/-/issues/21146
    PATCH_COMMAND git apply ${CMAKE_SOURCE_DIR}/patches/TextEditor.patch
)

FetchContent_Declare(
    imguifiledialog
    GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
    GIT_TAG v0.6.7 # Latest as of 2025-03-30
)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG f056911 # Latest as of 2025-03-30
)

# Make sure the dependencies are available
FetchContent_MakeAvailable(glfw glm glew imgui imguicolortextedit imguifiledialog stb)

# Find OpenGL
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
if(NOT OpenGL_FOUND)
    message(WARNING "OpenGL not found; this project requires OpenGL support.")
endif()

# Define imgui as a library
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${glfw_SOURCE_DIR}/include)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# ImGuiFileDialog is already defined , but we need to add imgui to it
target_link_libraries(ImGuiFileDialog PUBLIC imgui)

add_library(ImGuiColorTextEdit STATIC ${imguicolortextedit_SOURCE_DIR}/TextEditor.cpp)
target_include_directories(ImGuiColorTextEdit PUBLIC ${imguicolortextedit_SOURCE_DIR})
target_link_libraries(ImGuiColorTextEdit PUBLIC imgui)

# Define stb_image implementation
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
target_compile_definitions(stb INTERFACE STB_IMAGE_IMPLEMENTATION)

# Gather source files automatically
file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES src/*.h)

# Add executable
add_executable(OpenGLTest ${SRC_FILES} ${HEADER_FILES})

# Include directories
target_include_directories(OpenGLTest PRIVATE
    ${glfw_SOURCE_DIR}/include
    ${glew_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${imguicolortextedit_SOURCE_DIR}
    ${imguifiledialog_SOURCE_DIR}
    ${stb_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/tests
)

# Link libraries
target_link_libraries(OpenGLTest PRIVATE glfw OpenGL::GL libglew_static imgui ImGuiFileDialog ImGuiColorTextEdit stb)

# Post-build step to copy res/ directory to the configuration-specific output directory
add_custom_command(TARGET OpenGLTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res
    $<TARGET_FILE_DIR:OpenGLTest>/res
    COMMENT "Copying res/ directory to output"
)

# Enable warnings
if(MSVC)
# Set OpenGLTest as the startup project in Visual Studio
    target_compile_options(OpenGLTest PRIVATE /W4)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT OpenGLTest)
	set_target_properties(OpenGLTest PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:OpenGLTest>")
else()
    target_compile_options(OpenGLTest PRIVATE -Wall -Wextra -pedantic)
endif()