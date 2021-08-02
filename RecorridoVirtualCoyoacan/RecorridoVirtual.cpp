//COMPUTADORA--------------------------* /
//INTEGRANTES:
// AGUILERA ROA MAURICIO ARTURO
// DÍAZ GARCÍA PORFIRIO 

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
#include <SDL/SDL.h>

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


unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
GLFWmonitor* monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 0.5f, 0.0f));
float MovementSpeed = 1.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(-15.0f, 10.0f, -20.0f); // Dirección de la luz Acomodada de acuerdo a la skybox
glm::vec3 lightDirection(0.0f, -1.0f, 1.0f);

bool dia = true;
static bool ciclo = false;




void getResolution()
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main()
{

	glfwInit();
	
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC - Coyoacan 3D", NULL, NULL);
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


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	glEnable(GL_DEPTH_TEST);


	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	

	std::string skyboxPath = "resources/skybox/";
	std::string skyboxPathBuf = "resources/skybox/";

	Skybox skyboxes[8];

	vector<std::string> skyboxFaces;
	for (int i = 0; i < 8; i++) {

		skyboxFaces.push_back(skyboxPath.append("Right").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skyboxFaces.push_back(skyboxPath.append("Left").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skyboxFaces.push_back(skyboxPath.append("Top").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skyboxFaces.push_back(skyboxPath.append("Bottom").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skyboxFaces.push_back(skyboxPath.append("Front").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skyboxFaces.push_back(skyboxPath.append("Back").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skyboxes[i].setFaces(skyboxFaces);
		skyboxFaces.clear();
	}
	

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	skyboxShader.use();
	skyboxShader.setInt("skyboxNoche", 0);

	//Carga de modelos

	Model pisoDef("resources/objects/pisoDef/pisoDef.obj");
	Model kiosco("resources/objects/Kiosko/Kiosco.obj");

	//Variables para el ciclo de dia/noche
	bool f1 = 1, f2 = 0, f3 = 0;
	float t = 0.0f;
	int hora = 0;
	double paso = 2;

		// LOOP DE RENDERIZADO
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);
		skyboxShader.setInt("skyboxNoche", 0);
		lastFrame = SDL_GetTicks();


		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (f1) {
			if (t > paso) {
				hora++;
				t = 0.0;
			}
			if (hora >= 7) {
				f1 = 0;
				f2 = 1;
			}
		}
		if (f2) {
			if (t > paso) {
				hora--;
				t = 0.0f;
			}
			if (hora <= 0) {
				f2 = 0;
				f3 = 1;
			}
		}
		if (f3) {
			if (t > 4 * paso) {
				f3 = 0;
				f1 = 1;
			}
		}

		t += 0.01;
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		if (hora > 3) {
			staticShader.setVec3("dirLight.ambient", glm::vec3(0.33f, 0.33f, 0.33f));
			staticShader.setVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
			staticShader.setVec3("dirLight.specular", glm::vec3(0, 0, 0));
		}
		else if (hora <=3 && hora >0){
			staticShader.setVec3("dirLight.ambient", glm::vec3(0.001f, 0.001f, 0.001f));
			staticShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
			staticShader.setVec3("dirLight.specular", glm::vec3(0.4f, 0.4f, 0.4f));
		}
		else {
			staticShader.setVec3("dirLight.ambient", glm::vec3(0.001f, 0.001f, 0.001f));
			staticShader.setVec3("dirLight.diffuse", glm::vec3(0.1f, 0.1f, 0.1f));
			staticShader.setVec3("dirLight.specular", glm::vec3(0.1f, 0.1f, 0.1f));

		}


		//Luz Día y noche
		if (dia == true) {
			staticShader.setVec3("pointLight[0].position", glm::vec3(-1000.0f, -1000.0f, -1000.0f));
		}
		else {
			staticShader.setVec3("pointLight[0].position", glm::vec3(-1.0f, 4.2f, -11.2f));
		}

		staticShader.setVec3("pointLight[0].ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		if (dia == true) {
			staticShader.setVec3("pointLight[1].position", glm::vec3(-1000.0f, -1000.0f, -1000.0f));

		}
		else {
			staticShader.setVec3("pointLight[1].position", glm::vec3(-29.5f, 5.4f, -11.0f));
		}
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 0.08f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		if (dia == true) {
			staticShader.setVec3("pointLight[2].position", glm::vec3(-1000.0f, -1000.0f, -1000.0f));
		}
		else {
			staticShader.setVec3("pointLight[2].position", glm::vec3(-1.5f, 4.0f, -35.4001f));
		}
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[2].constant", 0.08f);
		staticShader.setFloat("pointLight[2].linear", 0.009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.032f);

		if (dia == true) {
			staticShader.setVec3("pointLight[3].position", glm::vec3(-1000.0f, -1000.0f, -1000.0f));
		}
		else {
			staticShader.setVec3("pointLight[3].position", glm::vec3(-29.5f, 5.8f, -35.4001f));
		}
		staticShader.setVec3("pointLight[3].ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("pointLight[3].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[3].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[3].constant", 0.08f);
		staticShader.setFloat("pointLight[3].linear", 0.009f);
		staticShader.setFloat("pointLight[3].quadratic", 0.032f);

		if (dia == true) {
			staticShader.setVec3("pointLight[4].position", glm::vec3(-1000.0f, -1000.0f, -1000.0f));
		}
		else {
			staticShader.setVec3("pointLight[4].position", glm::vec3(-1.5f, 3.2f, -59.6002f));
		}
		staticShader.setVec3("pointLight[4].ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("pointLight[4].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[4].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[4].constant", 0.08f);
		staticShader.setFloat("pointLight[4].linear", 0.009f);
		staticShader.setFloat("pointLight[4].quadratic", 0.032f);

		if (dia == true) {
			staticShader.setVec3("pointLight[5].position", glm::vec3(-1000.0f, -1000.0f, -1000.0f));
		}
		else {
			staticShader.setVec3("pointLight[5].position", glm::vec3(-29.5f, 5.4f, -59.8002f));
		}
		staticShader.setVec3("pointLight[5].ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("pointLight[5].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[5].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[5].constant", 0.08f);
		staticShader.setFloat("pointLight[5].linear", 0.009f);
		staticShader.setFloat("pointLight[5].quadratic", 0.032f);

		if (dia == true) {
			staticShader.setVec3("pointLight[6].position", glm::vec3(-1000.0f, -1000.0f, -1000.0f));
		}
		else {
			staticShader.setVec3("pointLight[6].position", glm::vec3(-29.5f, 5.4f, -84.2f));
		}
		staticShader.setVec3("pointLight[6].ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("pointLight[6].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[6].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[6].constant", 0.08f);
		staticShader.setFloat("pointLight[6].linear", 0.009f);
		staticShader.setFloat("pointLight[6].quadratic", 0.032f);

		if (dia == true) {
			staticShader.setVec3("pointLight[7].position", glm::vec3(-1000.0f, -1000.0f, -1000.0f));
		}
		else {
			staticShader.setVec3("pointLight[7].position", glm::vec3(-1.5f, 5.4f, -84.2f));
		}
		staticShader.setVec3("pointLight[7].ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("pointLight[7].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[7].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[7].constant", 0.08f);
		staticShader.setFloat("pointLight[7].linear", 0.009f);
		staticShader.setFloat("pointLight[7].quadratic", 0.032f);




		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		glm::mat4 turi1 = glm::mat4(1.0f);
		glm::mat4 turi2 = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);
		//// Light
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);
		//DECLARACIÓN DE SUELOS
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-20.0f, -1.0f, -30.0f));
		model = glm::scale(model, glm::vec3(1));
		staticShader.setMat4("model", model);
		pisoDef.Draw(staticShader);



		//**** SHADER MODELOS
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 45.0f);
		animShader.setVec3("light.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 0.5f, 0.5f, 0.5f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);


		//DIBUJO SKYBOX
		skyboxShader.use();
		
		skyboxes[hora].Draw(skyboxShader, view, projection, camera);

		//if (dia == true) { //Ciclo día y noche, cambiamos de Skybox de acuerdo a la bandera DÍA
		//	skybox.Draw

		//}
		//else {
		//	skyboxNoche.Draw(skyboxShader, view, projection, camera);
		//}


		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop

		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
		}

		skyboxes[hora].Terminate();

		glfwTerminate();
		return 0;
}

//CONFIGURACIÓN DE ENTRADAS
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime*0.05);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime*0.05);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime*0.05);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime*0.05);
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		if (dia == true) {
			dia = false;
			ciclo = true;
		}
		else {
			dia = true;
			ciclo = true;
		}

	}


}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
//FIELD OF VIEW AJUSTABLE
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}