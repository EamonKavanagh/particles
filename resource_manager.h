#pragma once
#include <map>
#include <string>

#include <glad/glad.h>

#include "texture.h"
#include "shader.h"


class ResourceManager {
public:
  static std::map<std::string, Shader> shaders;
  static std::map<std::string, Texture2D> textures;

  static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
  static Shader GetShader(std::string name);
  static Texture2D LoadTexture(const char* file, bool alpha, std::string name);
  static Texture2D GetTexture(std::string name);
  static void Clear();
private:
  ResourceManager() {}
  static Shader LoadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
  static Texture2D LoadTextureFromFile(const char* file, bool alpha);
};