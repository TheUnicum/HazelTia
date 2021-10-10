#pragma once

#include <filesystem>

<<<<<<< HEAD
=======
#include "Hazel/Renderer/Texture.h"

>>>>>>> content-browser
namespace Hazel {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;
<<<<<<< HEAD
=======

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
>>>>>>> content-browser
	};

}
