#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);

// Ajustes
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

// Camara
Camera camera(glm::vec3(0.0f, 10.0f, 50.0f));
float MovementSpeed = 0.1f;
GLfloat lastX = SCR_WIDTH / 2.0f;
GLfloat lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
glm::vec3 posicionCofre(-15.3f, 3.0f, 4.0f);
glm::vec3 posicionEsfera(2.0f, 3.0f, 5.0f);
glm::vec3 posicionGeneral(0.0f, 5.0f, 55.0f);

// Tiempo
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS;
double deltaTime = 0.0f;
double lastFrame = 0.0f;

// Animación del cofre
bool cofreAbierto = false;
float anguloTapaCofre = 0.0f;
float velocidadAperturaCofre = 80.0f;
float anguloMaximoCofre = 50.0f;

//=======POSEIDON ANIMACION========
int estadoAnimacion = 0;
float tiempoAnimacion = 0.0f;
float anguloHombroIzq = 0.0f;
float anguloCodoIzq = 0.0f;
float anguloHombroDer = 0.0f;
float anguloCodoDer = 0.0f;
float rotacionHombroIzqY = 0.0f;
float rotacionHombroIzqZ = 0.0f;
float rotacionHombroDerY = 0.0f;
float rotacionHombroDerZ = 0.0f;
float rotacionCodoDerY = 0.0f;
float rotacionCodoDerZ = 0.0f;
float rotacionCodoIzqY = 0.0f;
float rotacionCodoIzqZ = 0.0f;
float balanceoTorso = 0.0f;
float rotacionCuerpo = 0.0f;
//=======POSEIDON ANIMACION FIN========= 

//=======AVE ANIMACION========
float tiempoVueloAve = 0.0f;
float velocidadVueloAve = 0.9f;
float radioVuelo = 14.0f;
float alturaVuelo = 10.0f;
glm::vec3 centroVuelo(0.0f, 0.0f, 0.0f);

// Variables para aleteo
float anguloAleteo = 0.0f;
float velocidadAleteo = 40.0f;
float amplitudAleteo = 4.0f;
//=======AVE ANIMACION FIN========= 
//=======ESFERA Y AROS ANIMACION========
float tiempoEsfera = 0.0f;
float rotacionEsfera = 0.0f;
float pulsacionEsfera = 1.0f;
// Rotaciones independientes para cada aro
float rotacionAro1 = 0.0f;
float rotacionAro2 = 0.0f;
float rotacionAro3 = 0.0f;
float rotacionAro4 = 0.0f;
float rotacionAro5 = 0.0f;
// Velocidades de rotación 
float velocidadEsfera = 30.0f;
float velocidadAro1 = 45.0f;
float velocidadAro2 = 60.0f;
float velocidadAro3 = 75.0f;
float velocidadAro4 = 90.0f;
float velocidadAro5 = 105.0f;
// Pulsación
float velocidadPulsacion = 2.0f;
float amplitudPulsacion = 0.15f;
//=======ESFERA Y AROS ANIMACION FIN========

// Animación de cabeza de Toro
bool upHead = false;
float anguloHead = 0.0f;
float velocidad = 10.0f;
float anguloMax = 4.0f;

glm::vec3 posicionBull(-1.16222f, 1.18517f,  -22.3832f);

// ========== AUDIO ==========
//1
SDL_AudioStream* primerStream = nullptr;
Uint8* primerBuffer = nullptr;
Uint32 primerLength = 0;
bool audioInicializado = false;
SDL_AudioDeviceID audioDeviceID = 0;
// 2
SDL_AudioStream* segundoStream = nullptr;
Uint8* segundoBuffer = nullptr;
Uint32 segundoLength = 0;


// Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

// Floor
GLuint VAO_floor, VBO_floor, EBO_floor;
unsigned int floorTexture;
unsigned int texturaRuido;
void reproducirPrimerAudio() {
	if (primerStream && primerBuffer && audioDeviceID > 0) {
		SDL_ClearAudioStream(primerStream);

		SDL_BindAudioStream(audioDeviceID, primerStream);

		SDL_PutAudioStreamData(primerStream, primerBuffer, primerLength);
		SDL_ResumeAudioDevice(audioDeviceID);

		std::cout << " Reproduciendo primer audio (saludo)" << std::endl;
	}
}

void reproducirSegundoAudio() {
	if (segundoStream && segundoBuffer && audioDeviceID > 0) {
		SDL_ClearAudioStream(segundoStream);

		SDL_BindAudioStream(audioDeviceID, segundoStream);

		SDL_PutAudioStreamData(segundoStream, segundoBuffer, segundoLength);

		SDL_ResumeAudioDevice(audioDeviceID);

		std::cout << "Reproduciendo segundo audio (pajaros)" << std::endl;
	}
}

void setupFloor() {
	float verticesPiso[] = {
		// positions          // texture coords
		 50.0f, 0.0f,  50.0f,   10.0f, 0.0f,
		 50.0f, 0.0f, -50.0f,   10.0f, 10.0f,
		-50.0f, 0.0f, -50.0f,   0.0f, 10.0f,
		-50.0f, 0.0f,  50.0f,   0.0f, 0.0f
	};
	unsigned int indicesPiso[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays(1, &VAO_floor);
	glGenBuffers(1, &VBO_floor);
	glGenBuffers(1, &EBO_floor);

	glBindVertexArray(VAO_floor);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_floor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_floor);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

unsigned int generateTextures(const char* filename, bool alfa) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);

	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data) {
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture: " << filename << std::endl;
	}
	stbi_image_free(data);
	return textureID;
}

int main() {
	// GLFW: initialize and configure
	glfwInit();
	// ========================================
	// AUDIO
	// ========================================
	SDL_SetHint(SDL_HINT_AUDIO_DEVICE_APP_NAME, "Proyecto OpenGL");

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
		std::cerr << "Error al inicializar SDL Audio: " << SDL_GetError() << std::endl;
	}
	else {
		audioInicializado = true;
		std::cout << "=== DEBUG AUDIO ===" << std::endl;

		// Cargar 1 audio
		SDL_AudioSpec wavSpec;
		int resultado = SDL_LoadWAV("audio/saludo.wav", &wavSpec, &primerBuffer, &primerLength);

		if (resultado == 0) {
			std::cout << "Primer audio cargado correctamente" << std::endl;
			std::cout << "  Frecuencia: " << wavSpec.freq << " Hz" << std::endl;
			std::cout << "  Formato: " << SDL_AUDIO_BITSIZE(wavSpec.format) << " bits" << std::endl;
			std::cout << "  Canales: " << (int)wavSpec.channels << std::endl;
			std::cout << "  Tamaño: " << primerLength << " bytes" << std::endl;

			// Abrir dispositivo por defecto
			std::cout << "Intentando abrir dispositivo por defecto..." << std::endl;
			audioDeviceID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_OUTPUT, &wavSpec);

			if (audioDeviceID > 0) {
				std::cout << "Dispositivo abierto (ID: " << audioDeviceID << ")" << std::endl;

				// Obtener formato del dispositivo
				SDL_AudioSpec deviceSpec;
				int sampleFrames = 0;

				if (SDL_GetAudioDeviceFormat(audioDeviceID, &deviceSpec, &sampleFrames) == 0) {
					std::cout << "Formato del dispositivo:" << std::endl;
					std::cout << "  Frecuencia: " << deviceSpec.freq << " Hz" << std::endl;
					std::cout << "  Canales: " << deviceSpec.channels << std::endl;
				}

				SDL_AudioSpec outputSpec;
				outputSpec.freq = deviceSpec.freq;
				outputSpec.format = SDL_AUDIO_F32;
				outputSpec.channels = deviceSpec.channels;

				primerStream = SDL_CreateAudioStream(&wavSpec, &outputSpec);

				if (primerStream) {
					std::cout << "✓ Stream del primer audio creado" << std::endl;
				}
			}
			else {
				std::cerr << "Error abriendo dispositivo: " << SDL_GetError() << std::endl;
			}
		}
		else {
			std::cerr << "Error cargando primer WAV: " << SDL_GetError() << std::endl;
		}

	}
	//2
	if (audioInicializado && audioDeviceID > 0) {
		SDL_AudioSpec segundoWavSpec;

		int resultado2 = SDL_LoadWAV("audio/pajaro.wav", &segundoWavSpec, &segundoBuffer, &segundoLength);

		if (resultado2 == 0) {
			std::cout << "✓ Segundo audio cargado" << std::endl;
			std::cout << "  Tamaño: " << segundoLength << " bytes" << std::endl;

			SDL_AudioSpec outputSpec;
			SDL_GetAudioDeviceFormat(audioDeviceID, &outputSpec, nullptr);

			segundoStream = SDL_CreateAudioStream(&segundoWavSpec, &outputSpec);

			if (segundoStream) {
				std::cout << "✓ Stream del segundo audio creado" << std::endl;
			}
		}
		else {
			std::cerr << "✗ Error cargando segundo audio: " << SDL_GetError() << std::endl;
		}
	}
	// ========================================
	// FIN DE INICIALIZACIÓN DE AUDIO
	// ========================================

	// GLFW window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL - Camera Ready", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// Tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// GLAD: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// Setup floor
	setupFloor();
	floorTexture = generateTextures("Texturas/adoquin.jpg", false);
	texturaRuido = generateTextures("Texturas/textura_nubes.jpg", false);

	// Build and compile shaders
	Shader primitiveShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs");
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");
	Shader shaderFuente("shaders/fuente.vs", "shaders/fuente.fs");
	Shader shaderCorazon("shaders/heart.vs", "shaders/heart.fs");
	Shader alienShader("shaders/alien.vs", "shaders/alien.fs");


	// Skybox
	vector<std::string> faces{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};
	Skybox skybox = Skybox(faces);

	// Shader configuration
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// ========================================
	// MODELOS
	// ========================================

	Model eroda("resources/objects/estructura/arquitectura.obj");
	Model sala1("resources/objects/sala1/sala1.obj");
	Model cofre1T("resources/objects/sala1/tapacofre.obj");
	Model cofre2B("resources/objects/sala1/basecofre.obj");
	//POSEIDON
	Model poseidon("resources/objects/sala1/poseidon.obj");
	Model PbrazoI("resources/objects/sala1/PbrazoI.obj");
	Model PcodoI("resources/objects/sala1/PcodoI.obj");
	Model PbrazoR("resources/objects/sala1/PbrazoR.obj");
	Model PcodoR("resources/objects/sala1/PcodoR.obj");
	//AVE
	Model cuerpoAve("resources/objects/exterior/cuerpoAve.obj");
	Model alaDer("resources/objects/exterior/alaDer.obj");
	Model alaIzq("resources/objects/exterior/alaIzq.obj");
	//sala1
	Model silla("resources/objects/exterior/silla.obj");
	Model separador("resources/objects/sala1/separador.obj");
	Model banca("resources/objects/sala1/banca.obj");
	Model penelope("resources/objects/sala1/penelope.obj");
	Model jarron("resources/objects/sala1/jarron.obj");

	//exterior
	Model fuente("resources/objects/exterior/fuente.obj");
	Model erodaCartel("resources/objects/exterior/ERODA.obj");
	//centro
	Model esfera("resources/objects/centro/esfera.obj");
	Model aro1("resources/objects/centro/aro1.obj");
	Model aro2("resources/objects/centro/aro2.obj");
	Model aro3("resources/objects/centro/aro3.obj");
	Model aro4("resources/objects/centro/aro4.obj");
	Model aro5("resources/objects/centro/aro5.obj");

	ModelAnim leo("resources/objects/guardia/guardia.dae");
	leo.initShaders(animShader.ID);


	//Sala 2 kike
	Model Sala2("resources/objects/Sala2/Sala2k.obj");
	//Alien
	Model Alien("resources/objects/Sala2/Alien3.obj");
	//Bird
	Model Bird("resources/objects/Sala2/Bird.obj");
	//Bull por partes
	Model Bull("resources/objects/Sala2/Bull.obj");
	Model HeadBull("resources/objects/Sala2/HeadBull.obj");
	//Otros modelos
	Model Estatua("resources/objects/Sala2/Estatua.obj");
	Model Thinker("resources/objects/Sala2/Thinker.obj");



	// Debug: Ver qué texturas se cargaron
	std::cout << "=== DEBUG MODELO ===" << std::endl;
	std::cout << "Modelo cargado. Meshes: " << eroda.meshes.size() << std::endl;

	for (unsigned int i = 0; i < eroda.meshes.size(); i++) {
		std::cout << "Mesh " << i << " tiene " << eroda.meshes[i].textures.size() << " texturas:" << std::endl;
		for (unsigned int j = 0; j < eroda.meshes[i].textures.size(); j++) {
			std::cout << "  - Tipo: " << eroda.meshes[i].textures[j].type
				<< " | ID: " << eroda.meshes[i].textures[j].id
				<< " | Path: " << eroda.meshes[i].textures[j].path << std::endl;

			GLint width;
			glBindTexture(GL_TEXTURE_2D, eroda.meshes[i].textures[j].id);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
			if (width > 0) {
				std::cout << " Textura cargada OK (ancho: " << width << "px)" << std::endl;
			}
			else {
				std::cout << "  ERROR: Textura NO cargada correctamente" << std::endl;
			}
		}
		if (eroda.meshes[i].textures.size() == 0) {
			std::cout << "  *** ESTE MESH NO TIENE TEXTURAS ***" << std::endl;
		}
	}
	std::cout << "===================" << std::endl;

	// Create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);
	glm::mat4 viewOp = glm::mat4(1.0f);
	glm::mat4 projectionOp = glm::mat4(1.0f);
	
	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Per-frame time logic
		lastFrame = SDL_GetTicks();

		// Render
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Setup shader for static models
		staticShader.use();
		staticShader.setInt("material_diffuse", 0);
		staticShader.setInt("material_specular", 1);

		// Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);

		// Luz direccional
		staticShader.setVec3("dirLight.direction", glm::vec3(0.2f, -0.8f, -0.5f));
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.45f, 0.45f, 0.45f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.75f, 0.75f, 0.75f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.3f, 0.3f, 0.3f));

		// Luz puntual
		staticShader.setVec3("pointLight[0].position", glm::vec3(0.0f, 20.0f, -70.0f));
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.15f, 0.15f, 0.15f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.5f, 0.5f, 0.5f));
		staticShader.setFloat("pointLight[0].constant", 1.0f);
		staticShader.setFloat("pointLight[0].linear", 0.007f);
		staticShader.setFloat("pointLight[0].quadratic", 0.0002f);

		// Luz puntual 1 (¡ACTIVADA! - Luz del Holograma)
		glm::vec3 posBaseHolograma = glm::vec3(26.3213f, 3.00009f, -2.54276f); // Coordenadas Sala 3
		float alturaOscilacionHolo = sin((float)SDL_GetTicks() / 1000.0f * 2.0f) * 0.5f;
		glm::vec3 posLuzHolo = posBaseHolograma + glm::vec3(0.0f, alturaOscilacionHolo, 0.0f);

		staticShader.setVec3("pointLight[1].position", posLuzHolo); // La luz se mueve con el holograma
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.15f, 0.05f, 0.05f)); // Rojo suave
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(1.0f, 0.2f, 0.2f)); // Rojo brillante
		staticShader.setVec3("pointLight[1].specular", glm::vec3(1.0f, 0.2f, 0.2f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.09f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		// Material
		staticShader.setFloat("material_shininess", 32.0f);

		// View/projection transformations
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);
		// ========================================
		// DIBUJAR MODELOS
		// ========================================
		staticShader.use();
		//estructura
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, -1.2f, -40.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.3f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		eroda.Draw(staticShader);

		//sala1
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-25.6f, -1.8f, -7.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.2f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		sala1.Draw(staticShader);
		//sala1 penelope
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-10.4f, -0.7f, -5.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		penelope.Draw(staticShader);
		//sala1 jarron
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-10.4f, -0.85f, -9.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		jarron.Draw(staticShader);
		// ========== SEPARADOR ==========
		//1
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-18.5f, -0.7f, 0.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.09f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		separador.Draw(staticShader);
		//2
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-18.5f, -0.7f, -2.7f));
		modelOp = glm::scale(modelOp, glm::vec3(0.09f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		separador.Draw(staticShader);
		//3
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-18.65f, -0.7f, 0.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.09f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		separador.Draw(staticShader);
		//4
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-14.9f, -0.7f, 0.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.09f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		separador.Draw(staticShader);
		// ========== SEPARADOR FIN ==========
		//============BANCA===================
		// 1
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-17.0f, -0.7f, -5.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		banca.Draw(staticShader);
		// 2
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-17.0f, -0.7f, 3.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		banca.Draw(staticShader);
		// ==========BANCA FIN ===============
		//==========CENTRO ===============
		tiempoEsfera += deltaTime / 1000.0f;
		// Actualizar rotaciones
		rotacionEsfera += velocidadEsfera * (deltaTime / 1000.0f);
		rotacionAro1 += velocidadAro1 * (deltaTime / 1000.0f);
		rotacionAro2 += velocidadAro2 * (deltaTime / 1000.0f);
		rotacionAro3 += velocidadAro3 * (deltaTime / 1000.0f);
		rotacionAro4 += velocidadAro4 * (deltaTime / 1000.0f);
		rotacionAro5 += velocidadAro5 * (deltaTime / 1000.0f);
		// Calcular pulsación
		pulsacionEsfera = 1.0f + sin(tiempoEsfera * velocidadPulsacion) * amplitudPulsacion;
		// Posición base del centro
		glm::vec3 centroPos(0.0f, 2.0f, 0.0f);
		// ===== ESFERA CENTRAL =====
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), centroPos);
		modelOp = glm::scale(modelOp, glm::vec3(0.04f * pulsacionEsfera)); // Pulsación
		modelOp = glm::rotate(modelOp, glm::radians(rotacionEsfera), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación Y
		modelOp = glm::rotate(modelOp, glm::radians(rotacionEsfera * 0.5f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación X
		staticShader.setMat4("model", modelOp);
		esfera.Draw(staticShader);
		// ===== ARO 1 - Rotación en eje X =====
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), centroPos);
		modelOp = glm::rotate(modelOp, glm::radians(rotacionAro1), glm::vec3(1.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.04f));
		staticShader.setMat4("model", modelOp);
		aro1.Draw(staticShader);
		// ===== ARO 2 - Rotación en eje Y =====
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), centroPos);
		modelOp = glm::rotate(modelOp, glm::radians(rotacionAro2), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.04f));
		staticShader.setMat4("model", modelOp);
		aro2.Draw(staticShader);
		// ===== ARO 3 - Rotación en eje Z =====
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), centroPos);
		modelOp = glm::rotate(modelOp, glm::radians(rotacionAro3), glm::vec3(0.0f, 0.0f, 1.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.04f));
		staticShader.setMat4("model", modelOp);
		aro3.Draw(staticShader);
		// ===== ARO 4 - Rotación diagonal (X+Y) =====
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), centroPos);
		modelOp = glm::rotate(modelOp, glm::radians(rotacionAro4), glm::vec3(1.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.04f));
		staticShader.setMat4("model", modelOp);
		aro4.Draw(staticShader);
		// ===== ARO 5 - Rotación diagonal (Y+Z) =====
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), centroPos);
		modelOp = glm::rotate(modelOp, glm::radians(rotacionAro5), glm::vec3(0.0f, 1.0f, 1.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.04f));
		staticShader.setMat4("model", modelOp);
		aro5.Draw(staticShader);
		//==========CENTRO FIN ===============
		// //sala3
		//Model holograma("resources/objects/sala3/Heart.fbx");
		
		// ========== COFRE ==========
		if (cofreAbierto && anguloTapaCofre < anguloMaximoCofre) {
			anguloTapaCofre += velocidadAperturaCofre * (deltaTime / 1000.0f);
			if (anguloTapaCofre > anguloMaximoCofre)
				anguloTapaCofre = anguloMaximoCofre;
		}
		else if (!cofreAbierto && anguloTapaCofre > 0.0f) {
			anguloTapaCofre -= velocidadAperturaCofre * (deltaTime / 1000.0f);
			if (anguloTapaCofre < 0.0f)
				anguloTapaCofre = 0.0f;
		}

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-10.3f, -0.7f, 4.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		cofre2B.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-10.3f, -0.7f, 4.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, 7.5f, -5.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-anguloTapaCofre), glm::vec3(1.0f, 0.0f, 0.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, -7.5f, 5.0f));
		staticShader.setMat4("model", modelOp);
		cofre1T.Draw(staticShader);
		// ========== COFRE FIN ==========

		// ========== POSEIDON ==========
		tiempoAnimacion += deltaTime / 1000.0f;
		float t = tiempoAnimacion;

		float duracionTransicion = 2.0f;
		float progreso = glm::min(t / duracionTransicion, 1.0f);
		float suavizado = progreso * progreso * (3.0f - 2.0f * progreso);

		if (estadoAnimacion == 1) {
			float ondaSaludo = sin(tiempoAnimacion * 5.0f) * 20.0f;

			anguloHombroIzq = glm::mix(0.0f, 120.0f, suavizado);
			rotacionHombroIzqY = glm::mix(0.0f, 40.0f, suavizado);
			rotacionHombroIzqZ = glm::mix(0.0f, 0.0f, suavizado);

			anguloCodoIzq = glm::mix(0.0f, -110.0f + ondaSaludo, suavizado);
			rotacionCodoIzqY = glm::mix(0.0f, 0.0f, suavizado);
			rotacionCodoIzqZ = glm::mix(0.0f, 0.0f, suavizado);

			anguloHombroDer = glm::mix(0.0f, 100.0f, suavizado);
			rotacionHombroDerY = glm::mix(0.0f, 30.0f, suavizado);
			rotacionHombroDerZ = glm::mix(0.0f, 20.0f, suavizado);

			anguloCodoDer = glm::mix(0.0f, 30.0f, suavizado);
			rotacionCodoDerY = glm::mix(0.0f, 0.0f, suavizado);
			rotacionCodoDerZ = glm::mix(0.0f, 0.0f, suavizado);
		}

		glm::mat4 poseidonBase = glm::mat4(1.0f);
		poseidonBase = glm::translate(poseidonBase, glm::vec3(-17.0f, -0.7f, -1.0f));
		poseidonBase = glm::scale(poseidonBase, glm::vec3(0.3f));
		poseidonBase = glm::rotate(poseidonBase, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		modelOp = poseidonBase;
		staticShader.setMat4("model", modelOp);
		poseidon.Draw(staticShader); 

		glm::vec3 pivotBrazoI = glm::vec3(1.608f, 10.917f, 0.383f);
		glm::vec3 pivotCodoI = glm::vec3(3.883f, 10.862f, 1.168f);
		glm::vec3 pivotBrazoR = glm::vec3(-1.476f, 11.083f, 0.417f);
		glm::vec3 pivotCodoR = glm::vec3(-3.624f, 11.668f, 1.652f);

		glm::mat4 brazoIzq = poseidonBase;
		brazoIzq = glm::translate(brazoIzq, pivotBrazoI);
		brazoIzq = glm::rotate(brazoIzq, glm::radians(rotacionHombroIzqZ), glm::vec3(0.0f, 0.0f, 1.0f));
		brazoIzq = glm::rotate(brazoIzq, glm::radians(anguloHombroIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		brazoIzq = glm::rotate(brazoIzq, glm::radians(rotacionHombroIzqY), glm::vec3(0.0f, 1.0f, 0.0f));
		brazoIzq = glm::translate(brazoIzq, -pivotBrazoI);

		modelOp = brazoIzq;
		staticShader.setMat4("model", modelOp);
		PbrazoI.Draw(staticShader);

		glm::mat4 codoIzq = brazoIzq;
		codoIzq = glm::translate(codoIzq, pivotCodoI);
		codoIzq = glm::rotate(codoIzq, glm::radians(rotacionCodoIzqZ), glm::vec3(0.0f, 0.0f, 1.0f));
		codoIzq = glm::rotate(codoIzq, glm::radians(anguloCodoIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		codoIzq = glm::rotate(codoIzq, glm::radians(rotacionCodoIzqY), glm::vec3(0.0f, 1.0f, 0.0f));
		codoIzq = glm::translate(codoIzq, -pivotCodoI);

		modelOp = codoIzq;
		staticShader.setMat4("model", modelOp);
		PcodoI.Draw(staticShader);

		glm::mat4 brazoDer = poseidonBase;
		brazoDer = glm::translate(brazoDer, pivotBrazoR);
		brazoDer = glm::rotate(brazoDer, glm::radians(rotacionHombroDerZ), glm::vec3(0.0f, 0.0f, 1.0f));
		brazoDer = glm::rotate(brazoDer, glm::radians(anguloHombroDer), glm::vec3(1.0f, 0.0f, 0.0f));
		brazoDer = glm::rotate(brazoDer, glm::radians(rotacionHombroDerY), glm::vec3(0.0f, 1.0f, 0.0f));
		brazoDer = glm::translate(brazoDer, -pivotBrazoR);

		modelOp = brazoDer;
		staticShader.setMat4("model", modelOp);
		PbrazoR.Draw(staticShader);

		glm::mat4 codoDer = brazoDer;
		codoDer = glm::translate(codoDer, pivotCodoR);
		codoDer = glm::rotate(codoDer, glm::radians(rotacionCodoDerZ), glm::vec3(0.0f, 0.0f, 1.0f));
		codoDer = glm::rotate(codoDer, glm::radians(anguloCodoDer), glm::vec3(1.0f, 0.0f, 0.0f));
		codoDer = glm::rotate(codoDer, glm::radians(rotacionCodoDerY), glm::vec3(0.0f, 1.0f, 0.0f));
		codoDer = glm::translate(codoDer, -pivotCodoR);

		modelOp = codoDer;
		staticShader.setMat4("model", modelOp);
		PcodoR.Draw(staticShader);
		// ========== POSEIDON FIN==========

		//========== HOLOGRAMA CORAZÓN (SALA 3) ===============
		/* {
			// 1. Activa el nuevo shader
			shaderCorazon.use();
			shaderCorazon.setMat4("projection", projectionOp);
			shaderCorazon.setMat4("view", viewOp);

			// 2. CÁLCULO DE ANIMACIÓN "KEYFRAME" (CPU)
			float tiempoHolo = (float)SDL_GetTicks() / 1000.0f;
			float anguloRotacion = tiempoHolo * 45.0f;
			float alturaOscilacion = sin(tiempoHolo * 2.0f) * 0.5f;

			// 3. CONSTRUIR MATRIZ DE MODELO
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			glm::vec3 posBaseHolograma = glm::vec3(26.3213f, 3.00009f, -2.54276f);

			modelMatrix = glm::translate(modelMatrix, posBaseHolograma);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, alturaOscilacion, 0.0f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(anguloRotacion), glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0000001f)); // Escala visible

			// 4. PASAR UNIFORMS AL SHADER
			shaderCorazon.setMat4("u_model", modelMatrix);
			shaderCorazon.setVec3("u_cameraPos", camera.Position);
			shaderCorazon.setVec3("u_holoColor", glm::vec3(1.0f, 0.1f, 0.1f)); // Color Rojo Corazón

			// 5. DIBUJAR MANUALMENTE
			for (unsigned int i = 0; i < holograma.meshes.size(); i++)
			{
				glBindVertexArray(holograma.meshes[i].VAO);
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(holograma.meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}*/
		//===========================================

		// ========== AVE CON ANIMACIÓN DE VUELO CIRCULAR ==========
		tiempoVueloAve += velocidadVueloAve * (deltaTime / 1000.0f);

		float anguloVuelo = tiempoVueloAve;
		float x = centroVuelo.x + cos(anguloVuelo) * radioVuelo;
		float z = centroVuelo.z + sin(anguloVuelo) * radioVuelo;
		float y = alturaVuelo;

		glm::vec3 posicionAve = glm::vec3(x, y, z);

		float anguloRotacion = atan2(sin(anguloVuelo), cos(anguloVuelo));
		anguloRotacion = glm::degrees(anguloRotacion) + 90.0f;
		// Aleteo de las alas
		float tiempoAleteo = tiempoVueloAve * velocidadAleteo;
		anguloAleteo = sin(tiempoAleteo) * amplitudAleteo;

		glm::vec3 pivotAlaDer = glm::vec3(-6.6903f * 0.15f, 0.391823f * 0.15f, 11.9203f * 0.15f);
		glm::vec3 pivotAlaIzq = glm::vec3(-6.60685f * 0.15f, 0.197859f * 0.15f, 10.157f * 0.15f);

		glm::mat4 avePosicion = glm::mat4(1.0f);
		avePosicion = glm::translate(avePosicion, posicionAve);
		avePosicion = glm::rotate(avePosicion, glm::radians(anguloRotacion), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 cuerpoAveBase = avePosicion;
		cuerpoAveBase = glm::scale(cuerpoAveBase, glm::vec3(0.15f));

		modelOp = cuerpoAveBase;
		staticShader.setMat4("model", modelOp);
		cuerpoAve.Draw(staticShader);

		glm::mat4 alaIzqMat = avePosicion;
		alaIzqMat = glm::scale(alaIzqMat, glm::vec3(0.15f));
		alaIzqMat = glm::translate(alaIzqMat, pivotAlaIzq);
		alaIzqMat = glm::rotate(alaIzqMat, glm::radians(anguloAleteo), glm::vec3(1.0f, 0.0f, 0.0f));
		alaIzqMat = glm::translate(alaIzqMat, -pivotAlaIzq);
		modelOp = alaIzqMat;
		staticShader.setMat4("model", modelOp);
		alaIzq.Draw(staticShader);

		glm::mat4 alaDerMat = avePosicion;
		alaDerMat = glm::scale(alaDerMat, glm::vec3(0.15f));
		alaDerMat = glm::translate(alaDerMat, pivotAlaDer);
		alaDerMat = glm::rotate(alaDerMat, glm::radians(-anguloAleteo), glm::vec3(1.0f, 0.0f, 0.0f));
		alaDerMat = glm::translate(alaDerMat, -pivotAlaDer);
		modelOp = alaDerMat;
		staticShader.setMat4("model", modelOp);
		alaDer.Draw(staticShader);
		//// ========== AVE FIN==========
		//// ========== EXTRAS==========
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.2f, 15.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		staticShader.setMat4("model", modelOp);
		silla.Draw(staticShader);

		// ===== DIBUJAR FUENTE (MÉTODO POR MALLA - SOLUCIÓN MANUAL) =====
		
		// 1. Definimos la matriz del modelo base (Piedra/Pasto)
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, -1.0f, 30.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.5f));

		// --- PASE 1: Dibujar OPACOS (Piedra y "Base de Piedra") ---
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);
		staticShader.setMat4("model", modelOp);

		// Pasamos todas las luces al staticShader
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", glm::vec3(0.2f, -0.8f, -0.5f));
		staticShader.setVec3("pointLight[0].position", glm::vec3(0.0f, 20.0f, -70.0f));
		// (Asegúrate de que todas las luces que staticShader usa estén aquí)
		staticShader.setFloat("material_shininess", 32.0f);

		// 1. Obtenemos la textura de piedra de la malla 0
		unsigned int stoneTextureID = fuente.meshes[0].textures[0].id;

		// 2. Dibujamos las mallas
		for (unsigned int i = 0; i < fuente.meshes.size(); i++)
		{
			if (i <= 4) // Mallas 0, 1, 2, 3, 4 (Piedra y Base)
			{
				if (i <= 2) // La piedra original
				{
					fuente.meshes[i].Draw(staticShader);
				}
				else // Mallas 3, 4 (Pasto y Base)
				{
					// Forzamos la textura de piedra en estas mallas
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, stoneTextureID);
					// Dibujamos la malla manualmente
					glBindVertexArray(fuente.meshes[i].VAO);
					glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(fuente.meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
				}
			}
		}

		// --- PASE 2: Dibujar TRANSPARENTES (Agua) ---
		glDepthMask(GL_FALSE); // Desactivamos escritura de profundidad
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		// Creamos una NUEVA matriz para el agua, 0.01f más arriba
		glm::mat4 modelAgua = glm::translate(modelOp, glm::vec3(0.0f, 0.01f, 0.0f));

		// Configuramos y usamos el shader de agua
		shaderFuente.use();
		shaderFuente.setMat4("projection", projectionOp);
		shaderFuente.setMat4("view", viewOp);
		shaderFuente.setMat4("model", modelAgua); // Usamos la matriz elevada

		// Pasamos los uniforms del agua
		float tiempoActual = (float)SDL_GetTicks() / 1000.0f;
		shaderFuente.setFloat("u_time", tiempoActual);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaRuido);
		shaderFuente.setInt("u_noiseTexture", 0);

		// Dibujamos solo las mallas de agua
		for (unsigned int i = 0; i < fuente.meshes.size(); i++)
		{
			if (i > 3) //Mallas de agua (5, 6, 7)
			{
				// Usamos Draw() porque fuente.vs/fs son compatibles
				fuente.meshes[i].Draw(shaderFuente);
			}
		}
		
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		// ============================================
		
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-25.0f, -0.5f, 3.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.7f));
		staticShader.setMat4("model", modelOp);
		erodaCartel.Draw(staticShader);
		// ========== EXTRAS FIN==========

		animShader.use();
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("viewPos", camera.Position);

		animShader.setVec3("light.direction", glm::vec3(0.2f, -0.8f, -0.5f));
		animShader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		animShader.setVec3("light.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		animShader.setVec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));

		animShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		animShader.setFloat("material.shininess", 32.0f);

		animShader.setInt("texture_diffuse1", 0);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -0.8f, 15.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.01999f));
		animShader.setMat4("model", modelOp);

		leo.Draw(animShader);

		////sala2
		staticShader.use();
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(13.1f, -1.0f, -6.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.2f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		Sala2.Draw(staticShader);

		// ========== Toro ==========
		if (upHead && anguloHead < anguloMax) {
			anguloHead += velocidad * (deltaTime / 1000.0f);
			if (anguloHead > anguloMax)
				anguloHead = anguloMax;
		}
		else if (!upHead && anguloHead > 0.0f) {
			anguloHead -= velocidad * (deltaTime / 1000.0f);
			if (anguloHead < 0.0f)
				anguloHead = 0.0f;
		}
		staticShader.setMat4("model", modelOp);
		Bull.Draw(staticShader);

		
		glm::mat4 modelHead = modelOp; 
		modelHead = glm::translate(modelHead, glm::vec3(0.0f, 1.2f, 0.0f));
		modelHead = glm::rotate(modelHead, glm::radians(anguloHead), glm::vec3(1.0f, 0.0f, 1.0f));
		modelHead = glm::translate(modelHead, glm::vec3(0.0f, -1.2f, 0.0f));
		staticShader.setMat4("model", modelHead);
		HeadBull.Draw(staticShader);

		//Bird
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, -1.27f, -49.0f));
		//modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		staticShader.setMat4("model", modelOp);
		Bird.Draw(staticShader);

		//Estatua
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.2f, -1.27f, -36.5f));
		modelOp = glm::scale(modelOp, glm::vec3(1.2f));
		staticShader.setMat4("model", modelOp);
		Estatua.Draw(staticShader);

		//Thinker
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, -1.5f, -36.5f));
		//modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		staticShader.setMat4("model", modelOp);
		Thinker.Draw(staticShader);
		//Alien
		alienShader.use();
		alienShader.setMat4("projection", projectionOp);
		alienShader.setMat4("view", viewOp);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -1.0f, -31.0f));
		alienShader.setMat4("model", modelOp);

		// Uniforms del shader
		alienShader.setVec3("lightColor", glm::vec3(1.0f));
		alienShader.setVec3("lightPos", glm::vec3(0.0f, 20.0f, -70.0f));
		alienShader.setVec3("viewPos", camera.Position);

		// Pasar tiempo y textura de ruido
		tiempoActual = (float)SDL_GetTicks() / 1000.0f;
		alienShader.setFloat("u_time", tiempoActual);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texturaRuido); // usa la misma textura que la fuente
		alienShader.setInt("u_noiseTexture", 1);

		// Dibuja el modelo Alien3
		Alien.Draw(alienShader);


		primitiveShader.use();
		primitiveShader.setMat4("projection", projectionOp);
		primitiveShader.setMat4("view", viewOp);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		primitiveShader.setMat4("model", modelOp);
		primitiveShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);

		//


		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glBindVertexArray(VAO_floor);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		deltaTime = SDL_GetTicks() - lastFrame;
		if (deltaTime < LOOP_TIME) {
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//Audio 1 Saludo
	if (primerStream) {
		SDL_DestroyAudioStream(primerStream);
		primerStream = nullptr;
	}

	// Audio 2 Pajaros
	if (segundoStream) {
		SDL_DestroyAudioStream(segundoStream);
		segundoStream = nullptr;
	}

	if (audioDeviceID > 0) {
		SDL_CloseAudioDevice(audioDeviceID);
		audioDeviceID = 0;
	}

	if (primerBuffer) {
		SDL_free(primerBuffer);
		primerBuffer = nullptr;
	}

	if (segundoBuffer) {
		SDL_free(segundoBuffer);
		segundoBuffer = nullptr;
	}

	if (audioInicializado) {
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}

	glfwTerminate();
	return 0;
}

void my_input(GLFWwindow* window, int key, int scancode, int action, int mode) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);

	// === AÑADIR ESTE BLOQUE ===
	// Tecla C: Imprimir Coordenadas de la Cámara
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		std::cout << "=== POSICION ACTUAL ===" << std::endl;
		std::cout << "X: " << camera.Position.x << "f, Y: " << camera.Position.y << "f, Z: " << camera.Position.z << "f" << std::endl;
		std::cout << "=======================" << std::endl;
	}
	// ==========================

	// Cofre
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		cofreAbierto = !cofreAbierto;

	// Poseidon
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		estadoAnimacion = (estadoAnimacion == 0) ? 1 : 0;
		tiempoAnimacion = 0.0f;
	}

	if (key == GLFW_KEY_B && action == GLFW_PRESS)
		upHead = !upHead;
	// ========== CÁMARAS POSICIONES ==========
	// Tecla 4: Ir al Toro
	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		camera.Position = posicionBull;
		camera.Yaw = 50.0f;
		camera.Pitch = -30.0f;
		camera.updateCameraVectors();
	}

	// ========== CÁMARAS POSICIONES ==========
	// Tecla 1: Ir al cofre
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		camera.Position = posicionCofre;
		camera.Yaw = 50.0f;
		camera.Pitch = -30.0f;
		camera.updateCameraVectors();
	}

	// Tecla 2: Ir a Esfera
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		camera.Position = posicionEsfera;
		camera.Yaw = -120.0f;
		camera.Pitch = -10.0f;
		camera.updateCameraVectors();
	}

	// Tecla 3: Vista general
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		camera.Position = posicionGeneral;
		camera.Yaw = -90.0f;
		camera.Pitch = 10.0f;
		camera.updateCameraVectors();
	}
	// Tecla R: Reproducir SALUDO
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		reproducirPrimerAudio();
	}

	// Tecla T: Reproducir PAJAROS
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		reproducirSegundoAudio();
	}

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}