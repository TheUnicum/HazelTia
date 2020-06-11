#pragma once

#include <string>

#include <glm/glm.hpp>

#ifdef HZ_PLATFORM_WINDOWS
	#include <d3d11.h>
	#include <wrl.h>
#endif

#include "Hazel/Renderer/Bindable.h"
#include "Hazel/Renderer/GraphicsContext.h"

class OpenGLShader;
class D3D11Shader;

namespace Hazel {

	class Shader : public Bindable
	{
	public:
		Shader(GraphicsContext& ctx) : Bindable(ctx) {}
		virtual ~Shader() = default;

		virtual uint32_t GetID() const { HZ_CORE_ASSERT(false,"Function not implememted!"); return -1; };

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual const std::string& GetName() const = 0;

		// static
		static std::string GenerateUID(GraphicsContext& ctx, const std::string& filepath);
		static std::string GenerateUID(GraphicsContext& ctx, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		// Classes Factory
		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(GraphicsContext& ctx, const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(GraphicsContext& ctx, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		// Shader Resolver
		static Ref<Shader> Resolve(const std::string& filepath)
		{
			return std::static_pointer_cast<Shader>(Bindable::ResolvePctx<Shader>(filepath));
		}
		static Ref<Shader> Resolve(GraphicsContext& ctx, const std::string& filepath)
		{
			return std::static_pointer_cast<Shader>(Bindable::_Resolve<Shader>(ctx, filepath));
		}
	public:
		static std::unordered_map<std::string, Ref<Shader>> _s_map;
	#ifdef HZ_PLATFORM_WINDOWS 
		// Temporary solurion. 
		virtual Microsoft::WRL::ComPtr<ID3DBlob> GetpShaderBytecode() const { return pBlobStoredCompiledVertex; }
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
