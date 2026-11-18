#include "UI/UI_System.h"
#include "UI/UI_ObjectManager.h"
#include "UI/CanvasManager.h"
#include "UI/UI_BehaviourSystem.h"
#include <DeviceSystem/DeviceContext.h>
#include <Utility/Logger.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <Resources/TextureArrayMgr.h>

bool Engine::UI::UI_System::init() {
	namespace EG = Engine::Graphics;

	const uint32_t MAX_SPRITES_VERTEX_SIZE = MAX_SPRITES_PER_DRAW * 4;
	const uint32_t MAX_SPRITES_INDEX_SIZE = MAX_SPRITES_PER_DRAW * 6;

	const uint32_t MAX_SDF_TEXT_VERTEX_SIZE = MAX_SDF_TEXT_PER_DRAW * 4;
	const uint32_t MAX_SDF_TEXT_INDEX_SIZE = MAX_SDF_TEXT_PER_DRAW * 6;

	// Sprites shader handles & buffers
	std::error_code ec;
	std::filesystem::path vertex_shader_path;
	std::filesystem::path fragment_shader_path;

	vertex_shader_path = "Resources/shaders/UI/ui_sprites.vert";
	fragment_shader_path = "Resources/shaders/UI/ui_sprites.frag";
	sprites_shader = EG::Renderer::instance->create_ShaderHandle(vertex_shader_path, fragment_shader_path, ec);
	if (ec) {
		Engine::Util::Logger::Log("Failed to create Shader Handle for UI sprites: " + ec.message());
		return false;
	}

	fragment_shader_path = "Resources/shaders/UI/ui_tiled_sprites.frag";
	tiled_sprites_shader = EG::Renderer::instance->create_ShaderHandle(vertex_shader_path, fragment_shader_path, ec);
	if (ec) {
		Engine::Util::Logger::Log("Failed to create Shader Handle for UI tiled sprites: " + ec.message());
		return false;
	}

	// Common index buffer
	common_ib = EG::Renderer::instance->create_IndexBuffer(ec);
	uint32_t* sprite_index_buffer = new uint32_t[MAX_SPRITES_INDEX_SIZE];
	for (int i = 0; i < MAX_SPRITES_PER_DRAW; i++) {
		sprite_index_buffer[i * 6] = sprite_index_buffer[i * 6 + 3] = i * 4;
		sprite_index_buffer[i * 6 + 2] = sprite_index_buffer[i * 6 + 4] = sprite_index_buffer[i * 6] + 2;
		sprite_index_buffer[i * 6 + 1] = sprite_index_buffer[i * 6 + 2] - 1;
		sprite_index_buffer[i * 6 + 5] = sprite_index_buffer[i * 6 + 2] + 1;
	}
	common_ib->set_data(sprite_index_buffer, MAX_SPRITES_INDEX_SIZE * sizeof(uint32_t), EG::IndexBuffer::USAGE_STATIC);
	delete[] sprite_index_buffer;

	sprites_vl = EG::Renderer::instance->create_VertexLayout(ec);
	sprites_vl->add_attribute(0, EG::VertexAttributeType::FLOAT_VEC2, sizeof(UI_Vertex2f), offsetof(UI_Vertex2f, pos));
	sprites_vl->add_attribute(1, EG::VertexAttributeType::FLOAT_VEC2, sizeof(UI_Vertex2f), offsetof(UI_Vertex2f, UV));
	sprites_vl->add_attribute(2, EG::VertexAttributeType::FLOAT_VEC4, sizeof(UI_Vertex2f), offsetof(UI_Vertex2f, color));
	sprites_vl->add_attribute(3, EG::VertexAttributeType::UINT,		  sizeof(UI_Vertex2f), offsetof(UI_Vertex2f, tex_array_id));
	sprites_vl->add_attribute(4, EG::VertexAttributeType::UINT,		  sizeof(UI_Vertex2f), offsetof(UI_Vertex2f, tex_id));

	sprites_vb = EG::Renderer::instance->create_VertexBuffer(ec);
	sprites_vb->set_data(nullptr, MAX_SPRITES_VERTEX_SIZE * sizeof(UI_Vertex2f), EG::VertexBuffer::USAGE_DYNAMIC);

	// SDF text shader handle & buffers
	vertex_shader_path = "Resources/shaders/UI/sdf_text.vert";
	fragment_shader_path = "Resources/shaders/UI/sdf_text.frag";
	sdf_text_shader = EG::Renderer::instance->create_ShaderHandle(vertex_shader_path, fragment_shader_path, ec);
	if (ec) {
		Engine::Util::Logger::Log("Failed to create Shader Handle for UI SDF text: " + ec.message());
		return false;
	}

	sdf_text_vl = EG::Renderer::instance->create_VertexLayout(ec);
	sdf_text_vl->add_attribute(0, EG::VertexAttributeType::FLOAT_VEC2, sizeof(UI_Text_Vertex2f), offsetof(UI_Text_Vertex2f, pos));
	sdf_text_vl->add_attribute(1, EG::VertexAttributeType::FLOAT_VEC2, sizeof(UI_Text_Vertex2f), offsetof(UI_Text_Vertex2f, UV));
	sdf_text_vl->add_attribute(2, EG::VertexAttributeType::FLOAT_VEC4, sizeof(UI_Text_Vertex2f), offsetof(UI_Text_Vertex2f, color));

	sdf_text_vb = EG::Renderer::instance->create_VertexBuffer(ec);
	sdf_text_vb->set_data(nullptr, MAX_SDF_TEXT_VERTEX_SIZE * sizeof(UI_Text_Vertex2f), EG::VertexBuffer::USAGE_DYNAMIC);

	ubo_data.viewMatrix = glm::mat4(1.0f);
	ubo_data.projectionMatrix = glm::ortho(-DeviceContext::display.ratio, DeviceContext::display.ratio, -1.0f, 1.0f);
	common_ub = EG::Renderer::instance->create_UniformBuffer(ec);
	common_ub->set_data(&ubo_data, sizeof(UI_UBO), EG::UniformBuffer::USAGE_DYNAMIC);

	// Pipelines
	sprites_pipeline = EG::Renderer::instance->create_Pipeline(ec);
	if (!ec) sprites_pipeline->init(
		*sprites_shader, *sprites_vl,
		EG::Pipeline::PipelinePrimitives::Triangle, EG::Pipeline::PipelineScissor::Dynamic,
		EG::Pipeline::PipelineViewport::Dynamic, EG::Pipeline::PipelineBlendMode::Alpha,
		EG::Pipeline::PipelineDepthMode::ReadAndWrite, EG::Pipeline::PipelineCullMode::Back, ec
	);

	if (!ec) tiled_sprites_pipeline = EG::Renderer::instance->create_Pipeline(ec);
	if (!ec) tiled_sprites_pipeline->init(
		*tiled_sprites_shader, *sprites_vl,
		EG::Pipeline::PipelinePrimitives::Triangle, EG::Pipeline::PipelineScissor::Dynamic,
		EG::Pipeline::PipelineViewport::Dynamic, EG::Pipeline::PipelineBlendMode::Alpha,
		EG::Pipeline::PipelineDepthMode::ReadAndWrite, EG::Pipeline::PipelineCullMode::Back, ec
	);

	if (!ec) sdf_text_pipeline = EG::Renderer::instance->create_Pipeline(ec);
	if (!ec) sdf_text_pipeline->init(
		*sdf_text_shader, *sdf_text_vl,
		EG::Pipeline::PipelinePrimitives::Triangle, EG::Pipeline::PipelineScissor::Dynamic,
		EG::Pipeline::PipelineViewport::Dynamic, EG::Pipeline::PipelineBlendMode::Alpha,
		EG::Pipeline::PipelineDepthMode::ReadAndWrite, EG::Pipeline::PipelineCullMode::Back, ec
	);

	if (ec) {
		Engine::Util::Logger::Log("Failed to create UI pipelines: " + ec.message());
		return false;
	}

	// Bind all samplers [0, 8] and common UB to all pipelines
	Engine::Resource::TextureArrayManager::Instance().bind_samplers_to_pipeline(*sprites_pipeline);
	sprites_pipeline->bind_uniform_buffer(*common_ub, 1);

	Engine::Resource::TextureArrayManager::Instance().bind_samplers_to_pipeline(*tiled_sprites_pipeline);
	tiled_sprites_pipeline->bind_uniform_buffer(*common_ub, 1);

	Engine::Resource::TextureArrayManager::Instance().bind_samplers_to_pipeline(*sdf_text_pipeline);
	sdf_text_pipeline->bind_uniform_buffer(*common_ub, 1);

	// Reserve memory for buffers (this size should be enough for most scenarios)
	render_context.sprites_buffer.reserve(MAX_SPRITES_VERTEX_SIZE);
	render_context.sdf_text_buffer.reserve(MAX_SDF_TEXT_VERTEX_SIZE);

	return true;
}

void Engine::UI::UI_System::uninit() {
	common_ub.reset();
	common_ib.reset();

	sprites_pipeline.reset();
	tiled_sprites_pipeline.reset();
	sdf_text_pipeline.reset();

	sprites_shader.reset();
	tiled_sprites_shader.reset();
	sdf_text_shader.reset();

	sprites_vl.reset();
	sdf_text_vl.reset();

	sprites_vb.reset();
	sdf_text_vb.reset();
}

void Engine::UI::UI_System::update() {
	render_context.sprites_buffer.clear();
	render_context.sdf_text_buffer.clear();
	render_context.render_queue.resize(1); //each new frame at least one empty should remain

	render_context.clip_rects.clear();
	render_context.content_offsets.clear();

	// update hit queue for interactable components
	uint32_t components_size = UI_ComponentManager::Instance().size();
	if (render_context.hit_queue.capacity() < components_size) {
		render_context.hit_queue.resize(components_size);
	}
	render_context.hit_queue.clear();

	// remove all destroyed UI objects and their components
	UI_ObjectManager::Instance().remove_destroyed_objects();

	// place default clip rect and content offset
	render_context.clip_rects.emplace_back(
		0, 0, DeviceContext::display.width, DeviceContext::display.height,
		0.0f, 0.0f, DeviceContext::display.ratio * 2.0f, 2.0f
	).mouse_inside = true;
	render_context.content_offsets.emplace_back(0.0f, 0.0f);

	// update all dirty UI objects
	UI_ObjectManager::Instance().update_dirty_objects();

	// update all UI object buffers, build render queue and hit queue
	UI_RenderState state;
	CanvasManager::Instance().update_render_queue(render_context, state);

	// update interactable components
	UI_BehaviourSystem::Instance().update(render_context);

	sprites_vb->update_data(render_context.sprites_buffer.data(), render_context.sprites_buffer.size() * sizeof(UI_Vertex2f));
	sdf_text_vb->update_data(render_context.sdf_text_buffer.data(), render_context.sdf_text_buffer.size() * sizeof(UI_Text_Vertex2f));
}

void Engine::UI::UI_System::render(Engine::Graphics::Renderer* renderer) {
	sprites_INDEX_OFFSET = 0;
	sdf_text_INDEX_OFFSET = 0;

	renderer->use_scissor_test(true);
	renderer->set_scissor_rect(0, 0, DeviceContext::display.width, DeviceContext::display.height);

	ubo_data.content_offset = glm::vec2(0.0f);
	common_ub->update_data(&ubo_data, sizeof(UI_UBO));

	for (auto& render_entry : render_context.render_queue) {
		// change clip rectangle
		if (render_entry.change_clip_rect) {
			ClipRectangle& rect = render_context.clip_rects[render_entry.clip_rect_id];
			renderer->set_scissor_rect(rect.x, rect.y, rect.w, rect.h);
		}

		// change content offset in UBO
		if (render_entry.change_content_offset) {
			ubo_data.content_offset = render_context.content_offsets[render_entry.content_offset_id];
			common_ub->update_data(&ubo_data, sizeof(UI_UBO));
		}

		// render sprites
		if (render_entry.render_type == UI_Render_Type::UI_SPRITE) {
			renderer->bind_pipeline(*sprites_pipeline);
			renderer->renderIndexedData(*sprites_vb, *common_ib, render_entry.index_count, sprites_INDEX_OFFSET);
			sprites_INDEX_OFFSET += render_entry.index_count;
		}
		// render tiled sprites
		else if (render_entry.render_type == UI_Render_Type::UI_TILED_SPRITE) {
			renderer->bind_pipeline(*tiled_sprites_pipeline);
			renderer->renderIndexedData(*sprites_vb, *common_ib, render_entry.index_count, sprites_INDEX_OFFSET);
			sprites_INDEX_OFFSET += render_entry.index_count;
		}
		// render sdf text
		else if (render_entry.render_type == UI_Render_Type::UI_TEXT) {
			renderer->bind_pipeline(*sdf_text_pipeline);
			renderer->renderIndexedData(*sdf_text_vb, *common_ib, render_entry.index_count, sdf_text_INDEX_OFFSET);
			sdf_text_INDEX_OFFSET += render_entry.index_count;
		}
	}
	renderer->use_scissor_test(false);
}