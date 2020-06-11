#pragma once

#include <string>

#include "Hazel/Core/Base.h"

#include "Hazel/Renderer/Bindable.h"
#include "Hazel/Renderer/GraphicsContext.h"

namespace Hazel {

	class Texture : public Bindable
	{
	public:
		Texture(GraphicsContext& ctx) : Bindable(ctx) {}
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot) const = 0;
		virtual void SetSlot(uint32_t slot) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(GraphicsContext& ctx) : Texture(ctx) {}

		// static
		//static std::string GenerateUID(GraphicsContext& ctx, uint32_t width, uint32_t height);
		//static std::string GenerateUID(GraphicsContext& ctx, const std::string& filepath);

		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(GraphicsContext& ctx, uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);
		static Ref<Texture2D> Create(GraphicsContext& ctx, const std::string& path);
	};

}
