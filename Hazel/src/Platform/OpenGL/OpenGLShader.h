#pragma once

#include "Hazel/Renderer/Shader.h"
#include <glm/glm.hpp>

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Hazel {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const std::string& GetName() const override { return m_Name; };

		void UpdloadUniformInt(const std::string& name, int value);

		void UpdloadUniformFloat(const std::string& name, float value);
		void UpdloadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UpdloadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UpdloadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UpdloadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UpdloadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};

}
