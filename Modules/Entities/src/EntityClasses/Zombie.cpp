#include <Entities/EntityClasses/Zombie.h>
#include <Utility/TimeManager.h>

void Engine::Entity::Zombie::on_create() {
	anim_idle = anim_comp.get_clip_id(0);
	anim_walk = anim_comp.get_clip_id(1);
	anim_jump = anim_comp.get_clip_id(2);
	anim_comp.play(anim_idle);
}
void Engine::Entity::Zombie::on_destroy() {

}
void Engine::Entity::Zombie::update() {

}