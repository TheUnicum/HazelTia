#pragma once

#include "Hazel/Renderer/Shader.h"
#include <glm/glm.hpp>

namespace Hazel {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		void UpdloadUniformInt(const std::string& name, int value);

		void UpdloadUniformFloat(const std::string& name, float value);
		void UpdloadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UpdloadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UpdloadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UpdloadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UpdloadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		uint32_t m_RenderedID;
	};

}