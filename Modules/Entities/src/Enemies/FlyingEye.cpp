#include <Entities/Enemies/FlyingEye.h>
#include <Utility/TimeManager.h>

void GameEntity::FlyingEye::on_create() {
	flying_anim = anim_controller.get_clip_id(0);
	anim_controller.play(flying_anim);
}
void GameEntity::FlyingEye::on_destroy() {

}
void GameEntity::FlyingEye::update() {

}