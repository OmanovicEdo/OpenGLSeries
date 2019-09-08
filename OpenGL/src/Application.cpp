#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) 
{
	while (GLenum error = glGetError()) 
	{
		std::cout << "[OpenGL error] (" << error << "): " << function <<
			" " << file << ":" << line <<std::endl;
		return false;
	}
	return true;
}

struct ShaderProgramSource 
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	
	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else 
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	//error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);//iv: integer, vector - fn wants a pointer
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		//char message[length]; //c++ wants a constant.. or need new.. heap alloc, so we'll use a trick
		char* message = (char*)alloca(length * sizeof(char)); //allocates on stack dynamically
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
			<< "shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	//linked, stored inside program, we can delete them
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

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

	unsigned int buffer; //this is id of the buffer - opengl assigns these
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); //select this buffer, simple array
	GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(GL_FLOAT), positions, GL_STATIC_DRAW)); //data
	//we need to tell opengl how our data is structured

	GLCall(glEnableVertexAttribArray(0)); //gotta enable attrib
	//index 0.. vertex has 2 items.. last pointer is 0 since its our only attribute (position)
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0));
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)8);
	//for example if texture is 8 bytes offset - cast 8 into const void*

	unsigned int ibo; //index buffer object
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW)); //data
	

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");	
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(shader));

	//must call uniform after shader is bound
	GLCall(int location = glGetUniformLocation(shader, "u_Color"));
	ASSERT(location != -1);
	GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f);); //4floats

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
		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f);); //4floats
		//we are drawing 6 indices.. tho we have only 4 diff vertexes
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		 //nullptr because we have indices bound already
		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05f;

		r += increment;
		//bound buffer will be drawn
		//glDrawArrays(GL_TRIANGLES, 0, 6); //6 vertices to be rendered, starting at zero
		//when we issue draw call - vertex shader gets called (once for each vertex), primary purpose to tell where vertex goes on screen
		//then fragment shader called.. run once for each pixel rasterized (drawn) on screen - primary purp. to decide color of pixel(s)
		//fragment shader needs to be really well optimized
		//then we see result on the screen
				
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		
	}

	glDeleteProgram(shader);

	glfwTerminate();
}

