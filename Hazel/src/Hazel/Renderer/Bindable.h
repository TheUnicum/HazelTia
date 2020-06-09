#pragma once

#include <unordered_map>

#include "Hazel/Renderer/GraphicsContext.h"

namespace Hazel {

	class Bindable
	{
	public:
		Bindable() = default;
		virtual ~Bindable() = default;
	
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
	
		// Bindable Function to emable intellisense and automatic COLLECT TO ACTIVE CONTEXT if not manually inserted-	
		template<class T, typename...Params>
		static Ref<Bindable> ResolvePctx(Params&&...p) { return Bindable::_Resolve<T>((GraphicsContext&)GraphicsContext::Get_Active(), std::forward<Params>(p)...); }	
		//template<class T, typename...Params>
		//static Ref<Bindable> ResolveP2(GraphicsContext& ctx, Params&&...p) { return Bindable::_Resolve<T>(std::forward<Params>(p)...); }
	
	
		template<class T, typename...Params>
		static Ref<Bindable> _Resolve(Params&&...p)
		{
			const auto key = T::GenerateUID(std::forward<Params>(p)...);
			const auto bind = _s_map.find(key);
			if (bind == _s_map.end())
			{
				Ref<T> new_context = T::Create(std::forward<Params>(p)...);
				_s_map[key] = new_context;
				return new_context;
			}
			else
			{
				return std::static_pointer_cast<T>(bind->second);
			}
		}
		static std::unordered_map<std::string, Ref<Bindable>> _s_map;
	};

}


///////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////FACTORY CLASSES MAKER///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
#define GL_ON 1
#define D3D_ON 1

#if (GL_ON)
	#define GL_CLASS_CREATION_1arg(T_OPENGL, arg0)				case API::OpenGL:  return CreateRef<T_OPENGL>(arg0)
	#define GL_CLASS_CREATION_2arg(T_OPENGL, arg0, arg1)		case API::OpenGL:  return CreateRef<T_OPENGL>(arg0, arg1)
	#define GL_CLASS_CREATION_3arg(T_OPENGL, arg0, arg1, arg2)	case API::OpenGL:  return CreateRef<T_OPENGL>(arg0, arg1, arg2)
#else	
	#define GL_CLASS_CREATION_1arg(T_OPENGL, arg0)
	#define GL_CLASS_CREATION_2arg(T_OPENGL, arg0, arg1)
	#define GL_CLASS_CREATION_3arg(T_OPENGL, arg0, arg1, arg2)
#endif
	
#if (D3D_ON)
	#define D3D_CLASS_CREATION_1arg(T_D3D11, arg0)				case API::D3D11:  return CreateRef<T_D3D11>(arg0)
	#define D3D_CLASS_CREATION_2arg(T_D3D11, arg0, arg1)		case API::D3D11:  return CreateRef<T_D3D11>(arg0, arg1)
	#define D3D_CLASS_CREATION_3arg(T_D3D11, arg0, arg1, arg2)	case API::D3D11:  return CreateRef<T_D3D11>(arg0, arg1, arg2)
#else	
	#define D3D_CLASS_CREATION_1arg(T_D3D11, arg0)
	#define D3D_CLASS_CREATION_2arg(T_D3D11, arg0, arg1)
	#define D3D_CLASS_CREATION_3arg(T_D3D11, arg0, arg1, arg2)
#endif


#define MAKER_ON_ctx_1arg(T_OPENGL, T_D3D11, arg0)\
	switch (ctx.GetAPI())\
	{\
		case API::None:    HZ_CORE_ASSERT(false, "Rendererctx::None is currently not supported!"); return nullptr;\
		GL_CLASS_CREATION_1arg(T_OPENGL, arg0);\
		D3D_CLASS_CREATION_1arg(T_D3D11, arg0);\
	}\
	HZ_CORE_ASSERT(false, "Unknow Rendererctx!");\
	return nullptr;\

#define MAKER_ON_ctx_arg2(T_OPENGL, T_D3D11, arg0, arg1)\
	switch (ctx.GetAPI())\
	{\
		case API::None:    HZ_CORE_ASSERT(false, "Rendererctx::None is currently not supported!"); return nullptr;\
		GL_CLASS_CREATION_2arg(T_OPENGL, arg0, arg1);\
		D3D_CLASS_CREATION_2arg(T_D3D11, arg0, arg1);\
	}\
	HZ_CORE_ASSERT(false, "Unknow Rendererctx!");\
	return nullptr;\

#define MAKER_ON_ctx_arg3(T_OPENGL, T_D3D11, arg0, arg1, arg2)\
	switch (ctx.GetAPI())\
	{\
		case API::None:    HZ_CORE_ASSERT(false, "Rendererctx::None is currently not supported!"); return nullptr;\
		GL_CLASS_CREATION_3arg(T_OPENGL, arg0, arg1, arg2);\
		D3D_CLASS_CREATION_3arg(T_D3D11, arg0, arg1, arg2);\
	}\
	HZ_CORE_ASSERT(false, "Unknow Rendererctx!");\
	return nullptr;\

















/*

		// static
		static std::string GenerateUID(API api, Window& window);
		static GraphicsContext& Get_Active()
		{
			HZ_CORE_ASSERT(_s_active, "Graphiccs Context not initialized!");
			return *_s_active;
		}
		static API Get_API_Active();

		// Classes Factory
		static Ref<GraphicsContext> Create(Window& window);
		static Ref<GraphicsContext> Create(API api, Window& window);

		static Ref<GraphicsContext> Resolve(Window& window, bool make_new_entity = false);
		static Ref<GraphicsContext> Resolve(API api, Window& window, bool make_new_entity = false);
		static void Release(API api, Window& window);
	public:
		static Ref<GraphicsContext> _s_active;
		static std::unordered_map<std::string, Ref<GraphicsContext>> _s_map_context;

		// API functions
	public:
		static void DrawTriangle(float angle) { GraphicsContext::Get_Active().DrawTriangle_impl(angle); }
		virtual void DrawTriangle_impl(float angle) {};

		virtual void ClearBuffer_impl(float red, float green, float blue) {};
	};

}


*/