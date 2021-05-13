#pragma once
#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"

struct Character {
  unsigned int textureId;
  glm::ivec2 size;
  glm::ivec2 bearing;
  int advance;
};

class TextRenderer {
public:
  std::map<char, Character> characters;
  Shader textShader;

  TextRenderer(unsigned int width, unsigned int height);

  void Load(std::string font, unsigned int fontSize);
  void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
private:
  unsigned int VAO, VBO;
};