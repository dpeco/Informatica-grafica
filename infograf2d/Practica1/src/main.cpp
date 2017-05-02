//GLEW
#define GLEW_STATIC
#include <GL\glew.h>
//GLFW
#include <GLFW\glfw3.h>
#include <iostream>
#include <SOIL.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "shader.h"
#include "Camera.h"
#include "Model.h"
#include "Object.h"
#include "material.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

using namespace std;
using namespace glm;
const GLint WIDTH = 800, HEIGHT = 600;
bool WIREFRAME = false;
float rotateX = 0.0f;
float rotateY = 0.0f;
float offsetX = 0.0f;
float offsetY = 0.0f;
float offsetZ = 0.0f;
bool keys[1024];
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseScroll(GLFWwindow* window, double xScroll, double yScroll);
void DoMovement(GLFWwindow* window);
GLfloat mixValue = 0.6f;

//creamos la camara
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
Camera *camera = new Camera (cameraPos, cameraDirection, 0.1f, 60.f);

int modelSelect;
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

int main() {
	modelSelect = 1;
	//initGLFW
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	//comprobar que GLFW estaactivo
	if (!glfwInit())
		exit(EXIT_FAILURE);

	//set GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	//create a window
	window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 1A", nullptr, nullptr);
	if (!window) {
		cout << "Error al crear la ventana" << endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	
	//set GLEW and inicializate
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit()) {
		cout << "Error al iniciar glew" << endl;
		glfwTerminate();
		return NULL;
	}
		
	//set function when callback
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, MouseScroll);
	
	//set windows and viewport
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	//fondo
	//cargamos los shader
	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);

	const GLchar* vertexPath1 = "./src/SimpleVertexShader1.vertexshader";
	const GLchar* vertexPath2 = "./src/SimpleVertexShader2.vertexshader";
	const GLchar* fragmentPath1 = "./src/SimpleFragmentShader1.fragmentshader";
	const GLchar* fragmentPath2 = "./src/SimpleFragmentShader2.fragmentshader";
	const GLchar* fragmentPathPoint = "./src/SimpleFragmentShaderPoint.fragmentshader";
	const GLchar* fragmentPathDir = "./src/SimpleFragmentShaderDirectional.fragmentshader";
	const GLchar* fragmentPathSpot = "./src/SimpleFragmentShaderSpot.fragmentshader";

	Shader myShader1 = Shader::Shader(vertexPath1, fragmentPath1);
	Shader *myShader2 = new Shader(vertexPath2, fragmentPathDir);

	//load random cube
	FigureType type = cube;
	vec3 Vposition1 = vec3(-1.f, 0, 0);
	vec3 Vscale1 = vec3(0.5, 0.5, 0.5);
	vec3 Vrotate1 = vec3(1, 1, 1);
	Object ourCube1(Vscale1, Vrotate1, Vposition1, type);

	vec3 Vposition2 = vec3(3, 0, 0);
	vec3 Vscale2 = vec3(2, 2, 2);
	vec3 Vrotate2 = vec3(1, 1, 1);
	Object ourCube2(Vscale2, Vrotate2, Vposition2, type);

	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 lightDirection = vec3(1.0f, 0.0f, 0.0f);
	vec3 cubeColor = vec3(0.05f, 0.2f, 0.05f);

	vec3 focoDir = vec3(1.0f, 0.0f, 0.0f);
	float phiInner = cos(radians(13.0f));
	float phiOuter = cos(radians(17.0f));

	//material
	Material material ("./src/difuso.png", "./src/especular.png", 32); 

	material.SetMaterial(myShader2);
	//bucle de dibujado
	
	while (!glfwWindowShouldClose(window))
	{
		// Render
		// Clear the colorbuffer
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		myShader1.USE();

		DoMovement(window);
		mat4 view = camera->LookAt();

		///////////////projection
		glm::mat4 projection;
		GLfloat fov = camera->GetFOV();
		projection = glm::perspective(glm::radians(fov), float(screenWidth / screenHeight), 0.1f, 100.0f);
		
		//uniforms
		GLint cubeColor1 = glGetUniformLocation(myShader1.Program, "lightColor");
		glUniform3f(cubeColor1, lightColor.x, lightColor.y, lightColor.z);

		GLint modelLoc1 = glGetUniformLocation(myShader1.Program, "model");
		GLint viewLoc1 = glGetUniformLocation(myShader1.Program, "view");
		GLint projectionLoc1 = glGetUniformLocation(myShader1.Program, "projection");
		
		glUniformMatrix4fv(viewLoc1, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc1, 1, GL_FALSE, glm::value_ptr(projection));
		
		// Draw the loaded model
		mat4 model1;
		vec3 rotateVec = vec3(rotateX, rotateY, 0);
		vec3 moveVec = vec3(Vposition2.x + offsetX, Vposition2.y + offsetY, Vposition2.z + offsetZ);
		ourCube2.Move(moveVec);
		ourCube2.Rotate(rotateVec);
		model1 = ourCube1.GetModelMatrix();
		glUniformMatrix4fv(modelLoc1, 1, GL_FALSE, glm::value_ptr(model1));
		ourCube1.Draw();

		myShader2->USE();

		//uniforms

		//materiales

		material.SetShininess(myShader2);
		material.ActivateTextures();

		//color de la luz + color del cubo
		vec3 cube1position = ourCube1.GetPosition();
		GLint lightLoc1 = glGetUniformLocation(myShader2->Program, "lightColor");
		glUniform3f(lightLoc1, lightColor.x, lightColor.y, lightColor.z);
		
		GLint lightPosLoc = glGetUniformLocation(myShader2->Program, "lightPosition");
		glUniform3f(lightPosLoc, cube1position.x, cube1position.y, cube1position.z);

		GLint lightDirLoc = glGetUniformLocation(myShader2->Program, "lightDirection");
		glUniform3f(lightDirLoc, lightDirection.x, lightDirection.y, lightDirection.z);

		GLint focoDirLoc = glGetUniformLocation(myShader2->Program, "focoDir");
		glUniform3f(focoDirLoc, focoDir.x, focoDir.y, focoDir.z);

		GLint phiInnerLoc = glGetUniformLocation(myShader2->Program, "phiInner");
		glUniform1f(phiInnerLoc, phiInner);

		GLint phiOuterLoc = glGetUniformLocation(myShader2->Program, "phiOuter");
		glUniform1f(phiOuterLoc, phiOuter);

		GLint cubeLoc2 = glGetUniformLocation(myShader2->Program, "cubeColor");
		glUniform3f(cubeLoc2, cubeColor.x, cubeColor.y, cubeColor.z);
		vec3 cameraPosition = camera->GetPos();
		GLint cameraPosLoc = glGetUniformLocation(myShader2->Program, "cameraPosition");
		glUniform3f(cameraPosLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		GLint modelLoc2 = glGetUniformLocation(myShader2->Program, "model");
		GLint viewLoc2 = glGetUniformLocation(myShader2->Program, "view");
		GLint projectionLoc2 = glGetUniformLocation(myShader2->Program, "projection");

		glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc2, 1, GL_FALSE, glm::value_ptr(projection));
		mat4 model2;
		model2 = ourCube2.GetModelMatrix();
		glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));
		glColor3f(0.2, 0.2, 0.2);
		ourCube2.Draw();

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		camera->MouseMove(window, xpos, ypos);

		//intercambia el framebuffer
		glfwSwapBuffers(window);
		//comprueba que algun disparador se halla activado
		glfwPollEvents();	

	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	exit(EXIT_SUCCESS);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

}

void DoMovement(GLFWwindow* window){
	if (keys[GLFW_KEY_W])
		camera->DoMovement(window, GLFW_KEY_W);
	if (keys[GLFW_KEY_S])
		camera->DoMovement(window, GLFW_KEY_S);
	if (keys[GLFW_KEY_A])
		camera->DoMovement(window, GLFW_KEY_A);
	if (keys[GLFW_KEY_D])
		camera->DoMovement(window, GLFW_KEY_D);
	if (keys[GLFW_KEY_Q])
		camera->DoMovement(window, GLFW_KEY_Q);
	if (keys[GLFW_KEY_E])
		camera->DoMovement(window, GLFW_KEY_E);
	if (keys[GLFW_KEY_DOWN]) {
		offsetY -= 0.05f;
	}
	if (keys[GLFW_KEY_LEFT]) {
		offsetX -= 0.05f;
	}
	if (keys[GLFW_KEY_RIGHT]) {
		offsetX += 0.05f;
	}
	if (keys[GLFW_KEY_UP]) {
		offsetY += 0.05f;
	}
	if (keys[GLFW_KEY_O]) {
		offsetZ -= 0.05f;
	}
	if (keys[GLFW_KEY_L]) {
		offsetZ += 0.05f;
	}

	if (keys[GLFW_KEY_KP_8]) {
		rotateX -= 1.f;
	}
	else if (keys[GLFW_KEY_KP_2]) {
		rotateX += 1.f;
	}
	else if (keys[GLFW_KEY_KP_6]) {
		rotateY += 1.f;
	}
	else if (keys[GLFW_KEY_KP_4]) {
		rotateY -= 1.f;
	}
}

void MouseScroll(GLFWwindow* window, double xScroll, double yScroll) {
	camera->MouseScroll(window, xScroll, yScroll);
}
