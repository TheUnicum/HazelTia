#pragma once

#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/GraphicsContext.h"
#include "Platform/D3D11/D3D11Context.h"

#include <glm/glm.hpp>

// TODO: REMOVE!
typedef unsigned int GLenum;

#include <d3d11.h>
#include <wrl.h>


namespace Hazel {

	class D3D11Shader : public Shader
	{
	public:
		D3D11Shader(const std::string& filepath);
		D3D11Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~D3D11Shader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

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
	private:
		uint32_t m_RendererID;
		std::string m_Name;
	public:
		D3D11Context& _c;

		//Microsoft::WRL::ComPtr<ID3DBlob> pBlobStoredCompiledVertex;
		//Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		//Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	};

}
