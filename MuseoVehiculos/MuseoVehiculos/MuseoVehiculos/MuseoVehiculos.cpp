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

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;
GLuint VBO[2], VAO[2], EBO[2];
void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
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
	//Variables para GUARDAR Key Frames
	float posX; //Variable para PosicionX
	float posY;//Variable para PosicionY
	float posZ;//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;//introducir número en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void animate(void)
{
	if (play)
	{


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