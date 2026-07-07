#pragma once

#include "Animation.h"

struct AnimationLayer {
	Animation* m_animation = nullptr;
	std::vector<AnimatedTransform> m_globalNodeTransforms;
	std::vector<AnimatedTransform> m_localNodeTransforms;
	std::vector<float> m_boneWeights;
	float m_animationWeight = 1.f;
	float m_currentTime = 0.f;
	float m_animationSpeed = 1.f;
	bool m_loop = true;
	bool m_paused = false;
	bool m_isComlpete = false;
};

class Animator {
public:
	void PlayAnimation(const std::string& layerName, const std::string& animationName, float speed = 1.0f, bool loop = false);
	void PlayAndLoopAnimation(const std::string& layerName, const std::string& animationName, float speed = 1.0f);
	void UpdateAnimation(float dt);
	void CreateAnimationLayer(const std::string& name);
	void ClearAllAnimations();
	void PauseAnimation(const std::string& name);
	void PauseAllAnimations();

private:
	void UpdateAnimation(AnimationLayer& layer, float dt);
	float GetAnimationTimeInTicks(AnimationLayer& layer);
	const AnimatedNode* FindAnimatedNode(Animation* anim, const char* NodeName);
};