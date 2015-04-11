#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#include <string>
#include <assimp/importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/vector3.h>
#include "BasicMesh.hpp"

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}



GLint program;
GLint program2;
glm::vec3 eye;
bool wireframe(0);
bool shader(0);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (!wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframe = true;
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframe = false;
		}
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		if (!shader)
		{
			glUseProgram(program);
			shader = true;

			GLint location2 = glGetUniformLocation(program, "inputColor");
			glUniform4f(location2, 1.0, 1.0, 1.0, 1.0);

			GLint location3 = glGetUniformLocation(program, "lightDir");
			glUniform4f(location3, 1.0, 1.0, 1.0, 0.0);

			GLint location4 = glGetUniformLocation(program, "viewDir");
			glUniform4f(location4, eye.x, eye.y, eye.z, 0.0);
		}
		else
		{
			glUseProgram(program2);
			shader = false;

			GLint location2 = glGetUniformLocation(program2, "inputColor");
			glUniform4f(location2, 1.0, 1.0, 1.0, 1.0);

			GLint location3 = glGetUniformLocation(program2, "lightDir");
			glUniform4f(location3, 1.0, 1.0, 1.0, 0.0);

			GLint location4 = glGetUniformLocation(program2, "viewDir");
			glUniform4f(location4, eye.x, eye.y, eye.z, 0.0);
		}
	}

}

BasicMesh loadMesh(std::string filename)
{
	bool ret(false);
	Assimp::Importer Importer;
	const aiScene* pScene = Importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	if (pScene) 
	{

		const aiMesh* paiMesh = pScene->mMeshes[0];
		std::vector<float> verts;
		std::vector<float> norms;
		std::vector<float> uvCoords;
		std::vector<size_t> inds;

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		//load vertices and normals
		for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) 
		{
			const aiVector3D* pPos = &(paiMesh->mVertices[i]);
			const aiVector3D* pNorm = &(paiMesh->mNormals[i]);
			const aiVector3D* pUV = &(paiMesh->mTextureCoords[0][i]);

			verts.push_back(pPos->x); verts.push_back(pPos->y); verts.push_back(pPos->z);
			norms.push_back(pNorm->x); norms.push_back(pNorm->y); norms.push_back(pNorm->z);
			uvCoords.push_back(pUV->x); uvCoords.push_back(pUV->y);

		}

		//Load indices
		for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) 
		{
			const aiFace& Face = paiMesh->mFaces[i];
			assert(Face.mNumIndices == 3);
			inds.push_back(Face.mIndices[0]);
			inds.push_back(Face.mIndices[1]);
			inds.push_back(Face.mIndices[2]);
		}

		return BasicMesh(verts, norms, inds, uvCoords);
    }

	return BasicMesh();
}

GLint loadShader(std::string filename, std::string type)
{
	//Load vertShader and pixShader as strings
	std::ifstream ShaderStream(filename);
	std::string ShaderString((std::istreambuf_iterator<char>(ShaderStream)),
		std::istreambuf_iterator<char>());
	const GLchar *vSource = (const GLchar *)ShaderString.c_str();

	GLint Shader;
	if (type == "vertex")
	{
		Shader= glCreateShader(GL_VERTEX_SHADER);
	}
	else if (type == "fragment")
	{
		Shader= glCreateShader(GL_FRAGMENT_SHADER);
	}
	else
	{
		std::cout << "INVALID SHADER TYPE" << std::endl;
	}
	glShaderSource(Shader, 1, &vSource, 0);
	glCompileShader(Shader);

	//Refactor this to improve efficiency
	int params = -1;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		int max_length = 2048;
		int actual_length = 0;
		char log[2048];
		glGetShaderInfoLog(Shader, max_length, &actual_length, log);
		std::cout << log;
	}

	return Shader;
}



int main(void)
{
	////////////////GLFW INIT/////////////////////////////////////////////////////
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(1280, 960, "Mesh Glitcher", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

	///////////GLEW INIT//////////////////////////////////////////////////////
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	//////////////////////////////////////////////////////////////////////////

	//Create mesh;
	BasicMesh myMesh = loadMesh("modoman.obj");

	//Vertices
	GLuint vertexBuffer = 0;
	glGenBuffers (1, &vertexBuffer);
	glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData (GL_ARRAY_BUFFER, myMesh.meshVerts.size() * sizeof (float), myMesh.meshVerts.data(), GL_STATIC_DRAW);

	//Normals
	GLuint normalBuffer = 0;
	glGenBuffers (1, &normalBuffer);
	glBindBuffer (GL_ARRAY_BUFFER, normalBuffer);
	glBufferData (GL_ARRAY_BUFFER, myMesh.meshNorms.size() * sizeof (float), myMesh.meshNorms.data(), GL_STATIC_DRAW);

	//UVs
	GLuint uvBuffer = 0;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData (GL_ARRAY_BUFFER, myMesh.meshUVs.size() * sizeof (float), myMesh.meshUVs.data(), GL_STATIC_DRAW);
	
	//VAO
	GLuint vao = 0;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);

	//Vertex_Buffer
	glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//Normal_Buffer
	glBindBuffer (GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//UV_buffer
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);

	//Element Buffer: Generate a buffer for the indices
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, myMesh.meshInds.size() * sizeof(unsigned int), myMesh.meshInds.data(), GL_STATIC_DRAW);

	//Setting up matrices
	glm::mat4 modelMatrix = glm::rotate(glm::mat4(), 20.0f, glm::vec3(0,1,0));
	modelMatrix *= glm::scale(modelMatrix, glm::vec3(1.,1.,1.));
	modelMatrix *= glm::translate(modelMatrix, glm::vec3(0., 2.65, 0.));
	eye = glm::vec3(0,1.5,-4);
	glm::mat4 viewMatrix = glm::lookAt(eye, glm::vec3(0,1.5,1), glm::vec3(0,1,0));
	glm::mat4 projMatrix = glm::perspective(45.0f, 640.0f / 480.0f, 0.0001f, 1000.f);


		//Proj Mat declared Global above
	glm::mat4 MVmat = viewMatrix * modelMatrix;

	glm::mat4 normMatrix = glm::transpose(glm::inverse(MVmat));



	//Shaders
	GLint vSh = loadShader("VertexShader.glsl", "vertex");
	GLint vSh2 = loadShader("VShader2.glsl", "vertex");
	GLint fSh = loadShader("FragmentShader.glsl", "fragment");
	program = glCreateProgram();
	program2 = glCreateProgram();
	glAttachShader(program, vSh);
	glAttachShader(program, fSh);
	glLinkProgram(program);
	glUseProgram(program);

	glAttachShader(program2, vSh2);
	glAttachShader(program2, fSh);
	glLinkProgram(program2);


	//Uniforms
	GLint location1 = glGetUniformLocation(program, "MVmatrix");
	glUniformMatrix4fv(location1, 1, GL_FALSE, glm::value_ptr( MVmat ));

	GLint location5 = glGetUniformLocation(program, "ProjMat");
	glUniformMatrix4fv(location5, 1, GL_FALSE, glm::value_ptr( projMatrix ));

	GLint location2 = glGetUniformLocation(program, "inputColor");
	glUniform4f(location2, 1.0, 1.0, 1.0, 1.0);

	GLint location3 = glGetUniformLocation(program, "lightDir");
	glUniform4f(location3, 1.0, 1.0, 1.0, 0.0);

	GLint location4 = glGetUniformLocation(program, "viewDir");
	glUniform4f(location4, eye.x, eye.y, eye.z, 0.0);

	GLint location6 = glGetUniformLocation(program, "normMatrix");
	glUniformMatrix4fv(location6, 1, GL_FALSE, glm::value_ptr( normMatrix ));


	double divisor = 0.0001;
	GLint location7 = glGetUniformLocation(program, "divisor");
	glUniform1f(location7,(GLfloat)divisor);

	//Some pre-rendering configuration
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	
	int direction = 1;

    while (!glfwWindowShouldClose(window))
    {
		//Rotate model
		modelMatrix = glm::rotate(modelMatrix, 0.001f, glm::vec3(0,1,0));
		MVmat = viewMatrix * modelMatrix;
		normMatrix = glm::transpose(glm::inverse(MVmat));

		glUniformMatrix4fv(location1, 1, GL_FALSE, glm::value_ptr( MVmat ));
		glUniformMatrix4fv(location5, 1, GL_FALSE,  glm::value_ptr( projMatrix ));
		glUniformMatrix4fv(location6, 1, GL_FALSE, glm::value_ptr( normMatrix ));

		divisor += 0.00001 * direction;

		if (divisor > 0.15)
		{
			direction = -1;
		}
		else if (divisor <0.0001)
		{
			direction = 1;
		}


		glUniform1f(location7,(GLfloat)divisor);
		

		//Clear previous frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw model
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glDrawElements(GL_TRIANGLES, myMesh.meshInds.size(), GL_UNSIGNED_INT, (void*)0);

		//Buffer swap
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
