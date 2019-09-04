#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit()) {
		std::cout << "Error GLFW not ok" << std::endl;
		return -1;
	}
		
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);


	std::cout << glGetString(GL_VERSION) << std::endl;

	if (glewInit() != GLEW_OK) {
		std::cout << "Error GLEW not ok" << std::endl;
		return -1;
	}
		
	float positions[6] = { //its a buffer
		-0.5f, -0.5f, //vertex1 - point on a geometry.. not a position, but it does usually contain position
		 0.0f,  0.5f, //2
		 0.5f, -0.5f, //3
	};

	unsigned int buffer; //this is id of the buffer - opengl assigns these
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer); //select this buffer, simple array
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GL_FLOAT), positions, GL_STATIC_DRAW); //data
	//we need to tell opengl how our data is structured

	glEnableVertexAttribArray(0); //gotta enable attrib
	//index 0.. vertex has 2 items.. last pointer is 0 since its our only attribute (position)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)8);
	//for example if texture is 8 bytes offset - cast 8 into const void*

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		
		//vertex buffer - array of bytes in memory, vram in gpu..
		//we need to tell gpu how to read that buffer and how to draw it
		//shader - program that runs on gpu
		//opengl operates as state machine

		//bound buffer will be drawn
		glDrawArrays(GL_TRIANGLES, 0, 3); //3 vertices to be rendered, starting at zero

		//Mogu napamet kucat svako slovo

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
}