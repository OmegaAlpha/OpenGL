#include "Shader.h"
#include "Renderer.h"

Shader::Shader(const std::string& filepath)
    :m_FilePath(filepath), m_RendererID(0)
{
    // Getting our shaders from file
    auto [vertexSource, fragmentSource] = ParseShader(filepath); // using structured bindings (C++ 17)
    // Creating our shader program
    m_RendererID = CreateShader(vertexSource, fragmentSource);
}

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
{
    m_RendererID = CreateShader(vertexSource, fragmentSource);
}

Shader::~Shader()
{
    GLCallV(glDeleteProgram(m_RendererID));
}

std::tuple<std::string, std::string> Shader::ParseShader(const std::string& filePath)
{
    std::string line;
    std::stringstream ss[2];

    std::ifstream stream(filePath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;

            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            if (type != ShaderType::NONE) {
                ss[(int)type] << line << '\n';
            }
        }

    }

    return { ss[0].str(), ss[1].str() }; // returning a tuple of the two sources
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = GLCall(glCreateShader(type));
    const char* src = source.c_str();
    GLCallV(glShaderSource(id, 1, &src, nullptr));
    GLCallV(glCompileShader(id));

    int result;
    GLCallV(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
        if (result == false)
        {
            int length;
            GLCallV(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
            char* message = (char*)alloca(length * sizeof(char));
            GLCallV(glGetShaderInfoLog(id, length, &length, message));
            std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
            std::cout << message << std::endl;

            GLCallV(glDeleteShader(id));
            return 0;
        }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = GLCall(glCreateProgram());
    unsigned int vs = GLCall(CompileShader(GL_VERTEX_SHADER, vertexShader));
    unsigned int fs = GLCall(CompileShader(GL_FRAGMENT_SHADER, fragmentShader));

    GLCallV(glAttachShader(program, vs));
    GLCallV(glAttachShader(program, fs));
    GLCallV(glLinkProgram(program));
    GLCallV(glValidateProgram(program));

    // We can delete our shaders after compilation
    GLCallV(glDeleteShader(vs));
    GLCallV(glDeleteShader(fs));

    return program;
}

void Shader::Bind() const
{
    GLCallV(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GLCallV(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCallV(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCallV(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
    GLCallV(glUniform2f(GetUniformLocation(name), v0, v1));
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    GLCallV(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float f0, float f1)
{
    GLCallV(glUniform4f(GetUniformLocation(name), v0, v1, f0, f1));
}

void Shader::SetUniformMat4f(const std::string& name, glm::mat4& matrix)
{
    GLCallV(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    int location = GLCall(glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
    
    m_UniformLocationCache[name] = location;
    return location;
}

