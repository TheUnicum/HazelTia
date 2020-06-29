#include "hzpch.h"
#include "Hazel/Renderer/ShaderCode.h"

#include "vulkan/vulkan.h"

#include <fstream>
#include "Shaderc/shaderc.hpp"
#include "spirv_cross\spirv_glsl.hpp"
#include <utility>

namespace Hazel {

	static ShaderCode::ShaderType ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return ShaderCode::ShaderType::VERTEX;
		else if (type == "geometry")
			return ShaderCode::ShaderType::GEOMETRY;
		else if (type == "fragment" || type == "pixel")
			return ShaderCode::ShaderType::FRAGMENT;

		HZ_CORE_ASSERT(false, "Unknown shader type!");
		return ShaderCode::ShaderType::UNDEFINED;
	}

	ShaderCode::ShaderCode(const std::string& filepath)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
	}


	// Classes Factory
	Ref<ShaderCode> ShaderCode::Create(const std::string & filepath)
	{
		return CreateRef<ShaderCode>(filepath);
		//MAKER_ON_ctx_args_1(OpenGLShader, D3D11Shader, filepath);
		//
		//switch (ctx.GetAPI())
		//{
		//	case API::None:    HZ_CORE_ASSERT(false, "Rendererctx::None is currently not supported!"); return nullptr;
		//	case API::OpenGL:  return CreateRef<OpenGLShader>(filepath);
		//	case API::D3D11:   return CreateRef<D3D11Shader>(filepath);
		//	case API::Vulkan:  return CreateRef<VulkanShader>(filepath);
		//}
		//HZ_CORE_ASSERT(false, "Unknow Rendererctx!");
		//return nullptr;
	}

	std::string ShaderCode::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary | std::ios::ate);
		if (in)
		{
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], result.size());
				in.close();
			}
			else
			{
				HZ_CORE_ERROR("Could not read from file '{0}'", filepath);
			}
		}
		else
		{
			HZ_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<ShaderCode::ShaderType, std::string> ShaderCode::PreProcess(const std::string& source)
	{
		std::unordered_map<ShaderCode::ShaderType, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLenght = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLenght + 1; //Start of shader type name (after "#type" keyword)
			std::string type = source.substr(begin, eol - begin);
			//--HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified!");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void ShaderCode::Compile(const std::unordered_map<ShaderType, std::string>& shaderSources)
	{
		HZ_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shader for now!");

		for (auto& kv : shaderSources)
		{
			ShaderCode::ShaderType type = kv.first;
			const std::string& source = kv.second;

			shaderc::Compiler compiler;
			shaderc::CompileOptions options;

			//// Like -DMY_DEFINE=1
			//options.AddMacroDefinition("MY_DEFINE", "1");

			shaderc_shader_kind kind;
			if (type == ShaderCode::ShaderType::VERTEX)
				kind = shaderc_glsl_default_vertex_shader;
			else if (type == ShaderCode::ShaderType::GEOMETRY)
				kind = shaderc_glsl_default_geometry_shader;
			else if(type == ShaderCode::ShaderType::FRAGMENT)
				kind = shaderc_glsl_default_fragment_shader;
			else
				HZ_CORE_ASSERT(false, "ShaderType Unknown!");

			std::string name = "main";
			shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
				source.c_str(), source.size(), kind, name.c_str(), options);

			if (module.GetCompilationStatus() !=
				shaderc_compilation_status_success)
			{
				std::cout << module.GetErrorMessage(); //<------------------
			}

			std::vector<uint32_t> result(module.cbegin(), module.cend());
			shaderCodePIRV[type] = result;
		}
	}

	std::string ShaderCode::glsl()
	{
			//spirv_cross::CompilerGLSL glsl(shaderCodePIRV[ShaderType::VERTEX]);
			//
			//// The SPIR-V is now parsed, and we can perform reflection on it.
			//spirv_cross::ShaderResources resources = glsl.get_shader_resources();
			//
			//// Get all sampled images in the shader.
			//for (auto& resource : resources.sampled_images)
			//{
			//	unsigned set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
			//	unsigned binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
			//	printf("Image %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);
			//
			//	// Modify the decoration to prepare it for GLSL.
			//	glsl.unset_decoration(resource.id, spv::DecorationDescriptorSet);
			//
			//	// Some arbitrary remapping if we want.
			//	glsl.set_decoration(resource.id, spv::DecorationBinding, set * 16 + binding);
			//}
			//
			//// Set some options.
			//spirv_cross::CompilerGLSL::Options options;
			//options.version = 310;
			//options.es = true;
			////glsl.set_options(options);
			//
			//// Compile to GLSL, ready to give to GL driver.
			//std::string source = glsl.compile();
		std::string source;
		return source;
	}



}
