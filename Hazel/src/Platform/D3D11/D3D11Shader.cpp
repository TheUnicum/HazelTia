#include "hzpch.h"
#include "Platform/D3D11/D3D11Shader.h"

#include <fstream>
#include <glad/glad.h> // only for GL_ENUM (GL_VERTEX_SHADER)

#include <d3dcompiler.h>
namespace wrl = Microsoft::WRL;



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

	D3D11Shader::D3D11Shader(const std::string& filepath)
		: _c((D3D11Context&)GraphicsContext::Get_Active()) // possible to store as a normal context and add 
	{
		HZ_PROFILE_FUNCTION();

		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		//Extract name from filepath: assets/shaders/Texture.glsl
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind(".");
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	D3D11Shader::D3D11Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name), _c((D3D11Context&)GraphicsContext::Get_Active())
	{
		HZ_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	D3D11Shader::~D3D11Shader()
	{
		HZ_PROFILE_FUNCTION();

		//glDeleteProgram(m_RendererID);
	}

	std::string D3D11Shader::ReadFile(const std::string& filepath)
	{
		HZ_PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
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

	std::unordered_map<GLenum, std::string> D3D11Shader::PreProcess(const std::string& source)
	{
		HZ_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLenght = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLenght + 1; //Start of shader type name (after "#type" keyword)
			std::string type = source.substr(begin, eol - begin);
			HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified!");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void D3D11Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		wrl::ComPtr<ID3DBlob> pBlobCompiled;
		wrl::ComPtr<ID3DBlob> pBlobErrorMsgs;
		HRESULT hr;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
		#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
		#endif

		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			std::string string_pTarget;
			switch (type)
			{
			case GL_VERTEX_SHADER:	string_pTarget = "vs_4_0"; break;
			case GL_FRAGMENT_SHADER:string_pTarget = "ps_4_0"; break;
			default:
				break;
			}

			hr = D3DCompile(
				source.c_str(),
				source.length(),
				nullptr,						// LPCSTR                 pSourceName
				nullptr,						// const D3D_SHADER_MACRO * pDefines
				nullptr,						// ID3DInclude * pInclude,
				"main",							// LPCSTR                 pEntrypoint, 
				string_pTarget.c_str(),						// LPCSTR                 pTarget,
				flags,	// UINT                   Flags1,
				0,								// UINT                   Flags2,
				&pBlobCompiled,					// ID3DBlob               **ppCode,
				&pBlobErrorMsgs							// ID3DBlob               **ppErrorMsgs
			);
			if (FAILED(hr))
			{
				HZ_CORE_ERROR("{0}", (char*)pBlobErrorMsgs->GetBufferPointer()); //&infoLog[0]
				HZ_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			switch (type)
			{
			case GL_VERTEX_SHADER:
				pBlobStoredCompiledVertex = pBlobCompiled;
				_c.GetPP().m_pDevice->CreateVertexShader(pBlobCompiled->GetBufferPointer(), pBlobCompiled->GetBufferSize(), nullptr, &pVertexShader);
				break;
			case GL_FRAGMENT_SHADER:
				string_pTarget = "ps_4_0";
				_c.GetPP().m_pDevice->CreatePixelShader(pBlobCompiled->GetBufferPointer(), pBlobCompiled->GetBufferSize(), nullptr, &pPixelShader);

				break;
			default:
				break;
			}


		}





	}

	void D3D11Shader::Bind() const
	{
		HZ_PROFILE_FUNCTION();

		_c.GetPP().m_pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
		_c.GetPP().m_pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

		//glUseProgram(m_RendererID);
	}

	void D3D11Shader::Unbind() const
	{
		HZ_PROFILE_FUNCTION();

		//glUseProgram(0);
	}
}
