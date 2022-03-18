#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream> // stringstream
#include "Renderer.h"

Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{/*通过ParseShader获取文件内容，再通过CreateShader创建shader*/
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{//用完自动删除shader
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Use () const
{//使用shader
    GLCall(glUseProgram(m_RendererID));

}

void Shader::Unbind() const
{//解绑shader
    GLCall(glUseProgram(0));
}


unsigned int Shader::GetRenderID() const
{//返回shaderid
    return m_RendererID;
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);//获取文件内容

    enum class ShaderType//设定三种模式
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];//存放文件内容
    ShaderType type = ShaderType::NONE;
    while (std::getline(stream, line))//读取字符内容
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;//当找到vertex，转换状态
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;//当找到fragment，转换状态
            }
        }
        else
        {
            // 其余情况，根据状态分别存放vertex和fragment的字符内容
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{/*通过CompileShader创建vs和fs，再与program链接*/
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	// 查阅文档我们知道，验证的状态会被存储为程序对象状态的一部分你可以调用，比如用 glGetProgram 查询实际结果是什么之类的
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) // 第二个参数是因为 GLenum 是 unsigned int 的
{   /*根据类型来创建shader*/
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); // 返回的是一个指向 string 内部的指针，因此这个 string 必须存在才有效
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // iv: integer, vector
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        /*检查是否创建成功*/
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// 从文件中获取，然后编译、链接，生成buffer id，用于之后的绑定


void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

int Shader::GetUniformLocation(const std::string& name)
{   /*在表中存放uniform*/
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
    {
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
    }
    m_UniformLocationCache[name] = location;
	return location;
}
