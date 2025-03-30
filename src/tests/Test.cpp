#include "Test.h"
#include "Renderer.h"
#include "imgui.h"

namespace test {

	TestMenu::TestMenu(Test*& currentTestPointer)
		:m_CurrentTest(currentTestPointer){
	}

	void TestMenu::OnImGuiRender(){
		for (auto& test : m_Tests){
			if(ImGui::Button(test.first.c_str()))
				m_CurrentTest = test.second();
		}
	}

	void TestMenu::OnWindowResize(int width, int height) {
		GLCallV(glViewport(0, 0, width, height)); // Ensure OpenGL viewport is updated
	}



}