#include "hzpch.h"
#include "Hazel/Renderer/ShaderCode.h"

#include "vulkan/vulkan.h"

#include <fstream>
#include "Shaderc/shaderc.hpp"
#include "spirv_glsl.hpp"
#include "spirv_hlsl.hpp"

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

	std::vector<ShaderCode::Attribute> ShaderCode::GetVertexLayoutEleList()
	{
		std::vector<Attribute> attribute_elements;
		
		if (shaderCodePIRV.find(ShaderCode::ShaderType::VERTEX) != shaderCodePIRV.end() )
		{
			spirv_cross::CompilerGLSL glsl_Vertex_code(shaderCodePIRV[ShaderCode::ShaderType::VERTEX]);
	
			// The SPIR-V is now parsed, and we can perform reflection on it.
			spirv_cross::ShaderResources resources = glsl_Vertex_code.get_shader_resources();
	
			for (auto& resource : resources.stage_inputs)
			{
				const size_t iLoc = glsl_Vertex_code.get_decoration(resource.id, spv::DecorationLocation);
				const auto& base_type = glsl_Vertex_code.get_type(resource.base_type_id);

				VertexLayout::ElementType start_type;
				switch (base_type.basetype)
				{
				case spirv_cross::SPIRType::BaseType::Float: start_type = VertexLayout::ElementType::AP_FLOAT;
					break;
				case spirv_cross::SPIRType::BaseType::Int: start_type = VertexLayout::ElementType::AP_INT;
					break;
				default:
					HZ_CORE_ASSERT(false, "VertexAttribute types SUPPORTED are only FLOATx and INTx!");
					break;
				}

				HZ_CORE_TRACE("InputAttribute <{}> location= {}, type= {}, nr= {}",
					resource.name,
					iLoc,
					base_type.basetype == spirv_cross::SPIRType::BaseType::Float ? "FLOAT" : "INT",
				base_type.vecsize);
				VertexLayout::ElementType attrType = VertexLayout::ElementType((int)VertexLayout::ElementType::AP_FLOAT + base_type.vecsize - 1);
				attribute_elements.push_back({ iLoc, attrType, resource.name });
			}
		}
		else
		{
			HZ_CORE_ASSERT(false, "ShaderCode has NOT a VERTEX CODE!");
		}
		return attribute_elements;
	}

	std::unordered_map<ShaderCode::ShaderType, std::string> ShaderCode::GetCodeGLSL()
	{
		std::unordered_map<ShaderType, std::string> shaderCodeGLSL;

		for (auto& kv : shaderCodePIRV)
		{
			ShaderCode::ShaderType type = kv.first;
			auto& spirvCode = kv.second;

			spirv_cross::CompilerGLSL glsl(spirvCode);

			// Set some options.
			spirv_cross::CompilerGLSL::Options options;
			//options.version = 450;
			//options.es = true;
			glsl.set_common_options(options);

			shaderCodeGLSL[type] = glsl.compile();
		}
		return shaderCodeGLSL;
	}

	std::unordered_map<ShaderCode::ShaderType, std::string> ShaderCode::GetCodeHLSL()
	{
		std::unordered_map<ShaderType, std::string> shaderCodeHLSL;

		for (auto& kv : shaderCodePIRV)
		{
			ShaderCode::ShaderType type = kv.first;
			auto& spirvCode = kv.second;

			spirv_cross::CompilerHLSL hlsl(spirvCode);

			// Set some options
			// ...

			shaderCodeGLSL[type] = hlsl.compile();
		}
		return shaderCodeGLSL;
	}

}
