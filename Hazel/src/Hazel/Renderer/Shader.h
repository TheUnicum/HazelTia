#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#ifdef HZ_PLATFORM_WINDOWS
	#include <d3d11.h>
	#include <wrl.h>
#endif

#include "Hazel/Renderer/GraphicsContext.h"

class OpenGLShader;
class D3D11Shader;

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
		static std::string GenerateUID(GraphicsContext& ctx, const std::string& filepath);
		static std::string GenerateUID(GraphicsContext& ctx, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		// Classes Factory
		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(GraphicsContext& ctx, const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(GraphicsContext& ctx, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		static Ref<Shader> Resolve(const std::string& filepath, bool make_new_only = false);
		static Ref<Shader> Resolve(GraphicsContext& ctx, const std::string& filepath, bool make_new_only = false);

		//

		//template<class T, typename...Params>
		//static Ref<Shader> CreateT(API api, GraphicsContext& ctx, Params&&...p)
		//{
		//
		//}

		template<class T, typename...Params>
		static Ref<T> ResolveT(GraphicsContext& ctx, Params&&...p)
		{
			switch (ctx.GetAPI())
			{
				case API::None:    HZ_CORE_ASSERT(false, "Rendererctx::None is currently not supported!"); return nullptr;
				//case API::OpenGL:  return CreateRef<Shader>(std::forward<Params>(p)...);
				//case API::OpenGL:  return std::make_shared<T>(std::forward<Params>(p)...);

				//case API::D3D11:   return CreateRef<Shader>(std::forward<Params>(p)...);
			}
			HZ_CORE_ASSERT(false, "Unknow Rendererctx!");
			return nullptr;
		}

		////template<class Shader, typename...params>
		//template<class T, typename...Params>
		//static Ref<T> Make(Params&&...p)
		//{
		//	return Make<T>((GraphicsContext&)GraphicsContext::Get_Active(), std::forward<Params>(p)...);
		//}
		//
		//#define CREATE_2(TOpenGL, TD3D11, arg0)\
		//	switch (ctx.GetAPI())\
		//	{\
		//		case API::None:    HZ_CORE_ASSERT(false, "Rendererctx::None is currently not supported!"); return nullptr;\
		//		case API::OpenGL:  return CreateRef<TOpenGL>(arg0);\
		//		case API::D3D11:   return CreateRef<TD3D11>(arg0);\
		//	}\
		//	HZ_CORE_ASSERT(false, "Unknow Rendererctx!");\
		//	return nullptr;\
		//
		//




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
