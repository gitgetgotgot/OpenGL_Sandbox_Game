#pragma once
#include "Rendering/Interfaces/ShaderProgram.h"

namespace Engine::Graphics {
	class gl_ShaderPr : public ShaderProgram {
	public:
		gl_ShaderPr(
			const std::filesystem::path& vertex_shader_path,
			const std::filesystem::path& fragment_shader_path
		);
		~gl_ShaderPr();
		void use_shader() const override;
		void delete_shader() const override;
	private:
		uint32_t id;
	};
}