#define GLAD_GL_IMPLEMENTATION
#include "main.hpp"
	double GetTime() 
	{
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = now - startTime;
		return elapsed.count();
	};

int main()
{
	if(!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return -1;
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_Window* window = SDL_CreateWindow("RenderEngine", 800, 600, SDL_WINDOW_OPENGL);
	if (!window) {
		std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
		return -1;
	}
	SDL_SetWindowResizable(window, true);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context) {
		std::cerr << "Failed to initialize SDL context" << SDL_GetError() << std::endl;
		return -1;
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, 800, 600);

	// vertex shader file read
	std::string vertexShaderSource;
	ReadFileContents("vertex.glsl", 0, vertexShaderSource);
	const char* vertex = vertexShaderSource.c_str();
	if (!ReadFileContents("vertex.glsl", 0, vertexShaderSource)) {
		std::cerr << "Couldn't load vertex.glsl" << std::endl;
		return -1;
	}
	// fragment shader file read
	std::string fragmentShaderSource;
	ReadFileContents("fragment.glsl", 0, fragmentShaderSource);
	const char* fragment = fragmentShaderSource.c_str();
	if (!ReadFileContents("fragment.glsl", 0, fragmentShaderSource)) {
		std::cerr << "Couldn't load fragment.glsl" << std::endl;
		return -1;
	}


	float vertices[] = {
	 0.5f,  0.5f, 0.0f, // 0
	 0.5f, -0.5f, 0.0f, // 1
	-0.5f, -0.5f, 0.0f, // 2
	-0.5f,  0.5f, 0.0f  // 3
	};
	unsigned int indices[] = {
	0, 1, 3,
	1, 2, 3
	};

	uint EBO, VBO, VAO;
	glGenBuffers(1, &EBO);

	

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// vertex shader
	uint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertex, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n\t" << infoLog << std::endl;
	}

	// fragment shader
	uint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, &fragment, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n\t" << infoLog << std::endl;
	};

	// Shader program
	uint shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "PROGRAM LINK ERROR:\n" << infoLog << std::endl;
	}


	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	

	while(true)
	{
		if(loop(window, shaderProgram, VAO) == 0)
			break;
	}
	SDL_GL_DestroyContext(context);

	SDL_DestroyWindow(window);

	SDL_Quit();
}




//running the script in while loop
int loop(SDL_Window* window, uint shaderProgram, uint VAO) {
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT)
			return 0;
	}
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgram);

	double TimeValue = GetTime();
	float greenValue = (sin(TimeValue) / 2.0f) + 0.5f;

	int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
	glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

	glBindVertexArray(VAO); 
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	SDL_GL_SwapWindow(window);
	return 1;
}









int ReadFileContents(const char* filename, int binaryMode, std::string& contentOut)
{
	contentOut = "";
	std::ios_base::openmode mode = (std::ios_base::openmode)0;
	if (binaryMode)
		mode ^= std::ios::binary;
	std::ifstream file(filename, mode);
	if (!file) return 0;

	file.seekg(0, std::ios::end);
	std::streampos length = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(length);
	file.read(&buffer[0], length);

	contentOut.assign(&buffer[0], length);
	return 1;
}