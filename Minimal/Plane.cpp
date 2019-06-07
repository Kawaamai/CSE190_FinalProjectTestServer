#include "Plane.h"


const float vertices[] = {
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
};

const float normals[] = {
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
};

const GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
};

Plane::Plane()
{
	toWorld = glm::mat4(1.0f);

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &normalBuffer);
	glGenBuffers(1, &ebo);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(
	//	0,
	//	// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
	//	3,
	//	// This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
	//	GL_FLOAT, // What type these components are
	//	GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
	//	3 * sizeof(GLfloat),
	//	// Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
	//	(GLvoid*)0);
	// Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	// FIXME: vertices rather than normals are being passed through for some reason
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(1);
	//glVertexAttribPointer(
	//	1,
	//	// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 1. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
	//	3,
	//	// This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
	//	GL_FLOAT, // What type these components are
	//	GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
	//	3 * sizeof(GLfloat),
	//	// Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
	//	(GLvoid*)0);
	//// Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(elements), elements, GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}


Plane::~Plane()
{
}

void Plane::draw(GLuint shaderProgram, const glm::mat4 & projection, const glm::mat4 & view)
{
	glUseProgram(shaderProgram);
	glm::mat4 modelview = view * toWorld;

	uProjection = glGetUniformLocation(shaderProgram, "ProjectionMatrix");
	uModelview = glGetUniformLocation(shaderProgram, "CameraMatrix");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	glBindVertexArray(VAO);

	// textures
	//if (textureSet)
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textureId);
	//glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0):

	//glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * 6); // 3 vertices per triangle, 2 triangles per face, 6 faces
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
