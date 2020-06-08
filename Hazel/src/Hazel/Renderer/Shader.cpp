#include "hzpch.h"
#include "Hazel/Renderer/Shader.h"

#include "Hazel/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/D3D11/D3D11Shader.h"

#include "Hazel/Renderer/GraphicsContext.h"


namespace Hazel {

	std::unordered_map<std::string, Ref<Shader>> Shader::_s_map;

	std::string Shader::GenerateUID(RendererAPI__API api, const std::string& filepath)
	{
		return "#Api-" + std::to_string((int)api) + "@:" + filepath;
	}

	std::string Shader::GenerateUID(RendererAPI__API api, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		return "#Api-" + std::to_string((int)api) + "@:" + name +
			"V" + std::to_string(vertexSrc.length()) + vertexSrc.substr(0, 10) +
			"F" + std::to_string(fragmentSrc.length()) + fragmentSrc.substr(0, 10);
	}

	// Classes Factory
	Ref<Shader> Shader::Create(const std::string& filepath) { return Create((RendererAPI__API)GraphicsContext::Get_API_Active(), filepath); }
	Ref<Shader> Shader::Create(RendererAPI__API api, const std::string& filepath)
	{
		switch ((RendererAPI::API)api)
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(filepath);
		case RendererAPI::API::D3D11:   return CreateRef<D3D11Shader>(filepath);
		}

		HZ_CORE_ASSERT(false, "Unknow RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) { return Create((RendererAPI__API)GraphicsContext::Get_API_Active(), name, vertexSrc, fragmentSrc); }
	Ref<Shader> Shader::Create(RendererAPI__API api, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch ((RendererAPI::API)api)
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		case RendererAPI::API::D3D11:   return CreateRef<D3D11Shader>(name, vertexSrc, fragmentSrc);

		}

		HZ_CORE_ASSERT(false, "Unknow RendererAPI!");
		return nullptr;
	}


	Ref<Shader> Shader::Resolve(const std::string& filepath, bool make_new_only) { return Resolve((RendererAPI__API)GraphicsContext::Get_API_Active(), filepath, make_new_only); }
	Ref<Shader> Shader::Resolve(RendererAPI__API api, const std::string& filepath, bool make_new_only)
	{
		const auto key = Shader::GenerateUID(api, filepath);
		const auto internal_context = _s_map.find(key);
		if (internal_context == _s_map.end())
		{
			Ref<Shader> new_context = Create(api, filepath);
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
