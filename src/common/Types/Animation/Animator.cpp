#include "Animator.h"

void Animator::PlayAnimation(const std::string& layerName, const std::string& animationName, float speed, bool loop) {

}

void Animator::PlayAndLoopAnimation(const std::string& layerName, const std::string& animationName, float speed) {

}

void Animator::UpdateAnimation(float dt) {

}

void Animator::CreateAnimationLayer(const std::string& name) {

}

void Animator::ClearAllAnimations() {

}

void Animator::PauseAnimation(const std::string& name) {

}

void Animator::PauseAllAnimations() {

}

void Animator::UpdateAnimation(AnimationLayer& layer, float dt) {

}

float Animator::GetAnimationTimeInTicks(AnimationLayer& layer) {
	return 0;
}

const AnimatedNode* Animator::FindAnimatedNode(Animation* anim, const char* NodeName) {
	AnimatedNode* node;
	return node;
}
