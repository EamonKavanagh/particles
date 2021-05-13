#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

std::map<std::string, Texture2D> ResourceManager::textures;
std::map<std::string, Shader> ResourceManager::shaders;


Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name) {
  shaders[name] = LoadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
  return shaders[name];
}

Shader ResourceManager::GetShader(std::string name) {
  return shaders[name];
}

Texture2D ResourceManager::GetTexture(std::string name) {
  return textures[name];
}

void ResourceManager::Clear() {
  for (auto iter : shaders)
    glDeleteProgram(iter.second.id);
  for (auto iter : textures)
    glDeleteTextures(1, &iter.second.id);
}

Shader ResourceManager::LoadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile) {
  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;
  try {
    std::ifstream vertexShaderFile(vShaderFile);
    std::ifstream fragmentShaderFile(fShaderFile);
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vertexShaderFile.rdbuf();
    fShaderStream << fragmentShaderFile.rdbuf();

    vertexShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    if (gShaderFile != nullptr) {
      std::ifstream geometryShaderFile(gShaderFile);
      std::stringstream gShaderStream;
      gShaderStream << geometryShaderFile.rdbuf();
      geometryShaderFile.close();
      geometryCode = gShaderStream.str();
    }
  } catch (std::exception e) {
    std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
  }
  const char* vShaderCode = vertexCode.c_str();
  const char* fShaderCode = fragmentCode.c_str();
  const char* gShaderCode = geometryCode.c_str();

  Shader shader;
  shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
  return shader;
}