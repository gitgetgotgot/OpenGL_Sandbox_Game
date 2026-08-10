#include <Entities/Enemies/FlyingEye.h>
#include <Utility/TimeManager.h>

void FlyingEye::on_create() {
	flying_anim = anim_controller.get_clip_id(0);
	anim_controller.play(flying_anim);
}
void FlyingEye::on_destroy() {

}
void FlyingEye::update() {

}