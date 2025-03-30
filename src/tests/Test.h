#pragma once

#include <iostream>
#include <functional>
#include "Renderer.h"
#include "MouseInput.h"

namespace test {

	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnWindowResize(int width, int height) {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
		virtual void OnMouseMove( float x, float y){}
		virtual void OnMouseEvent(const MouseInput& mouse) { mouseState = mouse; }

	protected:
		float m_MouseX = 0.0f, m_MouseY = 0.0f; // Mouse position in viewport
		MouseInput mouseState;  // Mouse click state and position
	};



	class TestMenu : public Test
	{
	public:
		TestMenu(Test*& currentTestPointer);

		void OnImGuiRender() override;
		void OnWindowResize(int width, int height) override;


		template<typename T>
		void RegisterTest(const std::string& name){
			std::cout << "Registering test " << name << std::endl;
			m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
		}

	private:
		Test*& m_CurrentTest;
		std::vector<std::pair<std::string, std::function<Test*()>>> m_Tests;

	};

}