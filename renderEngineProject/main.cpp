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
	SDL_Window* window = SDL_CreateWindow(
		"Render Engine",
		800,
		600,
		SDL_WINDOW_OPENGL | SDL_WINDOW_ALWAYS_ON_TOP
	);
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
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 

	};
	

	uint VBO, VAO;

	

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
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

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init();


	while (true)
	{
		if (loop(window, shaderProgram, VAO) == 0)
			break;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

double previousTime = GetTime();
const int arraySize = 60;
double frameTimesBuffer[arraySize];

int currentIndex = 0;
int count = 0;

double lastFPS = 0.0;
double fpsCap = 0.0;

bool showMenu = false;

//running the script in while loop
int loop(SDL_Window* window, uint shaderProgram, uint VAO)
{
	SDL_Event event;

	double frameStartTime = GetTime();

	double currentTime = GetTime();
	double deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	frameTimesBuffer[currentIndex] = deltaTime;
	currentIndex = (currentIndex + 1) % arraySize;

	if (count < arraySize) {
		count++;
	};
	
	double sum = 0.0;

	for (int i = 0; i < count; i++) {
		sum += frameTimesBuffer[i];
	};

	double avgDeltaTime = sum / count;

	double currentFPS = (avgDeltaTime > 0.0) ? (1.0 / avgDeltaTime) : 0.0;
	double fpsSmoothed = (currentFPS * 0.8) + (lastFPS * 0.2);
	lastFPS = fpsSmoothed;


	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL3_ProcessEvent(&event);

		if (event.type == SDL_EVENT_QUIT)
			return 0;
		if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_INSERT) {
			showMenu = !showMenu;
		}
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	static ImGuiSliderFlags flags = ImGuiSliderFlags_None;
	const ImGuiSliderFlags flags_for_sliders = (flags & ~ImGuiSliderFlags_WrapAround);
	static int slider_i = 60;
	if (showMenu) {
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin("Render Engine");
		ImGui::Text("FPS: %.2f", fpsSmoothed);
		ImGui::SliderInt("FPS Cap", &slider_i, 0, 1000, " % d", flags_for_sliders);
		fpsCap = (slider_i > 0) ? (double)slider_i : 0.0;
		ImGui::End();
	}
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(window);


	//frame rate capping
	if (fpsCap > 0.0)
{
    double targetFrameTime = 1.0 / fpsCap;

    double frameEndTime = GetTime();
    double frameTime = frameEndTime - frameStartTime;

    if (frameTime < targetFrameTime)
    {
        double remaining = targetFrameTime - frameTime;

        // coarse wait (sleep part)
        if (remaining > 0.002)
        {
            std::this_thread::sleep_for(
                std::chrono::duration<double>(remaining - 0.001)
            );
        }

        // fine busy-wait (tight accuracy)
        while ((GetTime() - frameStartTime) < targetFrameTime)
        {
            // spin
        }
    }
}
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