#pragma once

#include <GL\glew.h>
#include <vector>
#include <glm/glm.hpp>
class Mesh
{
public:
	Mesh();

	void renderPoint();

	void createPointMesh(std::vector<glm::vec3> points);

	void CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices);
	void RenderMesh();
	void ClearMesh();

	~Mesh();

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};

