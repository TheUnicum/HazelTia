#include "hzpch.h"
#include "Platform/VulkanC/VulkanShader.h"

#include <fstream>
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_GEOMETRY_SHADER 0x8DD9

#include "Shaderc/shaderc.hpp"

namespace Hazel {

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		HZ_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	VulkanShader::VulkanShader(const std::string& filepath)
		: Shader(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
	}

	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: Shader(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx), m_Name(name)
	{
	}

	VulkanShader::~VulkanShader()
	{
	}

	void VulkanShader::Cleanup()
	{
		for (auto& shader : shaderModules)
		{
			vkDestroyShaderModule(_c.GetDevice(), shader.second, nullptr);
		}
	}

	VkShaderModule& VulkanShader::GetVertexShaderModule()
	{
		return shaderModules[GL_VERTEX_SHADER];
	}

	VkShaderModule& VulkanShader::GetFragmentShaderModule()
	{
		return shaderModules[GL_FRAGMENT_SHADER];
	}

	std::string VulkanShader::ReadFile(const std::string& filepath)
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

	std::unordered_map<GLenum, std::string> VulkanShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLenght = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			//--HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLenght + 1; //Start of shader type name (after "#type" keyword)
			std::string type = source.substr(begin, eol - begin);
			//--HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified!");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			//--HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void VulkanShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		HZ_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shader for now!");

		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			shaderc::Compiler compiler;
			shaderc::CompileOptions options;

			//// Like -DMY_DEFINE=1
			//options.AddMacroDefinition("MY_DEFINE", "1");

			shaderc_shader_kind kind;
			if (type == GL_VERTEX_SHADER)
				kind = shaderc_glsl_default_vertex_shader;
			else
				kind = shaderc_glsl_default_fragment_shader;
			std::string name = "main";
			shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
				source.c_str(), source.size(), kind, name.c_str(), options);

			if (module.GetCompilationStatus() !=
				shaderc_compilation_status_success)
			{
				std::cout << module.GetErrorMessage();
			}

			std::vector<uint32_t> result(module.cbegin(), module.cend());
			shaderModules[type] = CreateShaderModule(result);
		}
	}

	VkShaderModule VulkanShader::CreateShaderModule(const std::vector<uint32_t>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size() * sizeof(uint32_t);
		createInfo.pCode = code.data();

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(_c.GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

}
