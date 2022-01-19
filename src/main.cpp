// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"

// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>

// Linear Algebra Library
#include <Eigen/Core>
#include <Eigen/Dense>

// File reading libraries
#include <string>
#include <fstream>
#include <sstream>

// Sin, Cos, and Pow functions
#include <cmath>

// VertexBufferObject wrapper
VertexBufferObject VBO;

// Contains the vertex positions
Eigen::MatrixXf V(6, 0);

// Contains the camera location
Eigen::Vector3f camPos(0, 0, 1);

//void importBox(std::vector<unsigned int> & Index);
void importBox(GLuint * E);
void importBumpyCube(GLuint * E);
void importBunny(GLuint * E);
Eigen::Matrix4f scaleMatrix(const float & scale);
Eigen::Matrix4f rotateMatrix(const float & angle, const char & axis);
Eigen::Matrix4f translateMatrix(const float & shift, const char & axis);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GLuint E[3000];
	static float camXY = 0;
	if (action != GLFW_RELEASE && mods == 0) {
		switch (key)
		{
		case  GLFW_KEY_RIGHT:
			camXY -= 0.1;
			if (camXY < -2*3.141592) camXY = 0;
			camPos << sin(camXY), camPos[1], cos(camXY);
			break;
		case GLFW_KEY_LEFT:
			camXY += 0.1;
			if (camXY > 2*3.141592) camXY = 0;
			camPos << sin(camXY), camPos[1], cos(camXY);
			break;
		case  GLFW_KEY_UP:
			camPos[1] -= 0.1;
			if (camPos[1] < -1) camPos[1] = -1;
			break;
		case GLFW_KEY_DOWN:
			camPos[1] += 0.1;
			if (camPos[1] > 1) camPos[1] = 1;
			break;
		case  GLFW_KEY_R:
			rotateMatrix(0, 'r');
			scaleMatrix(-1);
			translateMatrix(0, 'r');
			camPos << 0, 0, 1;
			camXY = 0;
			break;
		case  GLFW_KEY_Q:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case  GLFW_KEY_W:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case  GLFW_KEY_3:
			rotateMatrix(0, 'r');
			scaleMatrix(-1);
			translateMatrix(0, 'r');
			camPos << 0, 0, 1;
			camXY = 0;
			importBunny(E);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*3000, E, GL_STATIC_DRAW);
			break;
		case  GLFW_KEY_2:
			rotateMatrix(0, 'r');
			scaleMatrix(-1);
			translateMatrix(0, 'r');
			camPos << 0, 0, 1;
			camXY = 0;
			importBumpyCube(E);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*3000, E, GL_STATIC_DRAW);
			break;
		case  GLFW_KEY_1:
			rotateMatrix(0, 'r');
			scaleMatrix(-1);
			translateMatrix(0, 'r');
			camPos << 0, 0, 1;
			camXY = 0;
			importBox(E);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 36, E, GL_STATIC_DRAW);
			break;
		case GLFW_KEY_KP_4:
			rotateMatrix(10, 'y');
			break;
		case GLFW_KEY_KP_6:
			rotateMatrix(-10, 'y');
			break;
		case GLFW_KEY_KP_8:
			rotateMatrix(-10, 'x');
			break;
		case GLFW_KEY_KP_2:
			rotateMatrix(10, 'x');
			break;
		case GLFW_KEY_KP_7:
			rotateMatrix(-10, 'z');
			break;
		case GLFW_KEY_KP_9:
			rotateMatrix(10, 'z');
			break;
		case GLFW_KEY_KP_5:
			scaleMatrix(-0.1);
			break;
		case GLFW_KEY_KP_1:
			scaleMatrix(0.1);
			break;
		default:
			break;
		}
	}
	if (action != GLFW_RELEASE && mods == GLFW_MOD_ALT) {
		switch (key)
		{
		case GLFW_KEY_KP_4:
			translateMatrix(-0.1, 'x');
			break;
		case GLFW_KEY_KP_6:
			translateMatrix(0.1, 'x');
			break;
		case GLFW_KEY_KP_8:
			translateMatrix(0.1, 'y');
			break;
		case GLFW_KEY_KP_2:
			translateMatrix(-0.1, 'y');
			break;
		case GLFW_KEY_KP_5:
			translateMatrix(-0.1, 'z');
			break;
		case GLFW_KEY_KP_1:
			translateMatrix(0.1, 'z');
			break;
		default:
			break;
		}
	}
	// Upload the change to the GPU
	VBO.update(V);
}

void window_resize_callback(GLFWwindow * window, int w, int h) {
	glViewport(0, 0, w, h);
}

int main(void)
{
	GLFWwindow* window;
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	#ifndef __APPLE__
	glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	#endif

	int major, minor, rev;
	major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
	printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
	printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
	printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	VertexArrayObject VAO;
	VAO.init();
	VAO.bind();

	VBO.init();
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	GLuint E[3000];
	importBox(E);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(E), E, GL_STATIC_DRAW);

	Program program;
	const GLchar* vertex_shader =
		"#version 150 core\n"
		"in vec3 position;"
		"in vec3 color;"
		"uniform mat4 model;"
		"uniform mat4 projection;"
		"out vec3 Color;"
		"void main()"
		"{"
		"	 Color = color;"
		"    gl_Position = projection * model * vec4(position, 1.0);"
		"}";
	const GLchar* fragment_shader =
		"#version 150 core\n"
		"in vec3 Color;"
		"out vec4 outColor;"
		"void main()"
		"{"
		"    outColor = vec4(Color+0.2, 1.0);" //Ambient lighting +0.2
		"}";

	program.init(vertex_shader, fragment_shader, "outColor");
	program.bind();

	GLint posAttrib = glGetAttribLocation(program.program_shader, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	GLint colAttrib = glGetAttribLocation(program.program_shader, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, window_resize_callback);

	while (!glfwWindowShouldClose(window))
	{
		program.bind();

		//Translate
		Eigen::Matrix4f translate = translateMatrix(0, 'x');

		//Rotate
		Eigen::Matrix4f rotate = rotateMatrix(0, 'x');

		//Scale
		Eigen::Matrix4f scale = scaleMatrix(0);

		//Combine transformations
		Eigen::Matrix4f model = translate * rotate * scale;

		glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, model.data());

		//Viewport
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		float aspect_ratio = float(height) / float(width);
		Eigen::Matrix4f view;
		view <<
			aspect_ratio, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;

		//Orth
		float l = -1, b = -1, n = 0.1;
		float r = 1, t = 1, f = -1000;
		Eigen::Matrix4f orth;
		orth <<
			2.0/(r-l), 0, 0, -1*(r+l)/(r-l),
			0, 2.0/(t-b), 0, -1*(t+b)/(t-b),
			0, 0, 2.0/(n-f), -1*(n+f)/(n-f),
			0, 0, 0, 1;

		//Camera
		Eigen::Vector3f target(0, 0, 0);
		Eigen::Vector3f g = target - camPos;
		Eigen::Vector3f w = -g / g.norm();
		Eigen::Vector3f up(0, 1, 0);
		Eigen::Vector3f u = up.cross(w) / (up.cross(w)).norm();
		Eigen::Vector3f v = w.cross(u);
		Eigen::Matrix4f cam;
		cam <<
		u[0], v[0], w[0], camPos[0],
		u[1], v[1], w[1], camPos[1],
		u[2], v[2], w[2], camPos[2],
		0, 0, 0, 1;

		Eigen::Matrix4f projection;
		projection = view * orth * cam.inverse();
		
		glUniformMatrix4fv(program.uniform("projection"), 1, GL_FALSE, projection.data());

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, sizeof(E) / 4, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	program.free();
	VAO.free();
	VBO.free();
	glfwTerminate();
	return 0;
}

void importBunny(GLuint * E) {
	//Clear V matrix
	V.resize(6,0);

	//Declarations
	Eigen::Vector3f vertices;
	Eigen::Vector4f faces;
	std::string line;
	std::ifstream inputfile;
	int num_of_vertices;
	int num_of_faces;

	//Open file
	inputfile.open("../data/bunny.off");

	//First line is useless
	getline(inputfile, line);

	//Second line has number of vertices and faces
	getline(inputfile, line);
	std::stringstream sstream(line);
	sstream >> num_of_vertices;
	sstream >> num_of_faces;

	//Add columns for new vertices
	V.conservativeResize(V.rows(), V.cols() + num_of_vertices);

	//Read in vertices line by line and put in V matrix
	for (int v = 0; v < num_of_vertices; ++v) {
		getline(inputfile, line);
		std::stringstream sstream(line);
		sstream >> vertices[0];
		sstream >> vertices[1];
		sstream >> vertices[2];
		vertices = vertices * 8; //Scale up 8x
		vertices[1]--;			 //Translate down 1
		V.col(v) << 
			vertices[0],			//X
			vertices[1],			//Y
			vertices[2],			//Z
			pow(vertices[0], 2),	//R
			pow(vertices[1], 2),	//G
			pow(vertices[2], 2);	//B
	}

	VBO.update(V);

	for (int camPos = 0; camPos < num_of_faces; ++camPos) {
		getline(inputfile, line);
		std::stringstream sstream(line);
		sstream >> faces[0]; //Useless first number
		sstream >> faces[1]; //Triangle point 1
		sstream >> faces[2]; //Triangle point 2
		sstream >> faces[3]; //Triangle point 3

		E[camPos * 3] = faces[1];		//Fill E with indices that will be put 
		E[(camPos * 3) + 1] = faces[2];	//into the element buffer
		E[(camPos * 3) + 2] = faces[3];
	}
	inputfile.close();
}

void importBox(GLuint * E) {

	//Manual input of Box vertex positions and colors
	V.resize(6, 8);
	V.col(0) << -0.5, 0.5, -0.5, 0.5, 0.5, 0.5;
	V.col(1) << 0.5, 0.5, -0.5, 0.5, 0.5, 0.5;
	V.col(2) << -0.5, -0.5, -0.5, 0.4, 0.4, 0.4;
	V.col(3) << 0.5, -0.5, -0.5, 0.4, 0.4, 0.4;
	V.col(4) << -0.5, 0.5, 0.5, 0.3, 0.3, 0.3;
	V.col(5) << 0.5, 0.5, 0.5, 0.3, 0.3, 0.3;
	V.col(6) << -0.5, -0.5, 0.5, 0.2, 0.2, 0.2;
	V.col(7) << 0.5, -0.5, 0.5, 0.2, 0.2, 0.2;

	//Manual input of Box indices
	Eigen::VectorXf elements(36);
	elements <<
		0, 1, 3,
		3, 2, 0,
		1, 5, 7,
		7, 3, 1,
		0, 4, 6,
		6, 2, 0,
		4, 5, 7,
		7, 6, 4,
		0, 4, 5,
		5, 1, 0,
		2, 3, 7,
		7, 6, 2;

	//Fill E with indices for the element buffer
	for (int i = 0; i < 36; ++i) {
		E[i] = elements[i];
	}
	VBO.update(V);
}

void importBumpyCube(GLuint * E) {
	//Clear V matrix
	V.resize(6, 0);

	//Declarations
	Eigen::Vector3f vertices;
	Eigen::Vector4f faces;
	std::string line;
	std::ifstream inputfile;
	int num_of_vertices;
	int num_of_faces;

	//Open file
	inputfile.open("../data/bumpy_cube.off");

	//First line is useless
	getline(inputfile, line);

	//Second line has number of vertices and faces
	getline(inputfile, line);
	std::stringstream sstream(line);
	sstream >> num_of_vertices;
	sstream >> num_of_faces;

	//Add columns for new vertices
	V.conservativeResize(V.rows(), V.cols() + num_of_vertices);

	//Read in vertices line by line and put in V matrix
	for (int v = 0; v < num_of_vertices; ++v) {
		getline(inputfile, line);
		std::stringstream sstream(line);
		sstream >> vertices[0];
		sstream >> vertices[1];
		sstream >> vertices[2];
		vertices = vertices * 0.2; //Scale down to 20%
		V.col(v) << 
			vertices[0],			//X
			vertices[1],			//Y
			vertices[2],			//Z
			pow(vertices[0], 2),	//R
			pow(vertices[1], 2),	//G
			pow(vertices[2], 2);	//B
	}

	VBO.update(V);

	for (int camPos = 0; camPos < num_of_faces; ++camPos) {
		getline(inputfile, line);
		std::stringstream sstream(line);
		sstream >> faces[0]; //Useless first number
		sstream >> faces[1]; //Triangle point 1
		sstream >> faces[2]; //Triangle point 2
		sstream >> faces[3]; //Triangle point 3

		E[camPos * 3] = faces[1];		//Fill E with indices that will be put 
		E[(camPos * 3) + 1] = faces[2];	//into the element buffer
		E[(camPos * 3) + 2] = faces[3];
	}
	inputfile.close();
}

Eigen::Matrix4f rotateMatrix(const float & angle, const char & axis) {
	//These track the level of rotation on each axis. Resets to 0 if over 2pi
	static float rotationX;
	static float rotationY;
	static float rotationZ;
	if (axis == 'x') rotationX += (angle * 3.141592) / 180;
	if (rotationX > 2 * 3.141592) rotationX = 0;
	if (axis == 'y') rotationY += (angle * 3.141592) / 180;
	if (rotationY > 2 * 3.141592) rotationY = 0;
	if (axis == 'z') rotationZ += (angle * 3.141592) / 180;
	if (rotationZ > 2 * 3.141592) rotationZ = 0;

	//If r is passed as the axis, reset rotation
	if (axis == 'r') {
		rotationX = 0;
		rotationY = 0;
		rotationZ = 0;
	}

	//Matrix rotation calculations
	Eigen::Matrix4f X;
	X <<
		1, 0, 0, 0,
		0, cos(rotationX), sin(rotationX), 0,
		0, -sin(rotationX), cos(rotationX), 0,
		0, 0, 0, 1;
	Eigen::Matrix4f Y;
	Y <<
		cos(rotationY), 0, sin(rotationY), 0,
		0, 1, 0, 0,
		-sin(rotationY), 0, cos(rotationY), 0,
		0, 0, 0, 1;
	Eigen::Matrix4f Z;
	Z <<
		cos(rotationZ), sin(rotationZ), 0, 0,
		-sin(rotationZ), cos(rotationZ), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	//Multiply them together for total rotation
	return X * Y * Z;
}

Eigen::Matrix4f scaleMatrix(const float & scale) {
	//Tracks scale of object
	static float Scale = 1;

	//Increase scale by value passed in
	Scale += scale;

	//Reset to 1 if -1 is passed in
	if (scale == -1) Scale = 1;

	Eigen::Matrix4f S;
	S <<
		Scale, 0, 0, 0,
		0, Scale, 0, 0,
		0, 0, Scale, 0,
		0, 0, 0, 1;

	return S;
}

Eigen::Matrix4f translateMatrix(const float & shift, const char & axis) {
	//These track the translation on each axis
	static float translateX = 0;
	static float translateY = 0;
	static float translateZ = 0;
	if (axis == 'x') translateX += shift;
	if (axis == 'y') translateY += shift;
	if (axis == 'z') translateZ += shift;

	//If r is passed as the axis, reset translation
	if (axis == 'r') {
		translateX = 0;
		translateY = 0;
		translateZ = 0;
	}

	Eigen::Matrix4f T;
	T <<
		1, 0, 0, translateX,
		0, 1, 0, translateY,
		0, 0, 1, translateZ,
		0, 0, 0, 1;

	return T;
}