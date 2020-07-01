#pragma once

namespace Hazel {

	class Window;

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		// API functions
		virtual void CmdClear_impl() {};
		virtual void CmdClearColor_impl(float red, float green, float blue) {};
		virtual void CmdDrawArrays_impl(uint32_t vertexCount, uint32_t offset = 0) {}; // = 0;
		virtual void CmdDrawArraysInstanced_impl(uint32_t vertexCount, uint32_t indexCount) {}; // = 0;
		virtual void CmdDrawIndexted_impl(uint32_t indexCount, uint32_t offset = 0) {}; // = 0;
		virtual void CmdDrawIndextedInstanced_impl(uint32_t indexCount, uint32_t instanceCount) {}; // = 0;

		virtual API GetAPI() const = 0;
		virtual std::string GetAPI_TEXT() const = 0;
		virtual API MakeCurrent() = 0;

		// static
		static std::string GenerateUID(API api, Window& window);
		static GraphicsContext& Get_Active();
		static API Get_API_Active();

		// Classes Factory
		static Ref<GraphicsContext> Create(Window& window);
		static Ref<GraphicsContext> Create(API api, Window& window);

		static Ref<GraphicsContext> Resolve(Window& window, bool make_new_entity = false);
		static Ref<GraphicsContext> Resolve(API api, Window& window, bool make_new_entity = false);
		static void Release(API api, Window& window);
	protected:
		static Ref<GraphicsContext> _s_active; // TODO: change to be a normal reference
	private:
		static std::unordered_map<std::string, Ref<GraphicsContext>> _s_map_context;

	// Example API Functions
	public:
		static void DrawTriangle(float angle) { GraphicsContext::Get_Active().DrawTriangle_impl(angle); }
		virtual void DrawTriangle_impl(float angle) {};
		
		virtual void ClearBuffer_impl(float red, float green, float blue) {};
	};
}
