#pragma once

enum class ParticleType {
	NONE,
	SAND,
	WATER,
	WOOD,
	FIRE,
	SMOKE,
	STEAM,
};

class Particle {
public:
	ParticleType type = ParticleType::NONE;
	int lifetime = 0;
	bool updatedThisFrame = false;

	Particle();
	bool IsNone();
	void ReassignTo(ParticleType type);
};