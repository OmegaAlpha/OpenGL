#pragma once

#include "Test.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include <memory>

namespace test {

	class TestTriangle : public Test
	{
	public:
		TestTriangle();
		~TestTriangle();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnWindowResize(int width, int height) override;
		void UpdateProjectionMatrix();
	private:
		std::unique_ptr <VertexArray> m_VAO;
		std::unique_ptr <VertexBuffer> m_VBO;
		std::unique_ptr <IndexBuffer> m_IBO;
		std::unique_ptr <Shader> m_Shader;
		std::unique_ptr <Texture> m_Texture;

		glm::mat4 m_Proj, m_View;
		glm::vec3 m_Translation; //Burde hete noe annet

		bool m_Spinning;
		bool m_Moving;
		float m_Rotation;
		float m_Scale;
		float m_MoveAngle;
		float m_MoveRadius;
		glm::vec3 m_Position;

		int m_WindowWidth, m_WindowHeight;

	};
}