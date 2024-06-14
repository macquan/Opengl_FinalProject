#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <SFML\Audio.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture leafTexture;

DirectionalLight dLight(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "D:/VS 2022/OpenGL x64/Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "D:/VS 2022/OpenGL x64/Shaders/shader.frag";


void CreateSphere(float radius, int sectorCount, int stackCount, std::vector<Mesh*>& meshList) {
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	float x, y, z, xy; // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius; // vertex normal
	float s, t; // vertex texCoord

	float sectorStep = 2 * M_PI / sectorCount;
	float stackStep = M_PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i) {
		stackAngle = M_PI / 2 - i * stackStep;
		xy = 1.02f * radius * cosf(stackAngle);
		z = radius * sinf(stackAngle);

		for (int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep;

			x = xy * cosf(sectorAngle);
			y = xy * sinf(sectorAngle);
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			vertices.push_back(nx);
			vertices.push_back(ny);
			vertices.push_back(nz);

			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			vertices.push_back(s);
			vertices.push_back(t);
		}
	}

	int k1, k2;
	for (int i = 0; i < stackCount; ++i) {
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (stackCount - 1)) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	Mesh* obj = new Mesh();
	obj->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
	meshList.push_back(obj);
}




void CreateObjects()
{
	// Leaf
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		-1.0f, -1.0f, -0.6f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
		 0.0f, -1.0f,  1.0f,   0.5f, 0.0f,   0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -0.6f,   1.0f, 0.0f,   0.0f, 0.0f, 0.0f,
		 0.0f,  1.0f,  0.0f,   0.5f, 1.0f,   0.0f, 0.0f, 0.0f,
	};

	// Stem
	unsigned int box_indices[] = {
		0, 1, 2,
		1, 3, 2,
		4, 6, 5,
		5, 6, 7,
		0, 2, 4,
		2, 6, 4,
		1, 5, 3,
		3, 5, 7,
		0, 4, 1,
		1, 4, 5,
		2, 3, 6,
		3, 7, 6
	};

	GLfloat box_vertices[] = {
		-0.3f, -2.0f, -0.3f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
		-0.3f, -2.0f,  0.3f,   0.5f, 0.0f,   0.0f, 0.0f, 0.0f,
		 0.3f, -2.0f, -0.3f,   1.0f, 0.0f,   0.0f, 0.0f, 0.0f,
		 0.3f, -2.0f,  0.3f,   0.5f, 1.0f,   0.0f, 0.0f, 0.0f,
		-0.3f,  0.0f, -0.3f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
		-0.3f,  0.0f,  0.3f,   0.5f, 0.0f,   0.0f, 0.0f, 0.0f,
		 0.3f,  0.0f, -0.3f,   1.0f, 0.0f,   0.0f, 0.0f, 0.0f,
		 0.3f,  0.0f,  0.3f,   0.5f, 1.0f,   0.0f, 0.0f, 0.0f,
	};

	// Ground
	unsigned int floor_indices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floor_vertices[] = {
		-15.0f, -2.0f, -10.f,  0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		5.0f, -2.0f, -10.0f,  2.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		-15.0f, -2.0f, 10.0f,  0.0f, 2.0f,   0.0f, -1.0f, 0.0f,
		5.0f, -2.0f, 10.0f,   2.0f, 2.0f,   0.0f, -1.0f, 0.0f,
	};

	// House
	unsigned int base_indices[] = {
		// Bottom face
		0, 1, 2,
		1, 3, 2,

		// Top face
		4, 5, 6,
		5, 7, 6,

		// Front face
		8, 9, 10,
		8, 10, 11,

		// Back face
		12, 13, 14,
		12, 14, 15,

		// Left face
		16, 17, 18,
		17, 19, 18,

		// Right face
		20, 21, 22,
		21, 23, 22
	};



	GLfloat base_vertices[] = {
		// Bottom face
		-7.9f, -2.0f, -4.5f,    0.0f, 0.0f,  0.0f, 1.0f, 0.0f,// 0
		-7.9f, -2.0f, -1.5f,    1.0f, 0.0f,  0.0f, 1.0f, 0.0f,// 1
		-4.9f, -2.0f, -4.5f,    0.0f, 1.0f,  0.0f, 1.0f, 0.0f,// 2
		-4.9f, -2.0f, -1.5f,    1.0f, 1.0f,  0.0f, 1.0f, 0.0f,// 3

		// Top face
		-7.9f,  1.0f, -4.5f,    0.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 4
		-7.9f,  1.0f, -1.5f,    1.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 5
		-4.9f,  1.0f, -4.5f,    0.0f, 1.0f,  0.0f, 0.0f, 0.0f,// 6
		-4.9f,  1.0f, -1.5f,    1.0f, 1.0f,  0.0f, 0.0f, 0.0f,// 7

		// Front face
		-7.9f, -2.0f, -1.5f,    0.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 8
		-4.9f, -2.0f, -1.5f,    1.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 9
		-4.9f,  1.0f, -1.5f,    0.0f, 1.0f,  0.0f, 0.0f, 0.0f,// 10
		-7.9f,  1.0f, -1.5f,    1.0f, 1.0f,  0.0f, 0.0f, 0.0f,// 11

		// Back face
		-7.9f, -2.0f, -4.5f,    0.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 12
		-4.9f, -2.0f, -4.5f,    1.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 13
		-4.9f,  1.0f, -4.5f,    0.0f, 1.0f,  0.0f, 0.0f, 0.0f,// 14
		-7.9f,  1.0f, -4.5f,    1.0f, 1.0f,  0.0f, 0.0f, 0.0f,// 15

		// Left face
		-7.9f, -2.0f, -4.5f,    0.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 16
		-7.9f, -2.0f, -1.5f,    1.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 17
		-7.9f,  1.0f, -4.5f,    0.0f, 1.0f,  0.0f, 0.0f, 0.0f,// 18
		-7.9f,  1.0f, -1.5f,    1.0f, 1.0f,  0.0f, 0.0f, 0.0f,// 19

		// Right face
		-4.9f, -2.0f, -4.5f,    0.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 20
		-4.9f, -2.0f, -1.5f,    1.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 21
		-4.9f,  1.0f, -4.5f,    0.0f, 1.0f,  0.0f, 0.0f, 0.0f,// 22
		-4.9f,  1.0f, -1.5f,    1.0f, 1.0f,  0.0f, 0.0f, 0.0f// 23
	};




	unsigned int roof_indices[] = {
		0, 2, 4,    // Front left triangle
		1, 3, 4,    // Front right triangle
		2, 3, 4,    // Back right triangle
		0, 1, 4     // Back left triangle
	};


	GLfloat roof_vertices[] = {
		// Roof vertices (4 base vertices and 1 top vertex)
		-7.9f, 1.0f, -4.5f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f, // 0
		-7.9f, 1.0f, -1.5f,   3.0f, 0.0f,   0.0f, 0.0f, 0.0f, // 1
		-4.9f, 1.0f, -4.5f,   3.0f, 3.0f,   0.0f, 0.0f, 0.0f, // 2
		-4.9f, 1.0f, -1.5f,   0.0f, 3.0f,   0.0f, 0.0f, 0.0f, // 3
		-6.4f, 2.5f, -3.0f,   1.5f, 4.5f,   0.0f, 0.0f, 0.0f  // 4
	};

	// door
	unsigned int door_indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	GLfloat door_vertices[] = {
		// Front face
		-7.0f, -2.0f, -1.49f,    0.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 0
		-5.7f, -2.0f, -1.49f,    1.0f, 0.0f,  0.0f, 0.0f, 0.0f,// 1
		-5.7f,  0.0f, -1.49f,    1.0f, 1.0f,  0.0f, 0.0f, 0.0f,// 2
		-7.0f,  0.0f, -1.49f,    0.0f, 1.0f,  0.0f, 0.0f, 0.0f,// 3
	};


	//big house
	//base
	unsigned int houseIndices[] = {
		// Indices for left (0 offset)
		0, 1, 2,
		0, 2, 3,

		// Indices for right (4 offset)
		4, 5, 6,
		4, 6, 7,

		// Indices for back (8 offset)
		8, 9, 10,
		8, 10, 11,

		// Indices for front (12 offset)
		12, 13, 14,
		12, 14, 15,

		// Indices for front1 (16 offset)
		16, 17, 18,
		16, 18, 19,

		// Indices for front2 (20 offset)
		20, 21, 22,
		20, 22, 23,

		// Indices for bottom (24 offset)
		24, 25, 26,
		24, 26, 27,
	};


	float scale = 0.5f;
	GLfloat houseVertices[] = {
		// left
		-7.0f + 10.0f * scale, -2.0f + 0.0f * scale, -15.0f * scale, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + 10.0f * scale, -2.0f + 0.0f * scale, 10.0f * scale, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + 10.0f * scale, -2.0f + 10.0f * scale, 10.0f * scale, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + 10.0f * scale, -2.0f + 10.0f * scale, -15.0f * scale, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		// right
		-7.0f + -10.0f * scale, -2.0f + 0.0f * scale, -15.0f * scale, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + -10.0f * scale, -2.0f + 0.0f * scale, 10.0f * scale, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + -10.0f * scale, -2.0f + 10.0f * scale, 10.0f * scale, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + -10.0f * scale, -2.0f + 10.0f * scale, -15.0f * scale, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		// back
		-7.0f + 10.0f * scale, -2.0f + 0.0f * scale, -15.0f * scale, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + -10.0f * scale, -2.0f + 0.0f * scale, -15.0f * scale, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + -10.0f * scale, -2.0f + 10.0f * scale, -15.0f * scale, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + 10.0f * scale, -2.0f + 10.0f * scale, -15.0f * scale, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		// front
		-7.0f + -10.0f * scale, -2.0f + 0.0f * scale, 10.0f * scale, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + -10.0f * scale, -2.0f + 10.0f * scale, 10.0f * scale, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + -1.5f * scale, -2.0f + 10.0f * scale, 10.0f * scale, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + -1.5f * scale, -2.0f + 0.0f * scale, 10.0f * scale, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,

		// front1
		-7.0f + 1.5f * scale, -2.0f + 0.0f * scale, 10.0f * scale, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + 1.5f * scale, -2.0f + 10.0f * scale, 10.0f * scale, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + 10.0f * scale, -2.0f + 10.0f * scale, 10.0f * scale, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + 10.0f * scale, -2.0f + 0.0f * scale, 10.0f * scale, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,

		// front2
		-7.0f + -1.5f * scale, -2.0f + 6.0f * scale, 10.0f * scale, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + -1.5f * scale, -2.0f + 10.0f * scale, 10.0f * scale, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + 1.5f * scale, -2.0f + 10.0f * scale, 10.0f * scale, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-7.0f + 1.5f * scale, -2.0f + 6.0f * scale, 10.0f * scale, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,

		// bottom
		-7.0f + 10.0f * scale, -2.0f + 0.0f * scale, -15.0f * scale, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-7.0f + -10.0f * scale, -2.0f + 0.0f * scale, -15.0f * scale, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-7.0f + -10.0f * scale, -2.0f + 0.0f * scale, 10.0f * scale, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-7.0f + 10.0f * scale, -2.0f + 0.0f * scale, 10.0f * scale, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	};


	//roof
	unsigned int big_indices[] = {
		0, 2, 4,    // Front left triangle
		1, 3, 4,    // Front right triangle
		2, 3, 4,    // Back right triangle
		0, 1, 4     // Back left triangle
	};


	GLfloat big_vertices[] = {
		// Roof vertices (4 base vertices and 1 top vertex)
		-12.0f, 3.0f, -7.5f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f, // 0 Back left
		-2.0f,  3.0f, -7.5f,  3.0f, 0.0f,  0.0f, 0.0f, 0.0f, // 1 Back right
		-12.0f, 3.0f,  5.0f,  3.0f, 3.0f,  0.0f, 0.0f, 0.0f, // 2 Front left
		-2.0f,  3.0f,  5.0f,  0.0f, 3.0f,  0.0f, 0.0f, 0.0f, // 3 Front right
		-7.0f,  6.5f, -1.25f, 1.5f, 4.5f,  0.0f, 0.0f, 0.0f  // 4 Peak
	};

	//table
	float tableScale = 2.0f;
	GLfloat tableVertices[] = {
		// Table top (cube)
		-1.0f * tableScale, 0.1f * tableScale, -0.5f * tableScale,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top left front
		 1.0f * tableScale, 0.1f * tableScale, -0.5f * tableScale,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Top right front
		 1.0f * tableScale, 0.1f * tableScale,  0.5f * tableScale,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Top right back
		-1.0f * tableScale, 0.1f * tableScale,  0.5f * tableScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top left back
		-1.0f * tableScale, 0.0f * tableScale, -0.5f * tableScale,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom left front
		 1.0f * tableScale, 0.0f * tableScale, -0.5f * tableScale,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom right front
		 1.0f * tableScale, 0.0f * tableScale,  0.5f * tableScale,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom right back
		-1.0f * tableScale, 0.0f * tableScale,  0.5f * tableScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom left back

		// Table legs (cubes) - 4 legs
		// Front left leg
		-1.0f * tableScale, -1.0f * tableScale, -0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front left
		-0.8f * tableScale, -1.0f * tableScale, -0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front right
		-0.8f * tableScale,  0.0f * tableScale, -0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front right
		-1.0f * tableScale,  0.0f * tableScale, -0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front left
		-1.0f * tableScale, -1.0f * tableScale, -0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back left
		-0.8f * tableScale, -1.0f * tableScale, -0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back right
		-0.8f * tableScale,  0.0f * tableScale, -0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back right
		-1.0f * tableScale,  0.0f * tableScale, -0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back left

		// Similar for the other three legs
		// Front right leg
		 1.0f * tableScale, -1.0f * tableScale, -0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front right
		 0.8f * tableScale, -1.0f * tableScale, -0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front left
		 0.8f * tableScale,  0.0f * tableScale, -0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front left
		 1.0f * tableScale,  0.0f * tableScale, -0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front right
		 1.0f * tableScale, -1.0f * tableScale, -0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back right
		 0.8f * tableScale, -1.0f * tableScale, -0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back left
		 0.8f * tableScale,  0.0f * tableScale, -0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back left
		 1.0f * tableScale,  0.0f * tableScale, -0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back right

		 // Back left leg
		 -1.0f * tableScale, -1.0f * tableScale,  0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back left
		 -0.8f * tableScale, -1.0f * tableScale,  0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back right
		 -0.8f * tableScale,  0.0f * tableScale,  0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back right
		 -1.0f * tableScale,  0.0f * tableScale,  0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back left
		 -1.0f * tableScale, -1.0f * tableScale,  0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front left
		 -0.8f * tableScale, -1.0f * tableScale,  0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front right
		 -0.8f * tableScale,  0.0f * tableScale,  0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front right
		 -1.0f * tableScale,  0.0f * tableScale,  0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front left

		 // Back right leg
		  1.0f * tableScale, -1.0f * tableScale,  0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back right
		  0.8f * tableScale, -1.0f * tableScale,  0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back left
		  0.8f * tableScale,  0.0f * tableScale,  0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back left
		  1.0f * tableScale,  0.0f * tableScale,  0.5f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back right
		  1.0f * tableScale, -1.0f * tableScale,  0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front right
		  0.8f * tableScale, -1.0f * tableScale,  0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front left
		  0.8f * tableScale,  0.0f * tableScale,  0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front left
		  1.0f * tableScale,  0.0f * tableScale,  0.3f * tableScale, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f  // Top front right
	};


	unsigned int tableIndices[] = {
		// Table top (cube)
		0, 1, 2,  0, 2, 3, // Top face
		0, 3, 7,  0, 7, 4, // Left face
		1, 2, 6,  1, 6, 5, // Right face
		3, 2, 6,  3, 6, 7, // Back face
		0, 1, 5,  0, 5, 4, // Front face
		4, 5, 6,  4, 6, 7, // Bottom face

		// Table legs (cubes)
		// Front left leg
		8, 9, 10,  8, 10, 11, // Front face
		8, 11, 15,  8, 15, 12, // Left face
		9, 10, 14,  9, 14, 13, // Right face
		11, 10, 14,  11, 14, 15, // Back face
		8, 9, 13,  8, 13, 12, // Bottom face
		12, 13, 14,  12, 14, 15, // Top face

		// Front right leg
		16, 17, 18,  16, 18, 19, // Front face
		16, 19, 23,  16, 23, 20, // Left face
		17, 18, 22,  17, 22, 21, // Right face
		19, 18, 22,  19, 22, 23, // Back face
		16, 17, 21,  16, 21, 20, // Bottom face
		20, 21, 22,  20, 22, 23, // Top face

		// Back left leg
		24, 25, 26,  24, 26, 27, // Front face
		24, 27, 31,  24, 31, 28, // Left face
		25, 26, 30,  25, 30, 29, // Right face
		27, 26, 30,  27, 30, 31, // Back face
		24, 25, 29,  24, 29, 28, // Bottom face
		28, 29, 30,  28, 30, 31, // Top face

		// Back right leg
		32, 33, 34,  32, 34, 35, // Front face
		32, 35, 39,  32, 39, 36, // Left face
		33, 34, 38,  33, 38, 37, // Right face
		35, 34, 38,  35, 38, 39, // Back face
		32, 33, 37,  32, 37, 36, // Bottom face
		36, 37, 38,  36, 38, 39  // Top face
	};


	//chair
	float chairScale = 1.0f;
	GLfloat chairVertices[] = {
		// Chair seat (cube)
		-0.5f * chairScale, 0.1f * chairScale, -0.5f * chairScale,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top left front
		 0.5f * chairScale, 0.1f * chairScale, -0.5f * chairScale,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Top right front
		 0.5f * chairScale, 0.1f * chairScale,  0.5f * chairScale,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Top right back
		-0.5f * chairScale, 0.1f * chairScale,  0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top left back
		-0.5f * chairScale, 0.0f * chairScale, -0.5f * chairScale,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom left front
		 0.5f * chairScale, 0.0f * chairScale, -0.5f * chairScale,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom right front
		 0.5f * chairScale, 0.0f * chairScale,  0.5f * chairScale,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom right back
		-0.5f * chairScale, 0.0f * chairScale,  0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom left back

		// Chair legs (cubes)
		// Front left leg
		-0.5f * chairScale, -0.5f * chairScale, -0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front left
		-0.4f * chairScale, -0.5f * chairScale, -0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front right
		-0.4f * chairScale,  0.0f * chairScale, -0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front right
		-0.5f * chairScale,  0.0f * chairScale, -0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front left
		-0.5f * chairScale, -0.5f * chairScale, -0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back left
		-0.4f * chairScale, -0.5f * chairScale, -0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back right
		-0.4f * chairScale,  0.0f * chairScale, -0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back right
		-0.5f * chairScale,  0.0f * chairScale, -0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back left

		// Front right leg
		 0.5f * chairScale, -0.5f * chairScale, -0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front right
		 0.4f * chairScale, -0.5f * chairScale, -0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front left
		 0.4f * chairScale,  0.0f * chairScale, -0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front left
		 0.5f * chairScale,  0.0f * chairScale, -0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front right
		 0.5f * chairScale, -0.5f * chairScale, -0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back right
		 0.4f * chairScale, -0.5f * chairScale, -0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back left
		 0.4f * chairScale,  0.0f * chairScale, -0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back left
		 0.5f * chairScale,  0.0f * chairScale, -0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back right

		 // Back left leg
		 -0.5f * chairScale, -0.5f * chairScale,  0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back left
		 -0.4f * chairScale, -0.5f * chairScale,  0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back right
		 -0.4f * chairScale,  0.0f * chairScale,  0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back right
		 -0.5f * chairScale,  0.0f * chairScale,  0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back left
		 -0.5f * chairScale, -0.5f * chairScale,  0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front left
		 -0.4f * chairScale, -0.5f * chairScale,  0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front right
		 -0.4f * chairScale,  0.0f * chairScale,  0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front right
		 -0.5f * chairScale,  0.0f * chairScale,  0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front left

		 // Back right leg
		  0.5f * chairScale, -0.5f * chairScale,  0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back right
		  0.4f * chairScale, -0.5f * chairScale,  0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom back left
		  0.4f * chairScale,  0.0f * chairScale,  0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back left
		  0.5f * chairScale,  0.0f * chairScale,  0.5f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top back right
		  0.5f * chairScale, -0.5f * chairScale,  0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front right
		  0.4f * chairScale, -0.5f * chairScale,  0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom front left
		  0.4f * chairScale,  0.0f * chairScale,  0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top front left
		  0.5f * chairScale,  0.0f * chairScale,  0.4f * chairScale,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f  // Top front right
	};

	unsigned int chairIndices[] = {
		// Chair seat (cube)
		0, 1, 2,  0, 2, 3, // Top face
		0, 3, 7,  0, 7, 4, // Left face
		1, 2, 6,  1, 6, 5, // Right face
		3, 2, 6,  3, 6, 7, // Back face
		0, 1, 5,  0, 5, 4, // Front face
		4, 5, 6,  4, 6, 7, // Bottom face

		// Chair legs (cubes)
		// Front left leg
		8, 9, 10,  8, 10, 11, // Front face
		8, 11, 15,  8, 15, 12, // Left face
		9, 10, 14,  9, 14, 13, // Right face
		11, 10, 14,  11, 14, 15, // Back face
		8, 9, 13,  8, 13, 12, // Bottom face
		12, 13, 14,  12, 14, 15, // Top face

		// Front right leg
		16, 17, 18,  16, 18, 19, // Front face
		16, 19, 23,  16, 23, 20, // Left face
		17, 18, 22,  17, 22, 21, // Right face
		19, 18, 22,  19, 22, 23, // Back face
		16, 17, 21,  16, 21, 20, // Bottom face
		20, 21, 22,  20, 22, 23, // Top face

		// Back left leg
		24, 25, 26,  24, 26, 27, // Front face
		24, 27, 31,  24, 31, 28, // Left face
		25, 26, 30,  25, 30, 29, // Right face
		27, 26, 30,  27, 30, 31, // Back face
		24, 25, 29,  24, 29, 28, // Bottom face
		28, 29, 30,  28, 30, 31, // Top face

		// Back right leg
		32, 33, 34,  32, 34, 35, // Front face
		32, 35, 39,  32, 39, 36, // Left face
		33, 34, 38,  33, 38, 37, // Right face
		35, 34, 38,  35, 38, 39, // Back face
		32, 33, 37,  32, 37, 36, // Bottom face
		36, 37, 38,  36, 38, 39  // Top face
	};

	//bed
	float bedScale = 1.0f;
	float bedVertices[] = {
		// Bed body (cube)
		// positions        // normals         // texture coords
		-1.0f * bedScale,  0.0f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left front
		 1.0f * bedScale,  0.0f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right front
		 1.0f * bedScale,  0.5f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right front
		-1.0f * bedScale,  0.5f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left front
		-1.0f * bedScale,  0.0f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left back
		 1.0f * bedScale,  0.0f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right back
		 1.0f * bedScale,  0.5f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right back
		-1.0f * bedScale,  0.5f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left back

		// Bed legs (cubes)
		// Front left leg
		-1.0f * bedScale, -0.5f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left front
		-0.9f * bedScale, -0.5f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right front
		-0.9f * bedScale,  0.0f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right front
		-1.0f * bedScale,  0.0f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left front
		-1.0f * bedScale, -0.5f * bedScale, -1.9f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left back
		-0.9f * bedScale, -0.5f * bedScale, -1.9f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right back
		-0.9f * bedScale,  0.0f * bedScale, -1.9f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right back
		-1.0f * bedScale,  0.0f * bedScale, -1.9f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left back

		// Front right leg
		 1.0f * bedScale, -0.5f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom right front
		 0.9f * bedScale, -0.5f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom left front
		 0.9f * bedScale,  0.0f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top left front
		 1.0f * bedScale,  0.0f * bedScale, -2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top right front
		 1.0f * bedScale, -0.5f * bedScale, -1.9f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom right back
		 0.9f * bedScale, -0.5f * bedScale, -1.9f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom left back
		 0.9f * bedScale,  0.0f * bedScale, -1.9f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top left back
		 1.0f * bedScale,  0.0f * bedScale, -1.9f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top right back

		 // Back left leg
		 -1.0f * bedScale, -0.5f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left back
		 -0.9f * bedScale, -0.5f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right back
		 -0.9f * bedScale,  0.0f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right back
		 -1.0f * bedScale,  0.0f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left back
		 -1.0f * bedScale, -0.5f * bedScale,  1.9f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left back
		 -0.9f * bedScale, -0.5f * bedScale,  1.9f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right back
		 -0.9f * bedScale,  0.0f * bedScale,  1.9f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right back
		 -1.0f * bedScale,  0.0f * bedScale,  1.9f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left back

		 // Back right leg
		  1.0f * bedScale, -0.5f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom right back
		  0.9f * bedScale, -0.5f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom left back
		  0.9f * bedScale,  0.0f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top left back
		  1.0f * bedScale,  0.0f * bedScale,  2.0f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top right back
		  1.0f * bedScale, -0.5f * bedScale,  1.9f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom right back
		  0.9f * bedScale, -0.5f * bedScale,  1.9f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom left back
		  0.9f * bedScale,  0.0f * bedScale,  1.9f * bedScale,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top left back
		  1.0f * bedScale,  0.0f * bedScale,  1.9f * bedScale,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f  // Top right back
	};


	unsigned int bedIndices[] = {
		// Bed body (cube)
		0, 1, 2,  0, 2, 3, // Bottom face
		0, 3, 7,  0, 7, 4, // Left face
		1, 2, 6,  1, 6, 5, // Right face
		3, 2, 6,  3, 6, 7, // Back face
		0, 1, 5,  0, 5, 4, // Front face
		4, 5, 6,  4, 6, 7, // Top face

		// Front left leg
		8, 9, 10,  8, 10, 11, // Bottom face
		8, 11, 15,  8, 15, 12, // Left face
		9, 10, 14,  9, 14, 13, // Right face
		11, 10, 14,  11, 14, 15, // Back face
		8, 9, 13,  8, 13, 12, // Front face
		12, 13, 14,  12, 14, 15, // Top face

		// Front right leg
		16, 17, 18,  16, 18, 19, // Bottom face
		16, 19, 23,  16, 23, 20, // Left face
		17, 18, 22,  17, 22, 21, // Right face
		19, 18, 22,  19, 22, 23, // Back face
		16, 17, 21,  16, 21, 20, // Front face
		20, 21, 22,  20, 22, 23, // Top face

		// Back left leg
		24, 25, 26,  24, 26, 27, // Bottom face
		24, 27, 31,  24, 31, 28, // Left face
		25, 26, 30,  25, 30, 29, // Right face
		27, 26, 30,  27, 30, 31, // Back face
		24, 25, 29,  24, 29, 28, // Front face
		28, 29, 30,  28, 30, 31, // Top face

		// Back right leg
		32, 33, 34,  32, 34, 35, // Bottom face
		32, 35, 39,  32, 39, 36, // Left face
		33, 34, 38,  33, 38, 37, // Right face
		35, 34, 38,  35, 38, 39, // Back face
		32, 33, 37,  32, 37, 36, // Front face
		36, 37, 38,  36, 38, 39  // Top face
	};

	//PC
	float monitorVertices[] = {
		// positions                  // normals           // texture coords
		-1.0f,  0.5f, -0.1f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left front
		 1.0f,  0.5f, -0.1f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right front
		 1.0f,  2.0f, -0.1f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right front
		-1.0f,  2.0f, -0.1f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left front
		-1.0f,  0.5f,  0.1f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left back
		 1.0f,  0.5f,  0.1f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right back
		 1.0f,  2.0f,  0.1f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right back
		-1.0f,  2.0f,  0.1f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left back
	};

	unsigned int monitorIndices[] = {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		0, 1, 5, 0, 5, 4,
		1, 2, 6, 1, 6, 5,
		2, 3, 7, 2, 7, 6,
		3, 0, 4, 3, 4, 7,
	};

	float cpuVertices[] = {
		// positions                  // normals           // texture coords
		-0.5f, -1.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left front
		 0.5f, -1.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right front
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right front
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left front
		-0.5f, -1.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left back
		 0.5f, -1.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right back
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right back
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left back
	};

	unsigned int cpuIndices[] = {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		0, 1, 5, 0, 5, 4,
		1, 2, 6, 1, 6, 5,
		2, 3, 7, 2, 7, 6,
		3, 0, 4, 3, 4, 7,
	};

	float keyboardVertices[] = {
		// positions                  // normals           // texture coords
		-1.5f, -2.5f, -0.1f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left front
		 1.5f, -2.5f, -0.1f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right front
		 1.5f, -2.3f, -0.1f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right front
		-1.5f, -2.3f, -0.1f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left front
		-1.5f, -2.5f,  0.1f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Bottom left back
		 1.5f, -2.5f,  0.1f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom right back
		 1.5f, -2.3f,  0.1f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top right back
		-1.5f, -2.3f,  0.1f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top left back
	};

	unsigned int keyboardIndices[] = {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		0, 1, 5, 0, 5, 4,
		1, 2, 6, 1, 6, 5,
		2, 3, 7, 2, 7, 6,
		3, 0, 4, 3, 4, 7,
	};





	Mesh* obj0 = new Mesh();
	obj0->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj0);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(box_vertices, box_indices, 64, 36);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(floor_vertices, floor_indices, 32, 6);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(base_vertices, base_indices, 192, 36);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(roof_vertices, roof_indices, 40, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(door_vertices, door_indices, 32, 6);
	meshList.push_back(obj5);

	//Sun
	CreateSphere(5.0f, 256, 256, meshList); //obj6

	//SkyBox
	CreateSphere(50.0f, 256, 256, meshList); //obj7

	//Tuong
	Mesh* obj8 = new Mesh();
	obj8->CreateMesh(houseVertices, houseIndices, 224, 42);
	meshList.push_back(obj8);

	Mesh* obj9 = new Mesh();
	obj9->CreateMesh(big_vertices, big_indices, 40, 12);
	meshList.push_back(obj9);

	Mesh* obj10 = new Mesh();
	obj10->CreateMesh(tableVertices, tableIndices, 320, 180);
	meshList.push_back(obj10);

	Mesh* obj11 = new Mesh();
	obj11->CreateMesh(chairVertices, chairIndices, 320, 180);
	meshList.push_back(obj11);

	Mesh* obj12 = new Mesh();
	obj12->CreateMesh(bedVertices, bedIndices, 320, 180);
	meshList.push_back(obj12);

	Mesh* obj13 = new Mesh();
	obj13->CreateMesh(monitorVertices, monitorIndices, 64, 36);
	meshList.push_back(obj13);

	Mesh* obj14 = new Mesh();
	obj14->CreateMesh(cpuVertices, cpuIndices, 64, 36);
	meshList.push_back(obj14);

	Mesh* obj15 = new Mesh();
	obj15->CreateMesh(keyboardVertices, keyboardIndices, 64, 36);
	meshList.push_back(obj15);


}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(2560, 1600);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	sf::Music music;
	if (!music.openFromFile("Music/music.ogg"))
	{
		std::cerr << "Failed to load music" << std::endl;
		return -1;
	}
	music.play();

	camera = Camera(glm::vec3(0.0f, 5.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 10.0f, 0.2f);

	leafTexture = Texture("Textures/leaf.jpg");
	leafTexture.LoadTexture();

	Texture woodTexture("Textures/wood.jpg");
	woodTexture.LoadTexture();

	Texture groundTexture("Textures/ground1.jpg");
	groundTexture.LoadTexture();

	Texture baseTexture("Textures/base.jpg");
	baseTexture.LoadTexture();

	Texture base1Texture("Textures/base1.jpg");
	base1Texture.LoadTexture();

	Texture roofTexture("Textures/roof.jpg");
	roofTexture.LoadTexture();

	Texture roof1Texture("Textures/roof1.jpg");
	roof1Texture.LoadTexture();

	Texture doorTexture("Textures/door.jpg");
	doorTexture.LoadTexture();

	Texture sunTexture("Textures/sun1.jpg");
	sunTexture.LoadTexture();

	Texture skyTexture("Textures/sky_box.jpg");
	skyTexture.LoadTexture();

	Texture chairTexture("Textures/chair.jpg");
	chairTexture.LoadTexture();

	Texture tableTexture("Textures/table.jpg");
	tableTexture.LoadTexture();

	Texture bedTexture("Textures/bed.jpg");
	bedTexture.LoadTexture();

	Texture pcTexture("Textures/pc.jpg");
	pcTexture.LoadTexture();

	
	PointLight pLight(0.5f, 0.5f, 1.0f, 0.5f, 1.0f, 1.0f);
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformAmbientIntensity = 0, uniformAmbientColour = 0, uniformDiffuseIntensity = 0, uniformLightPosition = 0, uniformLightConstant = 0, uniformLightLinear = 0, uniformLightQuadratic = 0, uniformLightDirection = 0, uniformCameraPos = 0, uniformSpecularIntensity = 0;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	float* ambient = dLight.getAmbientIntensity();

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime, ambient);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformAmbientColour = shaderList[0].GetAmbientColourLocation();
		uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();
		uniformDiffuseIntensity = shaderList[0].GetDiffuseIntensityLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformLightDirection = shaderList[0].GetLightDirectionLocation();

		dLight.UseDirLight(uniformAmbientIntensity, uniformAmbientColour,
			uniformDiffuseIntensity, uniformSpecularIntensity, uniformLightDirection);

		glm::mat4 model(1.0f);

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		leafTexture.UseTexture();
		meshList[0]->RenderMesh();

		for (int i = 0; i < 5; ++i) {
			model = glm::mat4(1.0f);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 5.0f));
			model = glm::rotate(model, glm::radians(i * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			leafTexture.UseTexture();
			meshList[0]->RenderMesh();
		}

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		woodTexture.UseTexture();
		meshList[1]->RenderMesh();


		model = glm::mat4(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		groundTexture.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(9.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		baseTexture.UseTexture();
		meshList[3]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(9.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		roofTexture.UseTexture();
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(9.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		doorTexture.UseTexture();
		meshList[5]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 20.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		sunTexture.UseTexture();
		meshList[6]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		skyTexture.UseTexture();
		meshList[7]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.02f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		base1Texture.UseTexture();
		meshList[8]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		roof1Texture.UseTexture();
		meshList[9]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-9.5f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		tableTexture.UseTexture();
		meshList[10]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, -1.5f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		chairTexture.UseTexture();
		meshList[11]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.5f, -1.5f, -5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		bedTexture.UseTexture();
		meshList[12]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-9.5f, -1.05f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		pcTexture.UseTexture();
		meshList[13]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-11.0f, 0.35f, 0.8f));
		model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		pcTexture.UseTexture();
		meshList[14]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 0.35f, 0.8f));
		model = glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		pcTexture.UseTexture();
		meshList[14]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-9.5f, 1.7f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		pcTexture.UseTexture();
		meshList[15]->RenderMesh();



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}