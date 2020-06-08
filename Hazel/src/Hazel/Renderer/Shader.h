#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#ifdef HZ_PLATFORM_WINDOWS
#include <d3d11.h>
#include <wrl.h>
#endif

enum class RendererAPI__API;

namespace Hazel {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		#ifdef HZ_PLATFORM_WINDOWS
		virtual Microsoft::WRL::ComPtr<ID3DBlob> GetpShaderBytecode() const { return pBlobStoredCompiledVertex; }
		#endif

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual const std::string& GetName() const = 0;

		// static
		static std::string GenerateUID(RendererAPI__API api, const std::string& filepath);
		static std::string GenerateUID(RendererAPI__API api, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		// Classes Factory
		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(RendererAPI__API api, const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(RendererAPI__API api, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		static Ref<Shader> Resolve(const std::string& filepath, bool make_new_only = false);
		static Ref<Shader> Resolve(RendererAPI__API api, const std::string& filepath, bool make_new_only = false);
	public:
		static std::unordered_map<std::string, Ref<Shader>> _s_map;

		#ifdef HZ_PLATFORM_WINDOWS
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobStoredCompiledVertex;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		#endif
	};




	class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;

	};

}
