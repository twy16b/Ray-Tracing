// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"

// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>

// Linear Algebra Library
#include <Eigen/Core>
#include <Eigen/Dense>

// Timer
#include <chrono>
#include <cmath>

// VertexBufferObject wrapper
VertexBufferObject VBO;

// Contains the vertex positions
Eigen::MatrixXf V(2, 3);

// Contains the view transformation
Eigen::Matrix4f translate(4, 4);
Eigen::Matrix4f scale(4, 4);
Eigen::Matrix4f rotate(4, 4);
Eigen::Matrix4f transform(4, 4);
Eigen::Matrix4f view(4, 4);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// Get the position of the mouse in the window
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Get the size of the window
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Convert screen position to world coordinates
	Eigen::Vector4f p_screen(xpos, height - 1 - ypos, 0, 1);
	Eigen::Vector4f p_canonical((p_screen[0] / width) * 2 - 1, (p_screen[1] / height) * 2 - 1, 0, 1);
	Eigen::Vector4f p_world = view.inverse()*p_canonical;

	// Update the position of the first vertex if the left button is pressed
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		V.col(0) << p_world[0], p_world[1];

	// Upload the change to the GPU
	VBO.update(V);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Update the position of the first vertex if the keys 1,2, or 3 are pressed
	switch (key)
	{
	case  GLFW_KEY_1:
		V.col(0) << -0.5, 0.5;
		break;
	case GLFW_KEY_2:
		V.col(0) << 0, 0.5;
		break;
	case  GLFW_KEY_3:
		V.col(0) << 0.5, 0.5;
		break;
	default:
		break;
	}

	// Upload the change to the GPU
	VBO.update(V);
}

void window_size_callback(GLFWwindow* window, int w, int h)
{
	//If the window size changes, change the viewport to match
	glViewport(0, 0, w, h);
}

int main(void)
{
	GLFWwindow* window;

	// Initialize the library
	if (!glfwInit())
		return -1;

	// Activate supersampling
	glfwWindowHint(GLFW_SAMPLES, 8);

	// Ensure that we get at least a 3.2 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
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

	// Initialize the VAO
	// A Vertex Array Object (or VAO) is an object that describes how the vertex
	// attributes are stored in a Vertex Buffer Object (or VBO). This means that
	// the VAO is not the actual object storing the vertex data,
	// but the descriptor of the vertex data.
	VertexArrayObject VAO;
	VAO.init();
	VAO.bind();

	// Initialize the VBO with the vertices data
	// A VBO is a data container that lives in the GPU memory
	VBO.init();

	V.resize(6, 12);
	V << -1, 1, -1, -1, 1, 1,	 1,  1,  1,  1,  1,  1,
		1, -1, -1, 1, 1,-1,		 1, -1, -1,  1, -1,  1,
		0, 0, 0, 0, 0, 0,		 0,  0, -1,  0, -1, -1,
		1, 1, 1, 1, 1, 1,		 0,  0,  0,  0,  0,  0,
		0, 0, 0, 0, 0, 0,		 1,  1,  1,  1,  1,  1,
		0, 0, 0, 0, 0, 0,		 0,  0,  0,  0,  0,  0;
	VBO.update(V);

	// Initialize the OpenGL Program
	// A program controls the OpenGL pipeline and it must contains
	// at least a vertex shader and a fragment shader to be valid
	Program program;
	const GLchar* vertex_shader =
		"#version 150 core\n"
		"in vec3 position;"
		"in vec3 color;"
		"out vec3 Color;"
		"uniform mat4 view;"
		"uniform mat4 transform;"
		"void main()"
		"{"
		"	 Color = color;"
		"    gl_Position = transform * view * vec4(position, 1.0);"
		"}";
	const GLchar* fragment_shader =
		"#version 150 core\n"
		"in vec3 Color;"
		"out vec4 outColor;"
		"void main()"
		"{"
		"    outColor = vec4(Color, 1.0);"
		"}";

	// Compile the two shaders and upload the binary to the GPU
	// Note that we have to explicitly specify that the output "slot" called outColor
	// is the one that we want in the fragment buffer (and thus on screen)
	program.init(vertex_shader, fragment_shader, "outColor");
	program.bind();

	// The vertex shader wants the position of the vertices as an input.
	// The following line connects the VBO we defined above with the position "slot"
	// in the vertex shader
	GLint posAttrib = glGetAttribLocation(program.program_shader, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(float), 0);

	GLint colAttrib = glGetAttribLocation(program.program_shader, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(float), (void*)(3 * sizeof(float)));

	auto t_start = std::chrono::high_resolution_clock::now();

	// Register the keyboard callback
	glfwSetKeyCallback(window, key_callback);

	// Register the mouse callback
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Register the window resize callback
	glfwSetWindowSizeCallback(window, window_size_callback);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Bind your VAO (not necessary if you have only one)
		VAO.bind();

		// Bind your program
		program.bind();


		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
		glUniform3f(program.uniform("triangleColor"), (float)(sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);


		// Get size of the window
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		GLdouble aspect_ratio = float(height) / float(width); // corresponds to the necessary width scaling

		view <<
			aspect_ratio, 0, 0, 0, // rightX, upX, forwardX, positionX
			0, 1, 0, 0, // rightY, upY, forwardY, positionY
			0, 0, 1, 0, // rightZ, upZ, forwardZ, positionZ
			0, 0, 0, 1; //	   0,	0,		  0,		 1

		translate <<
			1, 0, 0, 0, // 1, 0, 0, x
			0, 1, 0, 0, // 0, 1, 0, y
			0, 0, 1, 0, // 0, 0, 1, z
			0, 0, 0, 1; // 0, 0, 0, 1

		scale <<
			0.5, 0, 0, 0, // x, 0, 0, 0
			0, 0.5, 0, 0, // 0, y, 0, 0
			0, 0, 0.5, 0, // 0, 0, z, 0
			0, 0, 0, 1;   // 0, 0, 0, 1

		rotate <<									// Z AXIS						// X AXIS						// Y AXIS
			cos(time), 0, sin(time), 0,								//  cos(a),	sin(a), 0, 0		// 1,		0,		0, 0		//  cos(a),		 0,	sin(a), 0
			0, 1, 0, 0,								// -sin(a), cos(a), 0, 0		// 0,  cos(a), sin(a), 0		//		0,		 1,		 0, 0
			-sin(time), 0, cos(time), 0,							//		 0,		0,  1, 0		// 0, -sin(a), cos(a), 0		// -sin(a),		 0, cos(a), 0
			0, 0, 0, 1;								//		 0,		0,  0, 1		// 0,		0,		0, 1		//		 0,		 0,		 0, 1

		transform = rotate * scale * translate;

		glUniformMatrix4fv(program.uniform("transform"), 1, GL_FALSE, transform.data());
		glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, view.data());

		// Clear the framebuffer
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw a triangle
		glDrawArrays(GL_LINE_LOOP , 0, V.cols());

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	// Deallocate opengl memory
	program.free();
	VAO.free();
	VBO.free();

	// Deallocate glfw internals
	glfwTerminate();
	return 0;
}
