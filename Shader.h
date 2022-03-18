#pragma once

#include <string>
#include <unordered_map>

struct ShaderProgramSource
{  /*结构体，代表顶点着色器和片段着色器*/
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_FilePath;/*文件路径*/
	unsigned int m_RendererID;/*shader的id*/
	std::unordered_map<std::string, int> m_UniformLocationCache;/*把所有uniform储存在一个表中，方便查询*/
public:
	Shader(const std::string& filepath);/*调用private里面的前三个函数*/
	~Shader();

	void Use() const;
	void Unbind() const;
	unsigned int GetRenderID() const;

	// uniforms 封装，可自行再添加，调用了GetUniformLocation
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	int GetUniformLocation(const std::string& name);
};