#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 u_MVP; // Transformation matrix

out vec3 vertexColor; // This will be interpolated

void main()
{
	// Apply transformation matrix to vertex position
    gl_Position = u_MVP * vec4(aPos, 1.0);
	vertexColor = aColor; // Pass color to fragment shader
}

#shader fragment
#version 330 core
in vec3 vertexColor; // Interpolated color from vertex shader
out vec4 FragColor;


void main(){
	FragColor = vec4(vertexColor, 1.0f);
}