#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
#include <system_error>

namespace Engine::Graphics {
	class ShaderHandle {
	public:
		virtual ~ShaderHandle() = default;
		virtual void uninit() = 0;
	protected:
		std::string read_file_string(
			const std::filesystem::path& shader_path,
			std::error_code& ec
		) const;
		std::vector<uint32_t> read_file_uint32_t(
			const std::filesystem::path& shader_path,
			std::error_code& ec
		) const;
	};
}