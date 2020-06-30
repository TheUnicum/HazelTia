#pragma once

#include "Hazel/Renderer/Shader.h"
#include "Platform/Vulkan/VulkanContext.h"

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Hazel {

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& filepath);
		VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		VulkanShader(const Ref<ShaderCode> shaderCode);

		virtual ~VulkanShader();
		void Cleanup();

		virtual void Bind() const override {};
		virtual void Unbind() const override {};

		VkShaderModule& GetVertexShaderModule();
		VkShaderModule& GetFragmentShaderModule();
		
		//virtual Microsoft::WRL::ComPtr<ID3DBlob> GetpShaderBytecode() const override { return pBlobStoredCompiledVertex; }

		virtual void SetInt(const std::string& name, int value) override {};
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override {};
		virtual void SetFloat(const std::string& name, float value) override {};
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override {};
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override {};
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override {};

		virtual const std::string& GetName() const override { return m_Name; };

		//void UploadUniformInt(const std::string& name, int value);
		//void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		//
		//void UploadUniformFloat(const std::string& name, float value);
		//void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		//void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		//void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		//
		//void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		//void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		VkShaderModule CreateShaderModule(const std::vector<uint32_t>& code);

	private:
		std::string m_Name;
		std::unordered_map<GLenum, VkShaderModule> shaderModules;
	public:
		VulkanContext& _c;
	};

}
