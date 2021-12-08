// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Import the LoadShaders function
#include <common/shader.hpp>

int main() {
	// Initialize GLFW
	glewExperimental = true; // needed for core profile
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	// Create OpenGL window
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // MacOS
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // not old OpenGL

	// Open a window & create its OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "Tutorial 03", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version\n");
		glfwTerminate();
		return -1;
	}

	// Initialize GLEW
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// The VAO - Create a Vertex Array Object & set it as the current one
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Screen Coordinates - An array of 3 vectors which represents 3 verticles (x, y, z)
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	// Create and compile the GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// Drawing the triangle
	GLuint vertexbuffer; // identify the vertex buffer
	glGenBuffers(1, &vertexbuffer); // generate 1 buffer, put the resulting identifier in vertexbuffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); // talk about the 'vertexbuffer' buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW); // give the verticles to OpenGL

	// Projection matrix (45°Field of View, 4:3 ratio, display range 0.1~100 units)
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3),	// Camera is at (4, 3, 4) in World Space
		glm::vec3(0, 0, 0),	// and looks at the origin
		glm::vec3(0, 1, 0)	// Head is up (if (0, -1,0), looks upside-down)
	);

	// Model matrix - Identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

	// ModelViewProjection - multiplication of 3 matrices
	glm::mat4 mvp = Projection * View * Model;

	// Get a handle for "MVP" uniform (MVP 행렬 참조) only during the initialization
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// capture the escape key being presed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		// clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use the shader
		glUseProgram(programID);

		// Send transformation to the currently bound shader in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

		// 1st attribute buffer : verticles
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,			// attribute 0. No particular reason for 0, but must match the layout in the shader
			3,			// size
			GL_FLOAT,	// type
			GL_FALSE,	// normalized (T/F)
			0,			// stride (다음 요소까지의 간격)
			(void*)0	// array buffer offset (배열 버퍼의 옮기는 값)
		);

		// draw the triangle
		glDrawArrays(GL_TRIANGLES, 0, 3); // starting from the vertex 0, 3 verticles total (1 triangle)
		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// check if the ESC key was pressed OR the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
}