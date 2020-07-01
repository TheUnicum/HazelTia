#include "hzpch.h"
#include "Hazel/Renderer/Bindable.h"

namespace Hazel {

	std::unordered_map<std::string, Ref<Bindable>> Bindable::_s_map;

	Bindable::Bindable(GraphicsContext& ctx)
		: _ctx(ctx) {}

}
