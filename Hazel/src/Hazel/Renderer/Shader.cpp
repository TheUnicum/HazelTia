#include "hzpch.h"
#include "Hazel/Renderer/Shader.h"

#include "Hazel/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/D3D11/D3D11Shader.h"


namespace Hazel {

	std::unordered_map<std::string, Ref<Shader>> Shader::_s_map;

	std::string Shader::GenerateUID(GraphicsContext& ctx, const std::string& filepath)
	{
		return ctx.GetAPI_TEXT() + "@:" + std::to_string(reinterpret_cast<uintptr_t>(&ctx)) +
			"|" + std::to_string(reinterpret_cast<uintptr_t>(&filepath));
	}

	std::string Shader::GenerateUID(GraphicsContext& ctx, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		return ctx.GetAPI_TEXT() + "@:" + std::to_string(reinterpret_cast<uintptr_t>(&ctx)) +
			"V" + std::to_string(vertexSrc.length()) + vertexSrc.substr(0, 10) +
			"F" + std::to_string(fragmentSrc.length()) + fragmentSrc.substr(0, 10);
	}

	// Classes Factory
	Ref<Shader> Shader::Create(const std::string& filepath) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), filepath); }
	Ref<Shader> Shader::Create(GraphicsContext& ctx, const std::string& filepath)
	{
		//CREATE_2(OpenGLShader, D3D11Shader, filepath);

		switch (ctx.GetAPI())
		{
			case API::None:    HZ_CORE_ASSERT(false, "Rendererctx::None is currently not supported!"); return nullptr;
			case API::OpenGL:  return CreateRef<OpenGLShader>(filepath);
			case API::D3D11:   return CreateRef<D3D11Shader>(filepath);
		}
	
		HZ_CORE_ASSERT(false, "Unknow Rendererctx!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), name, vertexSrc, fragmentSrc); }
	Ref<Shader> Shader::Create(GraphicsContext& ctx, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (ctx.GetAPI())
		{
			case API::None:    HZ_CORE_ASSERT(false, "Rendererctx::None is currently not supported!"); return nullptr;
			case API::OpenGL:  return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
			case API::D3D11:   return CreateRef<D3D11Shader>(name, vertexSrc, fragmentSrc);

		}

		HZ_CORE_ASSERT(false, "Unknow Rendererctx!");
		return nullptr;
	}

	///
	Ref<Shader> Shader::Resolve(const std::string& filepath, bool make_new_only) { return Resolve((GraphicsContext&)GraphicsContext::Get_Active(), filepath, make_new_only); }
	Ref<Shader> Shader::Resolve(GraphicsContext& ctx, const std::string& filepath, bool make_new_only)
	{
		const auto key = Shader::GenerateUID(ctx, filepath);
		const auto internal_context = _s_map.find(key);
		if (internal_context == _s_map.end())
		{
			Ref<Shader> new_context = Create(ctx, filepath);
			_s_map[key] = new_context;
			return new_context;
		}
		else
		{
			if (make_new_only)
				HZ_CORE_ASSERT(false, "CANT have duplicate of this CONTEXT!");
			return internal_context->second;
		}
	}



	// -------------------
	// -------------------
	// -------------------
	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		HZ_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		HZ_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name)
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}
