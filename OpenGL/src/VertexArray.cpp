#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCallV(glGenVertexArrays(1, &m_RendererID));
}
VertexArray::~VertexArray()
{
	GLCallV(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();

	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		//Enabling the i'th index
		GLCallV(glEnableVertexAttribArray(i));
		// Linking the vertex buffer with the currently bound vao
		GLCallV(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset)); 
		offset += element.count * VertexbufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	GLCallV(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCallV(glBindVertexArray(0));
}
