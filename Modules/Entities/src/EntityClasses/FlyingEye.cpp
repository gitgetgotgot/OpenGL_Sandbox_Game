#include <Entities/EntityClasses/FlyingEye.h>
#include <Utility/TimeManager.h>

void Engine::Entity::FlyingEye::on_create() {
	flying_anim = anim_controller.get_clip_id(0);
	anim_controller.play(flying_anim);
}
void Engine::Entity::FlyingEye::on_destroy() {

}
void Engine::Entity::FlyingEye::update() {

}