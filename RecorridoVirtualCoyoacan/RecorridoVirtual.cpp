//Computación --------------------------* /
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
#include <irrKlang.h>

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
using namespace irrklang;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
GLFWmonitor* monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 0.55f, 0.0f));
float MovementSpeed = 1.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


const float toRadians = 3.14159265f / 180.0f;
const float pi = 3.14159265f;



//Struct para variables de animacion
struct variablesAnim {
	float x = 0.0f, y = 0.0f, z = 0.0f, xIni = 0.0f, yIni = 0.0f, zIni = 0.0f, Rot = 0.0f, rotIni, escala = 1.0f;
	float xp, yp, zp;
};



// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(-15.0f, 10.0f, -20.0f); // Dirección de la luz acomodada de acuerdo a la skybox
glm::vec3 lightDirection(0.0f, -1.0f, 1.0f);

bool dia = false;
static bool ciclo = false;
//-----------------------------coordenadas para posición de elementos-------------------------------
float movx = 0;
float movy = 0;
float movz = 0;
float giro = 0;
float escala = 1;

//------------------------------variable tipo de camara-----------------------------------------------
bool tipoCamara = 1;

//-----------------------------Animación avatar--------------------------------------------------------
//Interpolacion para Avatar
int i_max_stepsAv = 3;
int i_curr_stepsAv = 0;
typedef struct _frameInfo
{
	float rotBI, rotBD, rotPSI, rotPSD, rotPID, rotPII;
	float IncRotBI, IncRotBD, IncRotPSI, IncRotPSD, IncRotPID, IncRotPII;

}FRAMEAv;

#define MAX_FRAMES 100

FRAMEAv KeyFrameAv[MAX_FRAMES];
int FrameIndexAv = 9;
int playIndexAv = 0;

float rotBI, rotBD, rotPSI, rotPSD, rotPID, rotPII;

void resetElementsAvatar(void) //Tecla 0
{
	rotBD = KeyFrameAv[0].rotBD;
	rotBI = KeyFrameAv[0].rotBI;
	rotPSI = KeyFrameAv[0].rotPSI;
	rotPSD = KeyFrameAv[0].rotPSD;
	rotPII = KeyFrameAv[0].rotPII;
	rotPID = KeyFrameAv[0].rotPID;

}

void interpolationAvatar(void)
{
	for (int i = 0; i < FrameIndexAv - 1; i++) {
		KeyFrameAv[i].IncRotBD = (KeyFrameAv[i + 1].rotBD - KeyFrameAv[i].rotBD) / i_max_stepsAv;
		KeyFrameAv[i].IncRotBI = (KeyFrameAv[i + 1].rotBI - KeyFrameAv[i].rotBI) / i_max_stepsAv;
		KeyFrameAv[i].IncRotPSI = (KeyFrameAv[i + 1].rotPSI - KeyFrameAv[i].rotPSI) / i_max_stepsAv;
		KeyFrameAv[i].IncRotPSD = (KeyFrameAv[i + 1].rotPSD - KeyFrameAv[i].rotPSD) / i_max_stepsAv;
		KeyFrameAv[i].IncRotPII = (KeyFrameAv[i + 1].rotPII - KeyFrameAv[i].rotPII) / i_max_stepsAv;
		KeyFrameAv[i].IncRotPID = (KeyFrameAv[i + 1].rotPID - KeyFrameAv[i].rotPID) / i_max_stepsAv;
	}
	KeyFrameAv[FrameIndexAv - 1].IncRotBD = (KeyFrameAv[0].rotBD - KeyFrameAv[FrameIndexAv - 1].rotBD) / i_max_stepsAv;
	KeyFrameAv[FrameIndexAv - 1].IncRotBI = (KeyFrameAv[0].rotBI - KeyFrameAv[FrameIndexAv - 1].rotBI) / i_max_stepsAv;
	KeyFrameAv[FrameIndexAv - 1].IncRotPSI = (KeyFrameAv[0].rotPSI - KeyFrameAv[FrameIndexAv - 1].rotPSI) / i_max_stepsAv;
	KeyFrameAv[FrameIndexAv - 1].IncRotPSD = (KeyFrameAv[0].rotPSD - KeyFrameAv[FrameIndexAv - 1].rotPSD) / i_max_stepsAv;
	KeyFrameAv[FrameIndexAv - 1].IncRotPII = (KeyFrameAv[0].rotPII - KeyFrameAv[FrameIndexAv - 1].rotPII) / i_max_stepsAv;
	KeyFrameAv[FrameIndexAv - 1].IncRotPID = (KeyFrameAv[0].rotPID - KeyFrameAv[FrameIndexAv - 1].rotPID) / i_max_stepsAv;

}


void animateAvatar(bool playAv)
{
	//Movimiento del objeto // barra espaciadora
	if (playAv)
	{

		if (i_curr_stepsAv >= i_max_stepsAv) //end of animation between frames?
		{
			i_curr_stepsAv = 0;
			if (playIndexAv >= FrameIndexAv - 1)	//end of total animation?
			{
				playIndexAv = 0;
				//playAv = false;
			}
			else //Next frame interpolations
			{
				playIndexAv++;
			}
		}
		else
		{
			//Draw animation
			rotBD += KeyFrameAv[playIndexAv].IncRotBD;
			rotBI += KeyFrameAv[playIndexAv].IncRotBI;
			rotPSI += KeyFrameAv[playIndexAv].IncRotPSI;
			rotPSD += KeyFrameAv[playIndexAv].IncRotPSD;
			rotPII += KeyFrameAv[playIndexAv].IncRotPII;
			rotPID += KeyFrameAv[playIndexAv].IncRotPID;
			i_curr_stepsAv++;
		}

	}
}

//--------------------------------desplazamiento chica caminando-------------------------------------
float desx_chica = 0;
float desz_chica = 0;
float desrot_chica = 0;

//------------------------ Inicialización variables turibus------------------------------------------
float rotRuedaTuri = 0.0;
float movxTuri = 0.0;
float rotTuri = 0.0;
float movzTuri = 0.0;
bool adelanteTuri = true;
float rotRuedaTuri2 = 0.0,
		movxTuri2 = 0.0,
		rotTuri2 = 0.0,
		ejezRuedaTuri2=0.0,
		ejeyRuedaTuri2=0.0,
		movzTuri2 = 0.0;
bool	animacionTuri2 = true,
		recorrido1 = true,
		recorrido2 = true,
		recorrido3 = true,
		recorrido4 = true,
		recorrido5 = true,
		recorrido6 = true,
		avanza = true;



void animate(void)
{
	//---------------------------------------------- Animación Turibus----------------------------------
	if ((movzTuri < 100) && (adelanteTuri == true)) {
		if (rotRuedaTuri < 360) {
			rotRuedaTuri += 2.0f;
		}
		else {
			rotRuedaTuri = 0.0;
		}
		movxTuri = 0;
		rotTuri = 90;
		movzTuri += 0.3f;

	}
	else {
		adelanteTuri = false;
	}

	if ((movzTuri > -19) && (adelanteTuri == false)) {
		if (rotRuedaTuri < 360) {
			rotRuedaTuri += 2.0f;
		}
		else {
			rotRuedaTuri = 0.0;
		}
		movxTuri = -5;
		rotTuri = 270;
		movzTuri -= 0.3f;
	}
	else {
		adelanteTuri = true;
	}

	//---------------------------------------------- Animación Turibus 2----------------------------------
	
	
	if (animacionTuri2) {
		if (recorrido1) {
			movzTuri2 += 4.0f;
			rotRuedaTuri2 += 4.0f;
			if (movzTuri2 >= 45.0f) {
				recorrido1 = false;
				recorrido2 = true;
			}
		}
		if (recorrido2) {
			movzTuri2 += 1.0f;
			rotRuedaTuri2 += 1.0f;
			movxTuri2 += 0.250f;
			rotTuri2 = 20.0f;
			if (movxTuri2 > 5.0f){
				recorrido2 = false;
				recorrido3 = true;
			}
		}
		if(recorrido3){
			rotTuri2 = 0.0f;
			movzTuri2 +=1.0f;
			rotRuedaTuri2 +=1.0f;
			if (movzTuri2 > 85.0f) {
				recorrido3 = false;
				recorrido4 = true;
			}
		}
		if (recorrido4) {
			movzTuri2 -= 1.0f;
			rotRuedaTuri2 -= 1.0f;
			if (movzTuri2 < 55.0f) {
				recorrido4 = false;
			}
		}
		
	}
	else {
		animacionTuri2 = false;
	}
	//--------------------------------------Desplazamiento chica-----------------------------------------------

	if (desz_chica > -95 && desx_chica == 0.0) {
		desz_chica -= 0.1f;
		desrot_chica = 180;
	}
	else if (desz_chica < -95 && desx_chica < 45) {
		desx_chica += 0.1f;
		desrot_chica = 90;
	}
	else if (desz_chica < -34 && desx_chica > 45) {
		desz_chica += 0.1f;
		desrot_chica = 0;
	}
	else if (desx_chica < 73 && desz_chica >-34) {
		desx_chica += 0.1f;
		desrot_chica = 90;
	}
	else if (desz_chica < 0 ) {
		desz_chica += 0.1f;
		desrot_chica = 0;
	}
	else {
		desx_chica = 0;
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

	glfwInit();
	ISoundEngine* engine = createIrrKlangDevice(); //Se inicializa el motor de audio
	if (!engine)
	{
		printf("No se pudo iniciar el motor de audio\n");
		return 0;
	}
	engine->play2D("resources/audio/morning.wav", true);
	
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Comp.GráficaeIHC -Recorrido Virtual 3D Coyoacan", NULL, NULL);
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
	

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	skyboxShader.use();
	skyboxShader.setInt("skyboxNoche", 0);

	//Carga de modelos

	Model pisoDef("resources/objects/pisoDef/pisoDef.obj");
	Model banorte("resources/objects/Banorte/Banorte.obj");
	Model casaHC("resources/objects/CasaHC/casaHC.obj");
	Model avenida("resources/objects/Avenida/avenida.obj");
	Model tienda("resources/objects/Tienda/tienda.obj");
	Model tienda2("resources/objects/Tienda2/tienda.obj");
	Model tienda3("resources/objects/Tienda3/tienda.obj");
	Model tienda4("resources/objects/Tienda4/tienda.obj");
	Model tienda5("resources/objects/Tienda5/tienda.obj");
	Model tienda6("resources/objects/Tienda6/tienda.obj");
	Model tienda7("resources/objects/Tienda7/tienda.obj");
	Model tienda8("resources/objects/Tienda8/tienda.obj");
	Model iglesia2("resources/objects/iglesiaV2/iglesiav2.obj");
	Model muro("resources/objects/muroIglesia/muro.obj");
	Model tope("resources/objects/topes/tope.obj");
	Model arbolMaceta("resources/objects/arboles/arbol.obj");
	Model tel("resources/objects/telefonos/telPublico.obj");
	Model posteLuz("resources/objects/posteLuz/posLuz.obj");
	Model banca("resources/objects/banca/banca.obj");
	Model areasVerdes("resources/objects/areasVerdes/areasVerdes.obj");
	Model basura("resources/objects/basura/basura.obj");
	Model Kiosco("resources/objects/Kiosco/Kiosco.obj");
	Model miguel("resources/objects/Miguel/miguel.obj");
	Model salida("resources/objects/salida1/salida1.obj");
	Model turibus("resources/objects/turibus/turibus.obj");
	Model rueda("resources/objects/turibus/rueda.obj");
	Model avionRC("resources/objects/AvionRC/Airplane.obj");
	Model cocheRC("resources/objects/CocheRC/CocheRC.obj");
	Model chicoRC("resources/objects/ChicoCRC/Guy1.obj");
	Model ellie("resources/objects/Ellie/Ellie.obj");
	Model avatarTorso("resources/objects/Avatar/TorsoAvatar.obj");
	Model avatarPiernaInf("resources/objects/Avatar/PiernaInf.obj");
	Model avatarPiernaSupDer("resources/objects/Avatar/PiernaSupDer.obj");
	Model avatarPiernaSupIzq("resources/objects/Avatar/PiernaSupIzq.obj");
	Model avatarBrazoDer("resources/objects/Avatar/BrazoDer.obj");
	Model avatarBrazoIzq("resources/objects/Avatar/BrazoIzq.obj");
	Model avatar("resources/objects/Avatar/RobotCompleto.obj");

	ModelAnim chica_camina("resources/objects/Personajes_animados/Personas_caminando/Chica/chica_camina.fbx");
	chica_camina.initShaders(animShader.ID);
	
	//KEYFRAME ANIMACION AVATAR

	KeyFrameAv[0].rotBD = 0.0f;
	KeyFrameAv[0].rotBI = 0.0f;
	KeyFrameAv[0].rotPSI = 0.0f;
	KeyFrameAv[0].rotPSD = 0.0f;
	KeyFrameAv[0].rotPII = 0.0f;
	KeyFrameAv[0].rotPID = 0.0f;

	rotBD = 0.0f;
	rotBI = 0.0f;
	rotPSI = 0.0f;
	rotPSD = 0.0f;
	rotPII = 0.0f;
	rotPID = 0.0f;

	KeyFrameAv[1].rotBD = -20.0f;
	KeyFrameAv[1].rotBI = 20.0f;
	KeyFrameAv[1].rotPSI = 00.0f;
	KeyFrameAv[1].rotPSD = 60.0f;
	KeyFrameAv[1].rotPII = 0.0f;
	KeyFrameAv[1].rotPID = 0.0f;


	KeyFrameAv[2].rotBD = -20.0f;
	KeyFrameAv[2].rotBI = 20.0f;
	KeyFrameAv[2].rotPSI = 00.0f;
	KeyFrameAv[2].rotPSD = 60.0f;
	KeyFrameAv[2].rotPII = 0.0f;
	KeyFrameAv[2].rotPID = 60.0f;


	KeyFrameAv[3].rotBD = -20.0f;
	KeyFrameAv[3].rotBI = 20.0f;
	KeyFrameAv[3].rotPSI = -30.0f;
	KeyFrameAv[3].rotPSD = 30.0f;
	KeyFrameAv[3].rotPII = -30.0f;
	KeyFrameAv[3].rotPID = 30.0f;

	KeyFrameAv[4].rotBD = -20.0f;
	KeyFrameAv[4].rotBI = 20.0f;
	KeyFrameAv[4].rotPSI = -60.0f;
	KeyFrameAv[4].rotPSD = 0.0f;
	KeyFrameAv[4].rotPII = -60.0f;
	KeyFrameAv[4].rotPID = 0.0f;

	KeyFrameAv[5].rotBD = 20.0f;
	KeyFrameAv[5].rotBI = -20.0f;
	KeyFrameAv[5].rotPSI = 60.0f;
	KeyFrameAv[5].rotPSD = 0.0f;
	KeyFrameAv[5].rotPII = 0.0f;
	KeyFrameAv[5].rotPID = 0.0f;

	KeyFrameAv[6].rotBD = 20.0f;
	KeyFrameAv[6].rotBI = -20.0f;
	KeyFrameAv[6].rotPSI = 60.0f;
	KeyFrameAv[6].rotPSD = 0.0f;
	KeyFrameAv[6].rotPII = 60.0f;
	KeyFrameAv[6].rotPID = 0.0f;


	KeyFrameAv[7].rotBD = 20.0f;
	KeyFrameAv[7].rotBI = -20.0f;
	KeyFrameAv[7].rotPSI = 30.0f;
	KeyFrameAv[7].rotPSD = -30.0f;
	KeyFrameAv[7].rotPII = 30.0f;
	KeyFrameAv[7].rotPID = -30.0f;

	KeyFrameAv[8].rotBD = 20.0f;
	KeyFrameAv[8].rotBI = -20.0f;
	KeyFrameAv[8].rotPSI = 0.0f;
	KeyFrameAv[8].rotPSD = -60.0f;
	KeyFrameAv[8].rotPII = 0.0f;
	KeyFrameAv[8].rotPID = -60.0f;


	// Interpolacion de valores para la animaciòn del Avatar

	interpolationAvatar();

	float avatarRot, avatarFrontX, avatarFrontZ;

	//Variables para el ciclo de dia/noche
	bool f1 = 1, f2 = 0, f3 = 0;
	float t = 0.0f;
	int hora = 0;
	double paso = 1;

	//Variables para animaciones
	variablesAnim avionPar = variablesAnim();
	variablesAnim cochePar = variablesAnim();
	variablesAnim audiPar = variablesAnim();

	avionPar.xIni = -28.0f;
	avionPar.yIni = 7.0f;
	avionPar.zIni = -53.0f;
	avionPar.escala = 0.005f;
	avionPar.rotIni = 0;

	cochePar.xIni = -11.5f;
	cochePar.yIni = -1.0f;
	cochePar.zIni = -8.0f;
	cochePar.escala = 0.08f;
	cochePar.rotIni = glm::radians(90.0f);


	float t2 = 0, t3 = 0;
	bool b1 = 1, b2 = 0, b3 = 0, b4 = 0, b5 = 1, b6 = 0, b7 = 1,b8 = 1;
		// LOOP DE RENDERIZADO
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);
		skyboxShader.setInt("skyboxNoche", 0);
		lastFrame = SDL_GetTicks();

		if ((hora == 0 || hora == 1)) {
			b7 = 1;
		}
		else
			b7 = 0;

		if (b7) {
			if (hora == 0) {
				engine->stopAllSounds(); 
				engine->play2D("resources/audio/noche.wav", true);
			}
			else {
				engine->stopAllSounds();
				engine->play2D("resources/audio/morning.wav", true); 
			}
			b7 = 0;
		}
		

		animate();

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

		if (hora > 3)
			dia = true;
		else
			dia = false;
		
		//Luz Día y noche
		if (dia == true) {
			staticShader.setVec3("pointLight[0].position", glm::vec3(-1000.0f, -1000.0f, -1000.0f));
		}
		else {
			staticShader.setVec3("pointLight[0].position", glm::vec3(-1.0f, 6.5f, -11.2f));
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
			staticShader.setVec3("pointLight[1].position", glm::vec3(-29.5f, 6.5f, -11.0f));
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
			staticShader.setVec3("pointLight[2].position", glm::vec3(-1.5f, 6.5f, -35.4001f));
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
			staticShader.setVec3("pointLight[3].position", glm::vec3(-29.5f, 6.5f, -35.4001f));
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
			staticShader.setVec3("pointLight[4].position", glm::vec3(-1.5f, 6.5f, -59.6002f));
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
			staticShader.setVec3("pointLight[5].position", glm::vec3(-29.5f, 6.5f, -59.8002f));
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
			staticShader.setVec3("pointLight[6].position", glm::vec3(-29.5f, 6.5f, -84.2f));
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
			staticShader.setVec3("pointLight[7].position", glm::vec3(-1.5f, 6.5f, -84.2f));
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
		//glm::mat4 turi1 = glm::mat4(1.0f);
		//glm::mat4 turi2 = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		
		//------------------------------------ Light----------------------------------------------------
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

		//-------------------------------------Piso-----------------------------------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-20.0f, -1.0f, -30.0f));
		model = glm::scale(model, glm::vec3(1));
		staticShader.setMat4("model", model);
		pisoDef.Draw(staticShader);

		//------------------------------------Iglesia--------------------------------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-28.0f, 0.0f, -107.999f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		iglesia2.Draw(staticShader);

		//-------------------------------------Salida--------------------------------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(11.0f, -1.8f, 21.6f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		salida.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-69.0f, -4.8f, 21.6f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		salida.Draw(staticShader);

		//----------------------------------Tiendas a la redonda------------------------------------------
		for (int j = 0; j < 2; j++) {
			for (int i = -1; i < 4; i++) {
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(19.0f - (95 * j), -1.0f, -1.2f - (32 * i)));
				model = glm::rotate(model, glm::radians(180.0f * j), glm::vec3(0.0f, 1.0f, 0.0f));
				staticShader.setMat4("model", model);
				tienda.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(19.0f - (95 * j), -1.0f, -5.2f - (32 * i)));
				model = glm::rotate(model, glm::radians(180.0f * j), glm::vec3(0.0f, 1.0f, 0.0f));
				staticShader.setMat4("model", model);
				tienda2.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(19.0f - (95 * j), -1.0f, -9.2f - (32 * i)));
				model = glm::rotate(model, glm::radians(180.0f * j), glm::vec3(0.0f, 1.0f, 0.0f));
				staticShader.setMat4("model", model);
				tienda3.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(19.0f - (95 * j), -1.0f, -13.2f - (32 * i)));
				model = glm::rotate(model, glm::radians(180.0f * j), glm::vec3(0.0f, 1.0f, 0.0f));
				staticShader.setMat4("model", model);
				tienda4.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(19.0f - (95 * j), -1.0f, -17.2f - (32 * i)));
				model = glm::rotate(model, glm::radians(180.0f * j), glm::vec3(0.0f, 1.0f, 0.0f));
				staticShader.setMat4("model", model);
				tienda5.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(19.0f - (95 * j), -1.0f, -21.2f - (32 * i)));
				model = glm::rotate(model, glm::radians(180.0f * j), glm::vec3(0.0f, 1.0f, 0.0f));
				staticShader.setMat4("model", model);
				tienda6.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(19.0f - (95 * j), -1.0f, -25.2f - (32 * i)));
				model = glm::rotate(model, glm::radians(180.0f * j), glm::vec3(0.0f, 1.0f, 0.0f));
				staticShader.setMat4("model", model);
				tienda7.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(19.0f - (95 * j), -1.0f, -29.2f - (32 * i)));
				model = glm::rotate(model, glm::radians(180.0f * j), glm::vec3(0.0f, 1.0f, 0.0f));
				staticShader.setMat4("model", model);
				tienda8.Draw(staticShader);
			}


		}

		//---------------------------------------Banorte---------------------------------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, -1.5f, 4));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		banorte.Draw(staticShader);

		//---------------------------Casa de Hernán Cortéz,biblioteca I. Ramírez---------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-15.5f, -0.8f, 1.2f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		casaHC.Draw(staticShader);

		//----------------------------------------Avenidas-------------------------------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(8.5f, -0.95f, 0.4f));
		staticShader.setMat4("model", model);
		avenida.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-65.0f, -0.95f, 0.4f));
		staticShader.setMat4("model", model);
		avenida.Draw(staticShader);

		//-------------------------------Muro perimetral de la catedral--------------------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-27.5f, -1.2f, -104.4f));
		model = glm::scale(model, glm::vec3(0.78));
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		muro.Draw(staticShader);

		//-----------------------------------------Topes---------------------------------------------------
		for (int j = 0; j < 2; j++) {
			for (int i = 0; i < 35; i++) {
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(2.5f + (-61 * j), -1.0f, -0.6f - (i * 2.4f)));
				staticShader.setMat4("model", model);
				tope.Draw(staticShader);
			}

		}

		//---------------------------------------Árbol----------------------------------------------------

		for (int k = 0; k < 4; k++) {
			for (int j = 0; j < 2; j++) { //Patrón de dibujo por cuadrante
				for (int i = 0; i < 2; i++) {
					//-------------------------Jardineras-------------------------------------------------
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(-11.5f + i * 4 - (j * 28), -1.0f, -12.4f - (24 * k)));
					model = glm::scale(model, glm::vec3(0.4));
					staticShader.setMat4("model", model);
					arbolMaceta.Draw(staticShader);
				}


				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-19.0f - (j * 28), -1.0f, -12.2f - (24 * k)));
				model = glm::scale(model, glm::vec3(0.4));
				staticShader.setMat4("model", model);
				arbolMaceta.Draw(staticShader);


				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-3.5f - (j * 28), -0.8f, -20.6f - (24 * k)));
				staticShader.setMat4("model", model);
				areasVerdes.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-15.0f - (j * 28), -0.8f, -20.6f - (24 * k)));
				staticShader.setMat4("model", model);
				areasVerdes.Draw(staticShader);


				//-----------------------------------------Bancas-----------------------------------------------


				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-7.4f - (j * 28), -1.0f, -11.2f - (24 * k)));
				staticShader.setMat4("model", model);
				banca.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-11.4f - (j * 28), -1.0f, -11.2f - (24 * k)));
				staticShader.setMat4("model", model);
				banca.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-9.4f - (j * 28), -1.0f, -14.4f - (24 * k)));
				staticShader.setMat4("model", model);
				banca.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-16.0f - (j * 28), -1.0f, -14.4f - (24 * k)));
				staticShader.setMat4("model", model);
				banca.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-14.5f - (j * 28), -1.0f, -14.4f - (24 * k)));
				staticShader.setMat4("model", model);
				banca.Draw(staticShader);
				//-----------------------------------Bancas rotadas-----------------------------------------
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-18.0f - (j * 28), -1.0f, -12.0f - (24 * k)));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				staticShader.setMat4("model", model);
				banca.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-12.5f - (j * 28), -1.0f, -12.0f - (24 * k)));
				model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				staticShader.setMat4("model", model);
				banca.Draw(staticShader);

				//-------------------------------Luminarias jardín-------------------------------------------

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-1.5f - (j * 28), -1.2f, -12.4f - (24 * k)));
				staticShader.setMat4("model", model);
				posteLuz.Draw(staticShader);

				//-------------------------------Botes de basura---------------------------------------------
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-1.5f - (j * 28), -1.0f, -11.4f - (24 * k)));
				staticShader.setMat4("model", model);
				basura.Draw(staticShader);

				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-1.5f - (j * 28), -1.0f, -10.8f - (24 * k)));
				staticShader.setMat4("model", model);
				basura.Draw(staticShader);

				//----------------------------------Teléfonos------------------------------------------------

				for (int i = 0; i < 3; i++) {
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(-4.0f - (j * 28) + i, -1.2f, -15.8f - (24 * k)));
					staticShader.setMat4("model", model);
					tel.Draw(staticShader);
				}
			}
		}

		//-------------------------------------------Kiosco--------------------------------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-23.5f, -1.4f, -32.4f));
		staticShader.setMat4("model", model);
		Kiosco.Draw(staticShader);

		//---------------------------------Monumento a Miguel Hidalgo y Costilla-----------------------------

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-38.5f, -1.2f, -77.8f));
		staticShader.setMat4("model", model);
		miguel.Draw(staticShader);

		//------------------------------------------Turibus--------------------------------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-62.0f + movxTuri, -0.8f, -90.0f + movzTuri));
		model = glm::scale(model, glm::vec3(0.7f));
		tmp = model = glm::rotate(model, glm::radians(rotTuri), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		turibus.Draw(staticShader);
		
		//------------------------------------Llantas del turibus--------------------------------------------

		model = glm::translate(tmp, glm::vec3(2.5f, 0.8f, -1.2f));
		model = glm::rotate(model, glm::radians(rotRuedaTuri), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		rueda.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-3.5f, 0.8f, -1.2f));
		model = glm::rotate(model, glm::radians(rotRuedaTuri), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		rueda.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-3.5f, 0.8f, 1.2f));
		model = glm::rotate(model, glm::radians(rotRuedaTuri), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		rueda.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(2.5f, 0.8f, 1.2f));
		model = glm::rotate(model, glm::radians(rotRuedaTuri), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		rueda.Draw(staticShader);

		//--------------------------------------------turibus 2-----------------------------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0f + movxTuri2, -0.8f, -90.0f + movzTuri2));
		model = glm::scale(model, glm::vec3(0.7f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		tmp = model = glm::rotate(model, glm::radians(rotTuri2), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		turibus.Draw(staticShader);
		//-----------------------------------------Llantas Turibus 2
		model = glm::translate(tmp, glm::vec3(2.5f, 0.8f, -1.2f));
		model = glm::rotate(model, glm::radians(rotRuedaTuri2), glm::vec3(0.0f, 0.0f+ejeyRuedaTuri2, 1.0f-ejezRuedaTuri2));
		//model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		rueda.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-3.5f, 0.8f, -1.2f));
		model = glm::rotate(model, glm::radians(rotRuedaTuri2), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		rueda.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-3.5f, 0.8f, 1.2f));
		model = glm::rotate(model, glm::radians(rotRuedaTuri2), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		rueda.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(2.5f, 0.8f, 1.2f));
		model = glm::rotate(model, glm::radians(rotRuedaTuri2), glm::vec3(0.0f, 0.0f+ejeyRuedaTuri2, 1.0f-ejezRuedaTuri2));
		//model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		rueda.Draw(staticShader);

		//---------------------------------------Animaciones------------------------------------------------------------
		
		if (1) {
			if (t2 <= 45 || t2 >= 315) {
				cochePar.x = cochePar.xIni + 2 * cos(glm::radians(t2)) * sqrt(cos(2 * glm::radians(t2)));
				cochePar.z = cochePar.zIni + 2 * sin(glm::radians(t2)) * sqrt(cos(2 * glm::radians(t2)));
				if (t2 == 45)
					t2 = 135;
				if (t2 == 360)
					t2 = 0;
			}
			else if (t2 <= 225) {
				cochePar.x = cochePar.xIni + 2 * cos(glm::radians(t2)) * sqrt(cos(2 * glm::radians(t2)));
				cochePar.z = cochePar.zIni - 2 * sin(glm::radians(t2)) * sqrt(cos(2 * glm::radians(t2)));
				if (t2 == 225)
					t2 = 315;
			}
			if (t2 < 45 || t2 > 315) {
				cochePar.xp = -2 * sin(glm::radians(t2)) * sqrt(cos(2* glm::radians(t2))) - 2 * cos(glm::radians(t2)) * sin(2 * glm::radians(t2)) / (sqrt(cos(2 * glm::radians(t2))));
				cochePar.zp = 2 *  cos(glm::radians(t2)) * sqrt(cos(2* glm::radians(t2))) - 2 * sin(glm::radians(t2)) * sin(2 * glm::radians(t2)) / (sqrt(cos(2 * glm::radians(t2))));
				cochePar.Rot = cochePar.rotIni + atan(cochePar.zp / cochePar.xp);
			}
			else if (t2 < 225) {
				cochePar.xp = -2 * sin(glm::radians(t2)) * sqrt(cos(2 * glm::radians(t2))) - 2 * cos(glm::radians(t2)) * sin(2 * glm::radians(t2)) / (sqrt(cos(2 * glm::radians(t2))));
				cochePar.zp = -2 * cos(glm::radians(t2)) * sqrt(cos(2 * glm::radians(t2))) + 2 * sin(glm::radians(t2)) * sin(2 * glm::radians(t2)) / (sqrt(cos(2 * glm::radians(t2))));
				cochePar.Rot = cochePar.rotIni + atan(cochePar.zp / cochePar.xp);
			}
			if (t2 >= 180 && t2 <= 360) {
				cochePar.Rot += glm::radians(180.0f);
			}

			if (b1 && t3 <= 30) {
				if (t3 == 30) {
					t3 = 210;
					b1 = 0;
					b2 = 1;
				}
			}
			if (b2 && t3 <= 270) {
				if (t3 == 270) {
					t3 = 90;
					b2 = 0;
					b3 = 1;
				}
			}
			if (b3 && t3 <= 150) {
				if (t3 == 150) {
					t3 = 330;
					b4 = 1;
					b3 = 0;
				}
			}
			if (b4 && t3 <= 360) {
				if (t3 == 360) {
					t3 = 0;
					b4 = 0;
					b1 = 1;
				}
			}
			avionPar.y = avionPar.yIni + 5 * cos(glm::radians(t3)) * sqrt(cos(3 * glm::radians(t3)));
			avionPar.z = avionPar.zIni + 5 * sin(glm::radians(t3)) * sqrt(cos(3 * glm::radians(t3)));

			if ((int)t3 % 30 != 0) {
				avionPar.yp = -5 * sin(glm::radians(t3)) * sqrt(cos(3 * glm::radians(t3))) - (15 / 2) * (cos(glm::radians(t3)) * sin(3 * glm::radians(t3))) / (sqrt(cos(3 * glm::radians(t3))));
				avionPar.zp = 5 * cos( glm::radians(t3)) * sqrt(cos(3 * glm::radians(t3))) - (15 / 2) * (sin(glm::radians(t3)) * sin(3 * glm::radians(t3))) / (sqrt(cos(3 * glm::radians(t3))));
				if (avionPar.zp != 0)
					avionPar.Rot = avionPar.rotIni + atan(avionPar.yp / avionPar.zp);
				if ((t3 >= 114.47 && t3 <= 150) || (t3 >= 330 && t3 <= 339.59) || (t3 >= 21 && t3 <= 30) || (t3 >= 210 && t3 <= 245.51)) {
					avionPar.Rot -= glm::radians(180.0f);
				}
			}
			t3 += 1;
			t2 += 1;

		}
		else {

			t2 = 0;
			t3 = 0;
		}

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(avionPar.xIni, avionPar.y, avionPar.z));
		model = glm::scale(model, glm::vec3(avionPar.escala));
		model = glm::rotate(model, -avionPar.Rot, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		avionRC.Draw(staticShader);
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-28.0f, -1.0f, -48.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6));
		staticShader.setMat4("model", model);
		ellie.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-11.5f, -1.0f, -10.5f));
		model = glm::scale(model, glm::vec3(5.3f));
		staticShader.setMat4("model", model);
		chicoRC.Draw(staticShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(cochePar.x, cochePar.yIni, cochePar.z));
		model = glm::scale(model, glm::vec3(0.08f));
		model = glm::rotate(model, -cochePar.Rot, glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cocheRC.Draw(staticShader);


		//--------------------------------------Avatar--------------------------------------------------

		glm::mat4 modelAuxAv = glm::mat4(1.0);
		model = glm::mat4(1.0);
		if (tipoCamara)
			model = glm::translate(model, camera.Position - glm::vec3(0.5 * camera.Front.x, 0.0f, 0.5 * camera.Front.z));
		else
			model = glm::translate(model, camera.Position + glm::vec3(2 * camera.Front.x, 0.0f, 2 * camera.Front.z));

		model = glm::scale(model, glm::vec3(0.025f, 0.025f, 0.025f));
		avatarFrontX = camera.Front.x;
		avatarFrontZ = camera.Front.z;
		if (avatarFrontZ != 0)
			avatarRot = atan(avatarFrontX / avatarFrontZ);
		if (avatarFrontZ > 0)
			avatarRot += 180 * toRadians;
		model = glm::rotate(model, avatarRot + 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelAuxAv = model;
		staticShader.setMat4("model", model);
		avatarTorso.Draw(staticShader);


		model = modelAuxAv;
		model = glm::translate(model, glm::vec3(0.0f, -1.4f * (1 / 0.025)/2, 0.25f * (1 / 0.025) / 2));
		model = glm::rotate(model, rotPSD * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		avatarPiernaSupDer.Draw(staticShader);

		model = modelAuxAv;
		model = glm::translate(model, glm::vec3(0.0f, (1 / 0.025) * (-1.4f) / 2, (1 / 0.025) * -0.25f / 2));
		model = glm::rotate(model, rotPSI * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		avatarPiernaSupIzq.Draw(staticShader);

		model = modelAuxAv;
		model = glm::translate(model, glm::vec3((1 / 0.025) * (0.15f + 0.58 * sin(rotPSD * toRadians)) / 2, (1 / 0.025) * (-1.95f + 0.58 * (1 - cos(rotPSD * toRadians))) / 2, (1 / 0.025) * 0.3f /2));
		staticShader.setMat4("model", model);
		avatarPiernaInf.Draw(staticShader);

		model = modelAuxAv;
		model = glm::translate(model, glm::vec3((0.15f + 0.58 * sin(rotPSI * toRadians)) * (1 / 0.025) / 2, (-1.95f + 0.58 * (1 - cos(rotPSI * toRadians))) * (1 / 0.025)/2, -0.25f * (1 / 0.025) /2));
		staticShader.setMat4("model", model);
		avatarPiernaInf.Draw(staticShader);

		model = modelAuxAv;
		model = glm::translate(model, glm::vec3(-0.05f * (1 / 0.025) / 2, -0.35f * (1 / 0.025) / 2, 0.25f * (1 / 0.025) / 2));
		model = glm::rotate(model, rotBD * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		avatarBrazoDer.Draw(staticShader);

		model = modelAuxAv;
		model = glm::translate(model, glm::vec3(-0.05f * (1 / 0.025)/2, -0.35f * (1 / 0.025) / 2, -0.25f * (1 / 0.025) / 2));
		model = glm::rotate(model, rotBI * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		avatarBrazoIzq.Draw(staticShader);






		//---------------------------------------Shader para modelos animados------------------------------------------
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

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-71.0f + desx_chica, -1.0f, -5.0f + desz_chica));
		model = glm::rotate(model, glm::radians(desrot_chica), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		chica_camina.Draw(animShader);

		//DIBUJO SKYBOX
		skyboxShader.use();
		
		skyboxes[hora].Draw(skyboxShader, view, projection, camera);

	

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

//Definición de teclas
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, (float)deltaTime * 0.05);
		animateAvatar(1);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime * 0.05);
		animateAvatar(1);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, (float)deltaTime * 0.05);
		animateAvatar(1);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, (float)deltaTime * 0.05);
		animateAvatar(1);
	}
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
	//-------------------------------------animación Turibus2------------------------------------------------
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		animacionTuri2 = true;
		recorrido1 = true;
		movxTuri2 = 0.0f;
		movzTuri2 = 0.0f;
	}
	//-------------Imprime las coordenadas en pantalla para saber donde va el elemento-----------------------
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { 
		std::cout << "COORDENADAS EN X, Y,  Z " << movx << "f," << movy << "f," << movz << "f" << std::endl;
		std::cout << "ROTACION: " << giro << std::endl;
		std::cout << "ESCALA: " << escala << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		tipoCamara = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		tipoCamara = 0;
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