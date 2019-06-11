#include "TexturedPlane.h"


TexturedPlane::TexturedPlane() : toWorld(glm::mat4(1.0f))
{
	using namespace PlaneData;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &tbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


TexturedPlane::~TexturedPlane()
{
}

void TexturedPlane::draw(const glm::mat4 & projection, const glm::mat4 & view, GLuint textureId, const glm::vec3 eyePos)
{
	shader.use();
	shader.setMat4("Projection", projection);
	shader.setMat4("View", view);
	shader.setMat4("Model", toWorld);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	shader.setInt("tex", 0);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

// basic lighting
void TexturedPlane::draw(const glm::mat4 & projection, const glm::mat4 & view, GLuint textureId)
{
	shader.use();
	shader.setMat4("Projection", projection);
	shader.setMat4("View", view);
	shader.setMat4("Model", toWorld);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	//shader.setInt("tex", 0);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void TexturedPlane::draw(GLuint shaderProgram, const glm::mat4 & projection, const glm::mat4 & view)
{
	glUseProgram(shaderProgram);
	glm::mat4 modelview = view * toWorld;

	GLuint uProjection = glGetUniformLocation(shaderProgram, "ProjectionMatrix");
	GLuint uModelview = glGetUniformLocation(shaderProgram, "CameraMatrix");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	glBindVertexArray(vao);

	//glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * 6); // 3 vertices per triangle, 2 triangles per face, 6 faces
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
