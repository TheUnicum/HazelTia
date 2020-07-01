#pragma once

#include "Hazel/Renderer/VertexLayout.h"

#include <string>
#include <unordered_map>
#include <vector>


namespace Hazel {

	class ShaderCode
	{
	public:
		enum class ShaderType
		{
			UNDEFINED,
			VERTEX,
			GEOMETRY,
			FRAGMENT,
		};

		struct Attribute
		{
			size_t location;
			VertexLayout::ElementType type;
			std::string name;
		};

		const ShaderType& PIXEL = ShaderType::FRAGMENT;

	public:
		ShaderCode(const std::string& filepath);
		virtual ~ShaderCode() = default;


		static Ref<ShaderCode> ShaderCode::Create(const std::string& filepath);

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<ShaderType, std::string>& shaderSources);

	public:
		std::vector<Attribute> GetVertexLayoutEleList();
		std::vector<Attribute> GetVertexLayoutEleListHLSL();
		std::unordered_map<ShaderType, std::string> GetCodeGLSL();
		std::unordered_map<ShaderType, std::string> GetCodeHLSL();
	private:
		std::unordered_map<ShaderType, std::vector<uint32_t>> shaderCodePIRV;
		std::unordered_map<ShaderCode::ShaderType, std::string> shaderCodeGLSL;
	};

}