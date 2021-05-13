#include "particle.h"
#include <stdlib.h>

Particle::Particle() {}

bool Particle::IsNone() {
	return type == ParticleType::NONE;
}

void Particle::ReassignTo(ParticleType type) {
	this->type = type;
	if (type == ParticleType::FIRE) {
		lifetime = rand() % 50 + 200;
	} else if (type == ParticleType::SMOKE || type == ParticleType::STEAM) {
		lifetime = rand() % 50 + 100;
	}
}