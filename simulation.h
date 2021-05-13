#pragma once

#include "particle.h"

class Simulation {
public:
	unsigned int mouseX = 0, mouseY = 0;
	bool mouseHeld = false;
	unsigned int lastNumKeyPressed = 0;
	unsigned int brushSize = 2;

	Simulation(unsigned int width, unsigned int height);
	void Init();
	void ProcessInput();
	void Update();
	void Render(void* screenBuffer);
	void RenderUi(float dt);

private:
	unsigned int width, height;
	Particle* particles;
	ParticleType typeSelected = ParticleType::SAND;

	Particle* GetParticleAtPosition(unsigned int x, unsigned int y);
	bool TryMoveParticleToPosition(Particle* particle, unsigned int x, unsigned int y);
	void GetClampedCoords(
		unsigned int x, unsigned int y,
		unsigned int xDist, unsigned int yDist,
		unsigned int* xMin, unsigned int* yMin,
		unsigned int* xMax, unsigned int* yMax);
	void UpdateLeftToRight();
	void UpdateRightToLeft();
	void UpdateParticle(Particle* particle, int x, int y);
	void Flow(Particle* particle, int x, int y, int leftOrRight);
	void Float(Particle* particle, int x, int y, int leftOrRight);
	void TryCreateInRegion(ParticleType type, int x, int y, int xDist, int yDist);
};