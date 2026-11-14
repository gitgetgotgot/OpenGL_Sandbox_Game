#pragma once
#include "Rendering/Interfaces/ShaderHandle.h"

namespace Engine::Graphics {
	class GL_Shader : public ShaderHandle {
	public:
		GL_Shader(
			const std::filesystem::path& vertex_shader_path,
			const std::filesystem::path& fragment_shader_path
		);
		~GL_Shader();
		void uninit() override;
		void use_shader() const;
	private:
		uint32_t id;
	};
}