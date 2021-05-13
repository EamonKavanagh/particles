#include <glm/gtc/matrix_transform.hpp>

#include "text_renderer.h"
#include "resource_manager.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <fstream>
#include <iostream>
#include <vector>

TextRenderer::TextRenderer(unsigned int width, unsigned int height) {
  this->textShader = ResourceManager::LoadShader("resources/shaders/text_2d.vs", "resources/shaders/text_2d.fs", nullptr, "text");
  this->textShader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f), true);
  this->textShader.SetInteger("text", 0);
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);
  glBindVertexArray(this->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void TextRenderer::Load(std::string fontFile, unsigned int fontSize) {
  this->characters.clear();
 
  stbtt_fontinfo font;
  float scale, xpos = 2; // leave a little padding in case the character extends left

  std::ifstream file(fontFile, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  char* fileBuffer = (char*)malloc(size);
  if (!file.read(fileBuffer, size)) {
    std::cout << "Failed to read font file" << std::endl;
    return;
  }

  stbtt_InitFont(&font, (uint8_t*)fileBuffer, stbtt_GetFontOffsetForIndex((uint8_t*)fileBuffer, 0));

  scale = stbtt_ScaleForPixelHeight(&font, (float)fontSize);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for (GLubyte c = 0; c < 128; c++) {
    int width, height, xOffset, yOffset, advance, leftSideBearing;
    unsigned char* fontBitmap = stbtt_GetCodepointBitmap(&font, scale, scale, c, &width, &height, &xOffset, &yOffset);
    stbtt_GetCodepointHMetrics(&font, c, &advance, &leftSideBearing);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      width,
      height,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      fontBitmap
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {
        texture,
        glm::ivec2(width, height),
        glm::ivec2(xOffset, yOffset),
        advance
    };
    characters.insert(std::pair<char, Character>(c, character));
    stbtt_FreeBitmap(fontBitmap, 0);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  free(fileBuffer);
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color) {
  // activate corresponding render state	
  this->textShader.Use();
  this->textShader.SetVector3f("textColor", color);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(this->VAO);

  // iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {
    Character ch = characters[*c];

    float xpos = x + ch.bearing.x * scale;
    float ypos = y + ch.bearing.y * scale;

    float w = ch.size.x * scale;
    float h = ch.size.y * scale;
    // update VBO for each character
    float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 0.0f },
        { xpos,     ypos,       0.0f, 0.0f },

        { xpos,     ypos + h,   0.0f, 1.0f },
        { xpos + w, ypos + h,   1.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 0.0f }
    };
    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.textureId);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // now advance cursors for next glyph
    x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}