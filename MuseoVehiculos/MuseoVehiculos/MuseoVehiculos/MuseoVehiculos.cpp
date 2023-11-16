/*---------------------------------------------------------*/
/* ----------------  Práctica                   -----------*/
/*-----------------    2024-1   ---------------------------*/
/*------------- Alumno:                     ---------------*/
/*------------- No. Cuenta                  ---------------*/
#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>      //main
#include <stdlib.h>           
#include <glm/glm.hpp>  //camera y model
#include <glm/gtc/matrix_transform.hpp>   //camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <vector>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>  //Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

float posicionX, posicionY, posicionZ = 0.0f;
// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;
GLuint VBO[2], VAO[2], EBO[2];
void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 0.0f,	10.0f));
float MovementSpeed = 0.1f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double deltaTime = 0.0f,
lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

// Variabiles para animacion de avion
bool animacionAvion = true;
float avionMovX = 345.0f;
float avionMovY = 0.1f;
float avionMovZ = -150.0f;
float orientacionAvionX = 0.0f;
float orientacionAvionY = 180.0f;
float movAuxX = 0.0f;
float movAuxY = 0.0f;
float movAuxZ = 0.0f;
float angulo = 0.0f;
float giroAvion = 0.0f;
int estadoAvion = 0;

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	float posOpelX = 0.0f;
	float posOpelY = 0.0f;
	float posOpelZ = 0.0f;
	float rotOpelRueda = 0.0f;
	float rotOpelY = 0.0f;
	float rotOpelFaro = 0.0f;
	float rotOpelPuerta = 0.0f;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;//introducir número en caso de tener Key guardados
bool play = false;
int playIndex = 0;
//
//Keyframes (Manipulación y dibujo)
float	posOpelX = 0.0f,
posOpelY = 0.0f,
posOpelZ = 0.0f,
rotOpelRueda = 0.0f,
rotOpelY = 0.0f,
rotOpelFaro = 0.0f,
rotOpelPuerta = 0.0f;
int		incPosOpelX = 0,
incPosOpelY = 0,
incPosOpelZ = 0,
incRotOpelRueda = 0,
incRotOpelY = 0,
incRotOpelFaro = 0,
incRotOpelPuerta = 0;

void resetElements(void)
{
	posOpelX = KeyFrame[0].posOpelX;
	posOpelY = KeyFrame[0].posOpelY;
	posOpelZ = KeyFrame[0].posOpelZ;
	rotOpelRueda = KeyFrame[0].rotOpelRueda;
	rotOpelY = KeyFrame[0].rotOpelY;
	rotOpelFaro = KeyFrame[0].rotOpelFaro;
	rotOpelPuerta = KeyFrame[0].rotOpelPuerta;
}
//
void interpolation(void)
{
	incPosOpelX = (KeyFrame[playIndex + 1].posOpelX - KeyFrame[playIndex].posOpelX) / i_max_steps;
		incPosOpelY = (KeyFrame[playIndex+1].posOpelY - KeyFrame[playIndex].posOpelY)/i_max_steps;
		incPosOpelZ = (KeyFrame[playIndex+1].posOpelZ - KeyFrame[playIndex].posOpelZ)/i_max_steps;
		incRotOpelRueda = (KeyFrame[playIndex+1].rotOpelRueda - KeyFrame[playIndex].rotOpelRueda)/i_max_steps;
		incRotOpelY = (KeyFrame[playIndex+1].rotOpelY - KeyFrame[playIndex].rotOpelY)/i_max_steps;
		incRotOpelFaro = (KeyFrame[playIndex+1].rotOpelFaro - KeyFrame[playIndex].rotOpelFaro)/i_max_steps;
		incRotOpelPuerta = (KeyFrame[playIndex+1].rotOpelPuerta - KeyFrame[playIndex].rotOpelPuerta)/i_max_steps;
}


void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
				posOpelX += incPosOpelX;
				posOpelY += incPosOpelX;
				posOpelZ += incPosOpelZ;
				rotOpelRueda += incRotOpelRueda;
				rotOpelY += incRotOpelY;
				rotOpelFaro += incRotOpelFaro;
				rotOpelPuerta += incRotOpelPuerta;
			i_curr_steps++;
		}

	}


	if (animacionAvion)
	{
		if (estadoAvion == 0) {
			avionMovZ -= 0.1f;
			if (avionMovZ <= -50.0f) {
				std::cout << "Termina el estado 0" << std::endl;
				estadoAvion = 1;
			}
		}
		if (estadoAvion == 1) {
			avionMovY += 0.1 * 0.625f;
			avionMovZ -= 0.1f;

			if (avionMovY >= 50.0f) {
				std::cout << "Coord Y" << avionMovY << "Coord Z" << avionMovZ << std::endl;
				estadoAvion = 2;
			}
		}
		if (estadoAvion == 2) {
			angulo -= 0.01;
			avionMovX = 55 * cos(angulo) - 55;
			avionMovZ = sqrt((55 * 55) - (cos(angulo) * cos(angulo))) * sin(angulo) - 131;


			std::cout << "X: " << avionMovX << ", Z: " << avionMovZ << ", Angulo: " << angulo << std::endl;
			if (avionMovX <= -109.0f) {
				estadoAvion = 3;
			}

		}

	}


}
void generateWaves(std::vector<float>& vertices, std::vector<unsigned int>& indices, int numWaves, float length, float amplitude, float time) {
	// Limpiar el vector de vértices e índices
	vertices.clear();
	indices.clear();

	// Número de puntos a lo largo de la función senoidal
	const int numPoints = 1200;
	// Separación entre serpientes en el eje Z
	const float separationZ = 33.4f;

	// Calcular las coordenadas x, y, y z de la función senoidal para simular el movimiento de las serpientes
	for (int i = 0; i <= numPoints; ++i) {
		float t = (i / static_cast<float>(numPoints)) * length;

		// Generar una serpiente con desfase para cada iteración
		for (int j = 0; j < numWaves; ++j) {
			float x = t; // Ajustar el valor del desfase horizontal aquí
			float y = amplitude * sin(t + time ); // Onda en la vertical con desfase
			float z = j * separationZ; // Ajustar la separación en el eje Z aquí
			// Agregar las coordenadas al vector de vértices
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}
}


void getResolution()
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC 20241", NULL, NULL);
	if (window == NULL)
	{
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

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// Definición de los vértices del plano
	std::vector<float> vertices ={
	};
	// Definición de los índices para GL_TRIANGLE_FAN
	std::vector<unsigned int> indices = {};
	indices.push_back(0);

	// Agrega los índices para los vértices en sentido contrario a las agujas del reloj
	for (unsigned int i = 19; i > 0; --i) {
		indices.push_back(i);
	}

	// Agrega los índices para los vértices en sentido de las agujas del reloj
	for (unsigned int i = 1; i <= 19; ++i) {
		indices.push_back(i);
	}
	// Vertex Buffer Object (VBO) and Vertex Array Object (VAO)
	glGenVertexArrays(1, &VAO[0]);
	glGenBuffers(1, &VBO[0]);

	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");
	Shader waterShader("Shaders/shaderAgua.vs", "Shaders/shaderAgua.fs");

	vector<std::string> faces
	{
		  "resources/skybox/right.jpg",
		  "resources/skybox/left.jpg",
		  "resources/skybox/top.jpg",
		  "resources/skybox/bottom.jpg",
		  "resources/skybox/front.jpg",
		  "resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	waterShader.use();
	// load models
	// -----------
	Model mapa("resources/objects/piso/mapa.obj");
	Model pisoArbustos("resources/objects/piso/pisoArbustos.obj");
	Model pisoCercas("resources/objects/piso/pisoCercas.obj");
	Model pisoMesas("resources/objects/piso/pisoMesas.obj");
	Model recepcion("resources/objects/piso/recepcion.obj");

	float time = 0.0f;

	Model avionAmarillo("resources/objects/avionAmarillo/avionAmarillo.obj");
	Model avionDespegue("resources/objects/avionDespegue/avionDespegue.obj");
	Model avionGuerra("resources/objects/avionGuerra/avionGuerra.obj");
	Model helicopteroDron("resources/objects/helicopteroDron/helicopteroDron.obj");
	Model helicopteroGuerra("resources/objects/helicopteroGuerra/helicopteroGuerra.obj");
	Model boteVeloz("resources/objects/boteVeloz/boteVeloz.obj");
	Model submarino("resources/objects/submarino/submarino.obj");
	Model YateFantasia("resources/objects/YateFantasia/yateFantasia.obj");
	Model avionMarino("resources/objects/avionMarino/avionMarino.obj");
	Model starwars("resources/objects/starwars/starwars.obj");
	Model vaisseau("resources/objects/Vaisseau/vassieau.obj");
	Model cruceroEspacial("resources/objects/cruceroEspacial/cruceroEspacial.obj");
	Model luminaris("resources/objects/Luminaris/luminaris.obj");
	
	//Modelos Opel
	Model OpelCarr("resources/objects/opelGt/carro.obj");
	Model OpelPuertaDer("resources/objects/opelGt/puertaDer.obj");
	Model OpelPuertaIzq("resources/objects/opelGt/puertaIzq.obj");
	Model OpelRueda("resources/objects/opelGt/ruedaIzq.obj");
	Model OpelFaro("resources/objects/opelGt/faroDer.obj");
	Model OpelCapo("resources/objects/opelGt/capo.obj");

	//Inicialización de KeyFrames

	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posOpelX=0;
		KeyFrame[i].posOpelY=0;
		KeyFrame[i].posOpelZ=0;
		KeyFrame[i].rotOpelRueda=0;
		KeyFrame[i].rotOpelY=0;
		KeyFrame[i].rotOpelFaro =0;
		KeyFrame[i].rotOpelPuerta=0;
	}

	KeyFrame[0].posOpelX = 0;
	KeyFrame[0].posOpelY = 0;
	KeyFrame[0].posOpelZ = 0;
	KeyFrame[0].rotOpelRueda = 0;
	KeyFrame[0].rotOpelY = 0;
	KeyFrame[0].rotOpelFaro = 0;
	KeyFrame[0].rotOpelPuerta = 0;

	KeyFrame[1].posOpelX = 0;
	KeyFrame[1].posOpelY = 0;
	KeyFrame[1].posOpelZ = 0;
	KeyFrame[1].rotOpelRueda = 0;
	KeyFrame[1].rotOpelY = 0;
	KeyFrame[1].rotOpelFaro = 0;
	KeyFrame[1].rotOpelPuerta = 0;

	KeyFrame[2].posOpelX = 0;
	KeyFrame[2].posOpelY = 0;
	KeyFrame[2].posOpelZ = 0;
	KeyFrame[2].rotOpelRueda = 0;
	KeyFrame[2].rotOpelY = 0;
	KeyFrame[2].rotOpelFaro = 0;
	KeyFrame[2].rotOpelPuerta = 0;

	KeyFrame[0].posOpelX = 0;
	KeyFrame[0].posOpelY = 0;
	KeyFrame[0].posOpelZ = 0;
	KeyFrame[0].rotOpelRueda = 0;
	KeyFrame[0].rotOpelY = 0;
	KeyFrame[0].rotOpelFaro = 0;
	KeyFrame[0].rotOpelPuerta = 0;
	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", ambientColor);
		staticShader.setVec3("dirLight.diffuse", diffuseColor);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);
		
		waterShader.use();
		generateWaves(vertices,indices, 10, 199.0f, 0.4, time);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		model = glm::translate(model, glm::vec3(-100.0f,-1.3f,-400));
		waterShader.setMat4("model", model);
		waterShader.setFloat("time", glfwGetTime());
		waterShader.setMat4("projection", projection);
		waterShader.setMat4("view", view);
		waterShader.setVec3("waterColor", glm::vec3(0.18f, 0.83f, 0.80f));
		// draw the triangle
		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
		// Establecer el grosor de la línea
		glLineWidth(15.0f);
		glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 3);
		time += 0.1;

		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);



		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		mapa.Draw(staticShader);


		model = glm::translate(glm::mat4(1.0f), glm::vec3(130.0f,.1f, -300.0f));
		//model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(6.5f));
		staticShader.setMat4("model", model);
		avionAmarillo.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, .1f, -300.0f));
		//model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(6.0f));
		staticShader.setMat4("model", model);
		avionGuerra.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(140.0f, .1f, -170.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(60.0f));
		staticShader.setMat4("model", model);
		helicopteroDron.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, .1f, -170.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f));
		staticShader.setMat4("model", model);
		helicopteroGuerra.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f + avionMovX, 0.0f + avionMovY, 0.0f + avionMovZ));
		model = glm::rotate(model, glm::radians(orientacionAvionY), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(orientacionAvionX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.5f));
		staticShader.setMat4("model", model);
		avionDespegue.Draw(staticShader);


		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		pisoArbustos.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		pisoCercas.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		pisoMesas.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-45.0f, 10.0f, -170.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.85f));
		staticShader.setMat4("model", model);
		avionMarino.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(45.0f, .1f, -140.0f));
		model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.052));
		staticShader.setMat4("model", model);
		submarino.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(70.0f, .1f, -330.0f));
		model = glm::rotate(model, glm::radians(235.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.85f));
		staticShader.setMat4("model", model);
		YateFantasia.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, .1f, -360.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.85f));
		staticShader.setMat4("model", model);
		boteVeloz.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 120.0f, -260.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		vaisseau.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-230.0f, 90.0f, -340.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(6.0f));
		staticShader.setMat4("model", model);
		starwars.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, 200.0f, -450.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.70f));
		staticShader.setMat4("model", model);
		cruceroEspacial.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-235.0f, 0.01f, -315.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		luminaris.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		recepcion.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-199.4f, 0.0f, -97.9f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		OpelCarr.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-200.0f, 0.0, -100.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		OpelPuertaDer.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-200.0f, 0.0, -100.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		OpelPuertaIzq.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-200.0f, 0.0, -100.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model); //der
		OpelFaro.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-207.0f, 0.0f, -100.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model); //izq
		OpelFaro.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-200.0f, 0.0f, -100.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model); //der
		OpelCapo.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-189.0f, -0.0f, -100.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		OpelRueda.Draw(staticShader);  // delante Izq
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3 ( -200.0f, -0.0f, -100.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		OpelRueda.Draw(staticShader);  // delante Der

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-189.0f, -0.0f, -121.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		OpelRueda.Draw(staticShader);  // tras Izq

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-200.0, -0.0f, -121.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		OpelRueda.Draw(staticShader);  // tras Der

		//model = glm::translate(model, glm::vec3(0.0f, -100.75f, 0.0f));
		//model = glm::scale(model, glm::vec3(10.0f));
		//staticShader.setMat4("model", model);
		//OpelRueda.Draw(staticShader);  // atras Izq

		//model = glm::translate(model, glm::vec3(0.0f, -100.75f, 0.0f));
		//model = glm::scale(model, glm::vec3(10.0f));
		//staticShader.setMat4("model", model);
		//OpelRueda.Draw(staticShader);  // atras Der
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// clean up resources
	glDeleteVertexArrays(1, &VAO[0]);
	glDeleteBuffers(1, &VBO[0]);

	skybox.Terminate();
	// Cerrar SDL
	///*SDL_CloseAudio();
	//SDL_FreeWAV(buffer);*/

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{
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
	
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		posicionX+= 0.1;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		posicionX-= 0.1;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		posicionY+= 0.1;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		posicionY-= 0.1;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		posicionZ+= 0.1;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		posicionZ-= 0.1;


}



// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}