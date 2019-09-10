#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#include <iostream>

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit()) {
		std::cout << "Error GLFW not ok" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//GLFW_OPENGL_COMPAT_PROFILE is by default
		
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); //should use vsync

	std::cout << glGetString(GL_VERSION) << std::endl;

	if (glewInit() != GLEW_OK) {
		std::cout << "Error GLEW not ok" << std::endl;
		return -1;
	}
		
	float positions[] = { //its a buffer
		-0.5f, -0.5f, //vertex1 - point on a geometry.. not a position, but it does usually contain position
		 0.5f, -0.5f, //2
		 0.5f,  0.5f, //3		 		 
		-0.5f,  0.5f	
	};
	unsigned int indices[] = { //this is an index buffer
		0, 1, 2,
		2, 3, 0
	};

	//unsigned int vao; //vertex array object
	//GLCall(glGenVertexArrays(1, &vao));
	//GLCall(glBindVertexArray(vao));

	VertexArray va;
	VertexBuffer vb(positions, 4 * 2 * sizeof(GL_FLOAT));
	VertexBufferLayout layout;
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);
	
	IndexBuffer ib(indices, 6);
	
	Shader shader("res/shaders/Basic.shader");
	shader.Bind();
	shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
		
	va.Unbind();
	vb.Unbind();
	ib.Unbind();
	shader.Unbind();

	float r = 0.0f;
	float increment = 0.05f;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		
		//vertex buffer - array of bytes in memory, vram in gpu..
		//we need to tell gpu how to read that buffer and how to draw it
		//shader - program that runs on gpu
		//opengl operates as state machine

		//we set UNIFORMS before we do the drawings - they are set per draw
		shader.Bind();
		shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
						
		va.Bind();
		ib.Bind();
		
		//we are drawing 6 indices.. tho we have only 4 diff vertexes
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		//nullptr because we have indices bound already
		
		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05f;

		r += increment;
		
		//when we issue draw call - vertex shader gets called (once for each vertex), primary purpose to tell where vertex goes on screen
		//then fragment shader called.. run once for each pixel rasterized (drawn) on screen - primary purp. to decide color of pixel(s)
		//fragment shader needs to be really well optimized
		//then we see result on the screen
				
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
}


