#include <Entities/Enemies/Zombie.h>
#include <Utility/TimeManager.h>

void Zombie::on_create() {
	anim_idle = anim_controller.get_clip_id(0);
	anim_walk = anim_controller.get_clip_id(1);
	anim_jump = anim_controller.get_clip_id(2);
	anim_controller.play(anim_idle);
}
void Zombie::on_destroy() {

}
void Zombie::update() {

}