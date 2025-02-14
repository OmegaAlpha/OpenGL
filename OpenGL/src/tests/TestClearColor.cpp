#include "Renderer.h"
#include "TestClearColor.h"

#include "imgui/imgui.h"

test::TestClearColor::TestClearColor()
	:m_ClearColor { 0.2f, 0.3f, 0.8f, 1.0f }{
}

test::TestClearColor::~TestClearColor(){
}

void test::TestClearColor::OnUpdate(float deltaTime){
}
void test::TestClearColor::OnWindowResize(int width, int height) {

}

void test::TestClearColor::OnRender(){
	GLCallV(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
	GLCallV(glClear(GL_COLOR_BUFFER_BIT));
}

void test::TestClearColor::OnImGuiRender(){
	ImGui::ColorEdit4("Clear Color", m_ClearColor);
}
