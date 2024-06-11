//#define STB_IMAGE_IMPLEMENTATION

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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"
#include "load_model_meshes.h"
#include "shader_configure.h" // Used to create the model shaders.

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture leafTexture;

DirectionalLight dLight(1.0f, 1.0f, 1.0f, 0.5f, 0.8f, 1.0f);

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

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
		-10.0f, -2.0f, -10.f,  0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		10.0f, -2.0f, -10.0f,  2.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		-10.0f, -2.0f, 10.0f,  0.0f, 2.0f,   0.0f, -1.0f, 0.0f,
		10.0f, -2.0f, 10.0f,   2.0f, 2.0f,   0.0f, -1.0f, 0.0f,
	};

	unsigned int base_indices[] = {
		0, 1, 2,    1, 3, 2,    // Bottom face
		4, 5, 6,    5, 7, 6,    // Top face
		0, 2, 4,    2, 6, 4,    // Front face
		1, 5, 3,    3, 5, 7,    // Back face
		0, 4, 1,    1, 4, 5,    // Left face
		2, 3, 6,    3, 7, 6     // Right face
	};


	GLfloat base_vertices[] = {
		// Base vertices (8 vertices for the rectangular prism base)
		-7.9f, -2.0f, -4.5f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f, // 0
		-7.9f, -2.0f, -1.5f,   1.5f, 0.0f,   0.0f, 0.0f, 0.0f, // 1
		-4.9f, -2.0f, -4.5f,   3.0f, 0.0f,   0.0f, 0.0f, 0.0f, // 2
		-4.9f, -2.0f, -1.5f,   1.5f, 3.0f,   0.0f, 0.0f, 0.0f, // 3
		-7.9f, 1.0f, -4.5f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f, // 4
		-7.9f, 1.0f, -1.5f,   1.5f, 0.0f,   0.0f, 0.0f, 0.0f, // 5
		-4.9f, 1.0f, -4.5f,   3.0f, 0.0f,   0.0f, 0.0f, 0.0f, // 6
		-4.9f, 1.0f, -1.5f,   1.5f, 3.0f,   0.0f, 0.0f, 0.0f, // 7
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
		0, 1, 5,  // Front face of the door
		0, 5, 4,  // Top face of the door
		1, 3, 7,  // Right face of the door
		1, 7, 5   // Back face of the door
	};

	GLfloat door_vertices[] = {
		// Door vertices (8 vertices for the door)
		-7.0f, -2.0f, -3.01f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f, // 0
		-7.0f, -2.0f,  -1.49f,   1.0f, 0.0f,   0.0f, 0.0f, 0.0f, // 1
		-7.0f, 0.0f, -3.01f,   0.0f, 1.0f,   0.0f, 0.0f, 0.0f, // 2
		-7.0f, 0.0f,  -1.49f,   1.0f, 1.0f,   0.0f, 0.0f, 0.0f, // 3
		-5.7f, -2.0f, -3.01f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f, // 4
		-5.7f, -2.0f,  -1.49f,   1.0f, 0.0f,   0.0f, 0.0f, 0.0f, // 5
		-5.7f, 0.0f, -3.01f,   0.0f, 1.0f,   0.0f, 0.0f, 0.0f, // 6
		-5.7f, 0.0f,  -1.49f,   1.0f, 1.0f,   0.0f, 0.0f, 0.0f  // 7
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
	obj3->CreateMesh(base_vertices, base_indices, 64, 36);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(roof_vertices, roof_indices, 40, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(door_vertices, door_indices, 64, 12);
	meshList.push_back(obj5);


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


	camera = Camera(glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 10.0f, 0.2f);

	leafTexture = Texture("Textures/leaf.jpg");
	leafTexture.LoadTexture();

	Texture woodTexture("Textures/wood.jpg");
	woodTexture.LoadTexture();

	Texture groundTexture("Textures/ground.jpg");
	groundTexture.LoadTexture();

	Texture baseTexture("Textures/base.jpg");
	baseTexture.LoadTexture();

	Texture roofTexture("Textures/roof.jpg");
	roofTexture.LoadTexture();

	Texture doorTexture("Textures/door.jpg");
	doorTexture.LoadTexture();

	Model dragon("Models/Bambo_House.obj");
	glActiveTexture(GL_TEXTURE0); // Reusing the same texture unit for each model mesh.


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformAmbientIntensity = 0,
		uniformAmbientColour = 0, uniformDiffuseIntensity = 0, uniformSpecularIntensity = 0, uniformLightDirection = 0;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
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
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		leafTexture.UseTexture();
		meshList[0]->RenderMesh();

		for (int i = 0; i < 5; ++i) {
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(i * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			leafTexture.UseTexture();
			meshList[0]->RenderMesh();
		}

		model = glm::mat4(1.0f);
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
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		baseTexture.UseTexture();
		meshList[3]->RenderMesh();

		model = glm::mat4(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		roofTexture.UseTexture();
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		doorTexture.UseTexture();
		meshList[5]->RenderMesh();

		for (unsigned int i = 0; i < dragon.num_meshes; ++i) {\
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glBindTexture(GL_TEXTURE_2D, dragon.mesh_list[i].tex_handle);
			// Bind the VAO of the current mesh
			glBindVertexArray(dragon.mesh_list[i].VAO);
			// Draw the mesh
			glDrawElements(GL_TRIANGLES, (GLsizei)dragon.mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);

			// Unbind the VAO
			glBindVertexArray(0);
		}



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}