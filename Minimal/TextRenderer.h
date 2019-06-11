#pragma once

#include "Core.h"
#include "Shader.h"
#include <ft2build.h>
#include <map>
#include FT_FREETYPE_H

struct Character {
	GLuint     textureID;	// ID handle of the glyph texture
	glm::ivec2 size;		// size of glyph
	glm::ivec2 bearing;		// offset from baseline to left/top of glyph
	GLuint     advance;		// Offset to advance to next glyph
};

class TextRenderer {
private:
	std::string fontpath;

	FT_Library ft;
	FT_Face face;
	int fontSize;

	std::map<GLchar, Character> characters;

	Shader textShader = Shader("../Minimal/textShader.vert", "../Minimal/textShader.frag");
	GLuint VAO, VBO;

	inline void loadAciiChars() {
		textShader.use();
		// disable bye-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (GLubyte c = 0; c < 128; c++) {
			// Load character glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cerr << "ERROR::FREETYPE: Failed to load Glyph: " << c << std::endl;
				continue;
			}

			// Generate texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			characters.insert(std::pair<GLchar, Character>(c, character));
		}
	}

public:
	TextRenderer(std::string path, int size = 48) : fontpath(path), fontSize(size) {
		// Initialize Freetype library
		if (FT_Init_FreeType(&ft))
			FAIL("ERROR::FREETYPE: Could not init FreeType Library");
		// load the font as a face
		//if (FT_New_Face(ft, fontpath.c_str(), 0, &face))
		//	FAIL("ERROR::FREETYPE: Failed to load font");
		if (FT_New_Face(ft, "../Minimal/fonts/arial.ttf", 0, &face))
			FAIL("ERROR::FREETYPE: Failed to load font");

		//// set font size
		// width = 0 to dynamically calculate the width based on the given height
		FT_Set_Pixel_Sizes(face, 0, fontSize);

		loadAciiChars();

		// clean up
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		// init VAO, VBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// 2D quads require 6 vertices of 4 floats each
		// updating VBO's memory quite often, allocate the memory with GL_DYNAMIC_DRAW
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void renderText(
		const glm::mat4 & proj,
		std::string text,
		GLfloat x,
		GLfloat y,
		GLfloat scale,
		glm::vec3 color
	) {
		//glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		 // Activate corresponding render state	
		textShader.use();
		textShader.setMat4("projection", proj);
		textShader.setVec3("textColor", color);
		textShader.setFloat("zpos", 0.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		// Iterate through all characters
		for (char c : text) {
			//Character ch = characters[*c];
			Character ch = characters[c];

			GLfloat xpos = x + ch.bearing.x * scale;
			GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

			GLfloat w = ch.size.x * scale;
			GLfloat h = ch.size.y * scale;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.textureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void renderText(
		const glm::mat4 & proj,
		std::string text,
		glm::vec3 pos,
		GLfloat scale,
		glm::vec3 color
	) {
		//glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		 // Activate corresponding render state	
		textShader.use();
		textShader.setMat4("projection", proj);
		textShader.setVec3("textColor", color);
		textShader.setFloat("zpos", pos.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		// Iterate through all characters
		for (char c : text) {
			Character ch = characters[c];

			GLfloat xpos = pos.x + ch.bearing.x * scale;
			GLfloat ypos = pos.y - (ch.size.y - ch.bearing.y) * scale;

			GLfloat w = ch.size.x * scale;
			GLfloat h = ch.size.y * scale;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.textureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			pos.x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}

		//glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};

