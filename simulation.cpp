#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "simulation.h"
#include "text_renderer.h"

// RGBA colors
constexpr uint32_t SAND_COLOR = 194 + (178 << 8) + (128 << 16) + (255 << 24); 
const glm::vec3 SAND_COLOR_VEC = glm::vec3(0.76f, 0.7f, 0.5f);
constexpr uint32_t SAND_COLOR_CURSOR = 194 + (178 << 8) + (128 << 16) + (128 << 24);

constexpr uint32_t WATER_COLOR = 0 + (105 << 8) + (148 << 16) + (255 << 24); 
const glm::vec3 WATER_COLOR_VEC = glm::vec3(0, 0.41f, 0.58f);
constexpr uint32_t WATER_COLOR_CURSOR = 0 + (105 << 8) + (148 << 16) + (128 << 24);

constexpr uint32_t WOOD_COLOR = 111 + (76 << 8) + (30 << 16) + (135 << 24); 
const glm::vec3 WOOD_COLOR_VEC = glm::vec3(0.44f, 0.30f, 0.12f);
constexpr uint32_t WOOD_COLOR_CURSOR = 111 + (76 << 8) + (30 << 16) + (128 << 24);

constexpr uint32_t FIRE_COLOR = 226 + (88 << 8) + (34 << 16) + (255 << 24); 
const glm::vec3 FIRE_COLOR_VEC = glm::vec3(0.89f, 0.35f, 0.13f);
constexpr uint32_t FIRE_COLOR_CURSOR = 226 + (88 << 8) + (34 << 16) + (128 << 24);

constexpr uint32_t SMOKE_COLOR = 131 + (131 << 8) + (131 << 16) + (255 << 24);
const glm::vec3 SMOKE_COLOR_VEC = glm::vec3(0.51f, 0.51f, 0.51f);
constexpr uint32_t SMOKE_COLOR_CURSOR = 131 + (131 << 8) + (131 << 16) + (128 << 24);

constexpr uint32_t STEAM_COLOR = 245 + (245 << 8) + (245 << 16) + (255 << 24);
const glm::vec3 STEAM_COLOR_VEC = glm::vec3(0.96f, 0.96f, 0.96f);
constexpr uint32_t STEAM_COLOR_CURSOR = 245 + (245 << 8) + (245 << 16) + (128 << 24);

TextRenderer* text;

bool ShouldCatchFire() {
	return rand() % 70 == 0;
}

Simulation::Simulation(unsigned int width, unsigned int height) : width(width), height(height) {
	particles = new Particle[width * height];
}

void Simulation::Init() {
	text = new TextRenderer(this->width, this->height);
	text->Load("resources/fonts/LiberationMono-Regular.ttf", 24);
}

void Simulation::ProcessInput() {
	if (lastNumKeyPressed == 1) {
		typeSelected = ParticleType::SAND;
	} else if (lastNumKeyPressed == 2) {
		typeSelected = ParticleType::WATER;
	} else if (lastNumKeyPressed == 3) {
		typeSelected = ParticleType::WOOD;
	} else if (lastNumKeyPressed == 4) {
		typeSelected = ParticleType::FIRE;
	} else if (lastNumKeyPressed == 5) {
		typeSelected = ParticleType::SMOKE;
	} else if (lastNumKeyPressed == 6) {
		typeSelected = ParticleType::STEAM;
	}

	if (mouseHeld) {
		TryCreateInRegion(typeSelected, mouseX, mouseY, brushSize, brushSize);
	}
}

void Simulation::Update() {
	bool leftToRight = rand() % 2 == 0;
	if (leftToRight) {
		UpdateLeftToRight();
	} else {
		UpdateRightToLeft();
	}
}

void Simulation::Render(void* screenBuffer) {
	uint32_t* pixelData = (uint32_t*)screenBuffer;
	Particle* currentParticle = particles;

	unsigned int xMouseMin, yMouseMin, xMouseMax, yMouseMax;
	GetClampedCoords(mouseX, mouseY, brushSize, brushSize,
		&xMouseMin, &yMouseMin, &xMouseMax, &yMouseMax);
	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			currentParticle->updatedThisFrame = false;
			switch (currentParticle->type) {
				case ParticleType::NONE:
					*pixelData = 0;
					break;
				case ParticleType::SAND:
					*pixelData = SAND_COLOR;
					break;
				case ParticleType::WATER:
					*pixelData = WATER_COLOR;
					break;
				case ParticleType::WOOD:
					*pixelData = WOOD_COLOR;
					break;
				case ParticleType::FIRE:
					*pixelData = FIRE_COLOR;
					break;
				case ParticleType::SMOKE:
					*pixelData = SMOKE_COLOR;
					break;
				case ParticleType::STEAM:
					*pixelData = STEAM_COLOR;
					break;
			}

			if (x >= xMouseMin && x <= xMouseMax && y >= yMouseMin && y <= yMouseMax && currentParticle->IsNone()) {
				switch (typeSelected) {
					case ParticleType::SAND:
						*pixelData = SAND_COLOR_CURSOR;
						break;
					case ParticleType::WATER:
						*pixelData = WATER_COLOR_CURSOR;
						break;
					case ParticleType::WOOD:
						*pixelData = WOOD_COLOR_CURSOR;
						break;
					case ParticleType::FIRE:
						*pixelData = FIRE_COLOR_CURSOR;
						break;
					case ParticleType::SMOKE:
						*pixelData = SMOKE_COLOR_CURSOR;
						break;
					case ParticleType::STEAM:
						*pixelData = STEAM_COLOR_CURSOR;
						break;
				}
			}

			currentParticle++;
			pixelData++;
		}
	}
}

void Simulation::RenderUi(float dt) {
	text->RenderText("Sand(1)", 4.0f, 6.0f, .25f, SAND_COLOR_VEC);
	text->RenderText("Water(2)", 4.0f, 14.0f, .25f, WATER_COLOR_VEC);
	text->RenderText("Wood(3)", 4.0f, 22.0f, .25f, WOOD_COLOR_VEC);
	text->RenderText("Fire(4)", 4.0f, 30.0f, .25f, FIRE_COLOR_VEC);
	text->RenderText("Smoke(5)", 4.0f, 38.0f, .25f, SMOKE_COLOR_VEC);
	text->RenderText("Steam(6)", 4.0f, 46.0f, .25f, STEAM_COLOR_VEC);
	text->RenderText("Brush(scroll)", width - 65.0f, 6.0f, .25f);
	std::string s(16, '\0');
	auto written = std::snprintf(&s[0], s.size(), "MS/F %.1f", dt * 1000);
	s.resize(written);
	text->RenderText(s, width - 47.0f, 14.0f, .25f);
}

Particle* Simulation::GetParticleAtPosition(unsigned int x, unsigned int y) {
	assert(x < width);
	assert(y < height);
	return &particles[x + y * width];
}

bool Simulation::TryMoveParticleToPosition(Particle* particle, unsigned int x, unsigned int y) {
	if (x <= 0 || y <= 0 || x >= width || y >= height || !GetParticleAtPosition(x, y)->IsNone()) {
		return false;
	}
	Particle* newParticle = GetParticleAtPosition(x, y);
	newParticle->type = particle->type;
	newParticle->lifetime = particle->lifetime;
	newParticle->updatedThisFrame = particle->updatedThisFrame;
	particle->type = ParticleType::NONE;
	particle->lifetime = 0;
	particle->updatedThisFrame = true;
}

void Simulation::GetClampedCoords(
	unsigned int x, unsigned int y,
	unsigned int xDist, unsigned int yDist,
	unsigned int* xMin, unsigned int* yMin,
	unsigned int* xMax, unsigned int* yMax) {
	int xMinResult = x - xDist;
	if (xMinResult < 0) {
		xMinResult = 0;
	}
	int yMinResult = y - yDist;
	if (yMinResult < 0) {
		yMinResult = 0;
	}
	unsigned int xMaxResult = x + xDist;
	if (xMaxResult >= width) {
		xMaxResult = width - 1;
	}
	unsigned int yMaxResult = y + yDist;
	if (yMaxResult >= height) {
		yMaxResult = height - 1;
	}

	*xMin = xMinResult;
	*xMax = xMaxResult;
	*yMin = yMinResult;
	*yMax = yMaxResult;
}

void Simulation::UpdateLeftToRight() {
	Particle* currentParticle = particles;
	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			UpdateParticle(currentParticle, x, y);
			currentParticle++;
		}
	}
}

void Simulation::UpdateRightToLeft() {
	Particle* currentParticle = particles;
	for (unsigned int y = 0; y < height; y++) {
		currentParticle = &particles[(y + 1) * width - 1];
		for (int x = width - 1; x >= 0; x--) {
			UpdateParticle(currentParticle, x, y);
			currentParticle--;
		}
	}
}

void Simulation::UpdateParticle(Particle* particle, int x, int y) {
	if (particle->updatedThisFrame) {
		return;
	}
	particle->updatedThisFrame = true;
	int leftOrRight = rand() % 2 == 0 ? -1 : 1;
	if (particle->type == ParticleType::SAND) {
		if (y > 0) {
			TryMoveParticleToPosition(particle, x, y - 1) ||
				TryMoveParticleToPosition(particle, x + leftOrRight, y - 1) ||
				TryMoveParticleToPosition(particle, x - leftOrRight, y - 1);
		}
	} else if (particle->type == ParticleType::WATER) {
		Flow(particle, x, y, leftOrRight);
	} else if (particle->type == ParticleType::FIRE) {
		particle->lifetime--;
		if (particle->lifetime <= 0) {
			particle->lifetime = 0;
			particle->type = ParticleType::NONE;
		} else if (y > 0 && GetParticleAtPosition(x, y - 1)->type == ParticleType::WATER) {
			particle->ReassignTo(ParticleType::STEAM);
			GetParticleAtPosition(x, y - 1)->ReassignTo(ParticleType::STEAM);
		} else {
			unsigned int xMin, yMin, xMax, yMax;
			GetClampedCoords(x, y, 1, 1, &xMin, &yMin, &xMax, &yMax);
			bool didCatchFire = false;
			for (unsigned int j = yMin; j < yMax; j++) {
				for (unsigned int i = xMin; i < xMax; i++) {
					if (GetParticleAtPosition(i, j)->type == ParticleType::WOOD && ShouldCatchFire()) {
						GetParticleAtPosition(i, j)->ReassignTo(ParticleType::FIRE);
						didCatchFire = true;
						TryCreateInRegion(ParticleType::SMOKE, i, j + 2, 3, 2);
					}
				}
			}
			if (!didCatchFire) {
				Flow(particle, x, y, leftOrRight);
			}
		}
	} else if (particle->type == ParticleType::SMOKE || particle->type == ParticleType::STEAM) {
		particle->lifetime--;
		if (particle->lifetime <= 0) {
			particle->lifetime = 0;
			particle->type = ParticleType::NONE;
		} else {
			Float(particle, x, y, leftOrRight);
		}
	}
}

void Simulation::Flow(Particle* particle, int x, int y, int leftOrRight) {
	bool didMove = y > 0 &&
		(TryMoveParticleToPosition(particle, x, y - 1) ||
			TryMoveParticleToPosition(particle, x + leftOrRight, y - 1) ||
			TryMoveParticleToPosition(particle, x - leftOrRight, y - 1));

	if (!didMove) {
		TryMoveParticleToPosition(particle, x + leftOrRight, y) || TryMoveParticleToPosition(particle, x - leftOrRight, y);
	}
}

void Simulation::Float(Particle* particle, int x, int y, int leftOrRight) {
	bool didMove = false;
	if (y + 1 < height) {
		didMove = TryMoveParticleToPosition(particle, x, y + 1) ||
			TryMoveParticleToPosition(particle, x + leftOrRight, y + 1) ||
			TryMoveParticleToPosition(particle, x - leftOrRight, y + 1);
	}

	if (!didMove) {
		didMove = TryMoveParticleToPosition(particle, x + leftOrRight, y) || TryMoveParticleToPosition(particle, x - leftOrRight, y);
	}

	if (!didMove) {
		int j = y + 1;
		Particle* particleAbove;
		while (j < height) {
			particleAbove = GetParticleAtPosition(x, j);
			if (particleAbove->IsNone()) {
				TryMoveParticleToPosition(particle, x, j);
				break;
			} else if (!(particleAbove->type == ParticleType::FIRE ||
				particleAbove->type == ParticleType::WATER ||
				particleAbove->type == ParticleType::SMOKE ||
				particleAbove->type == ParticleType::STEAM)) {
				// Can only move through fire/water/smoke/steam
				break;
			}
			j += 5;
		}
	}
}

void Simulation::TryCreateInRegion(ParticleType type, int x, int y, int xDist, int yDist) {
	unsigned int xMouseMin, yMouseMin, xMouseMax, yMouseMax;
	GetClampedCoords(x, y, xDist, yDist,
		&xMouseMin, &yMouseMin, &xMouseMax, &yMouseMax);
	for (unsigned int j = yMouseMin; j < yMouseMax; j++) {
		for (unsigned int i = xMouseMin; i < xMouseMax; i++) {
			if (GetParticleAtPosition(i, j)->IsNone()) {
				GetParticleAtPosition(i, j)->ReassignTo(type);
			}
		}
	}
}