#pragma once

#include <string>
#include <optional>

namespace Hazel {

	class FileDialogs
	{
	public:
		// These returns enpty string if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

}
