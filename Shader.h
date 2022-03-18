#pragma once

#include <string>
#include <unordered_map>

struct ShaderProgramSource
{  /*�ṹ�壬��������ɫ����Ƭ����ɫ��*/
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_FilePath;/*�ļ�·��*/
	unsigned int m_RendererID;/*shader��id*/
	std::unordered_map<std::string, int> m_UniformLocationCache;/*������uniform������һ�����У������ѯ*/
public:
	Shader(const std::string& filepath);/*����private�����ǰ��������*/
	~Shader();

	void Use() const;
	void Unbind() const;
	unsigned int GetRenderID() const;

	// uniforms ��װ������������ӣ�������GetUniformLocation
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	int GetUniformLocation(const std::string& name);
};