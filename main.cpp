#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "simulation.h"
#include "resource_manager.h"

constexpr unsigned int SCREEN_WIDTH = 960;
constexpr unsigned int SCREEN_HEIGHT = 640;
constexpr unsigned int GAME_SCALE_FACTOR = 4;
constexpr unsigned int GAME_WIDTH = SCREEN_WIDTH / GAME_SCALE_FACTOR;
constexpr unsigned int GAME_HEIGHT = SCREEN_HEIGHT / GAME_SCALE_FACTOR;

Simulation simulation(GAME_WIDTH, GAME_HEIGHT);

void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void MousePositionCallback(GLFWwindow* window, double xPos, double yPos);
void ScrollWheelCallback(GLFWwindow* window, double xoffset, double yoffset);

int main() {
	// BEGIN INIT GLFW
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Particles", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to load OpenGL functions with glad" << std::endl;
		return -1;
	}

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, MousePositionCallback);
	glfwSetScrollCallback(window, ScrollWheelCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSwapInterval(1);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	// END INIT GLFW

	ResourceManager::LoadShader("resources/shaders/game.vs", "resources/shaders/game.fs", nullptr, "game");

	float vertices[] = {
		 1.0f,  1.0f,   1.0f, 1.0f,
		 1.0f, -1.0f,   1.0f, 0.0f,
		-1.0f, -1.0f,   0.0f, 0.0f,
		-1.0f,  1.0f,   0.0f, 1.0f,
	};
	unsigned int indices[] = {
				0, 1, 3, // first triangle
				1, 2, 3  // second triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ResourceManager::GetShader("game").Use().SetInteger("tex", 0);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	simulation.Init();
	void* screenBuffer = malloc(sizeof(uint32_t) * GAME_WIDTH * GAME_HEIGHT);
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();

		simulation.ProcessInput();
		simulation.Update();

		glClear(GL_COLOR_BUFFER_BIT);
		simulation.Render(screenBuffer);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			GAME_WIDTH,
			GAME_HEIGHT,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			screenBuffer);

		ResourceManager::GetShader("game").Use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		simulation.RenderUi(deltaTime);

		glfwSwapBuffers(window);
	}

	free(screenBuffer);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	ResourceManager::Clear();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void ErrorCallback(int error, const char* description) {
	std::cout << "Got OpenGL error" << std::endl;
	std::cout << description << std::endl;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}

	if (action != GLFW_PRESS) {
		return;
	}

	if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) {
		simulation.lastNumKeyPressed = key - GLFW_KEY_1 + 1;
	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_1) {
		if (action == GLFW_PRESS) {
			simulation.mouseHeld = true;
		} else if (action == GLFW_RELEASE) {
			simulation.mouseHeld = false;
		}
	} else if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS) {
		simulation.brushSize = 2; // Restore to default
	}
}

void MousePositionCallback(GLFWwindow* window, double xPos, double yPos) {
	// In OpenGL, the X values go from left to right and Y values go from top to bottom.
	// In our simulation, X values go from left to right and Y values go from bottom to top.
	// Thus, we reverse the Y values.
	if (xPos < 0 || yPos <= 0 || xPos >= SCREEN_WIDTH || yPos > SCREEN_HEIGHT) {
		return;
	}
	simulation.mouseX = (unsigned int)xPos / GAME_SCALE_FACTOR;
	simulation.mouseY = (SCREEN_HEIGHT - (unsigned int)yPos) / GAME_SCALE_FACTOR;
}

void ScrollWheelCallback(GLFWwindow* window, double xOffset, double yOffset) {
	int newBrushSize = simulation.brushSize + (int)yOffset;
	if (newBrushSize > 0 && newBrushSize < 25) {
		simulation.brushSize = newBrushSize;
	}
}