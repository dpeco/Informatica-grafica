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
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

using namespace std;
using namespace glm;
const GLint WIDTH = 800, HEIGHT = 600;
bool WIREFRAME = false;
float rotateX = 0.0f;
float rotateY = 0.0f;
bool keys[1024];
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseMove(GLFWwindow* window, double xpos, double ypos);
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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//TODO

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

	const GLchar* vertexPath = "./src/SimpleVertexShader.vertexshader";
	const GLchar* fragmentPath = "./src/SimpleFragmentShader.fragmentshader";
	Shader myShader = Shader::Shader(vertexPath, fragmentPath);

	float offset = 0;
	bool turnOffset = true;

	//models
	// Load models
	Model ourModel1("./spider/WusonOBJ.obj");
	Model ourModel2("./spider/spider.obj");
	Model ourModel3("./spider/empty_mat.obj");
	//bucle de dibujado
	
	while (!glfwWindowShouldClose(window))
	{
		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myShader.USE();

		DoMovement(window);
		mat4 view = camera->LookAt();

		///////////////projection
		glm::mat4 projection;
		GLfloat fov = camera->GetFOV();
		projection = glm::perspective(glm::radians(fov), float(screenWidth / screenHeight), 0.1f, 100.0f);
		
		//changing the color 
		GLint variableShader = glGetUniformLocation(myShader.Program, "offset");
		glUniform1f(variableShader, offset);
		
		//uniforms
		GLint modelLoc = glGetUniformLocation(myShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(myShader.Program, "view");
		GLint projectionLoc = glGetUniformLocation(myShader.Program, "projection");
		
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		
		// Load model
		switch (modelSelect){
		case 1:
			ourModel1.Draw(myShader, GL_TRIANGLES);
		break;
		case 2:
			ourModel2.Draw(myShader, GL_TRIANGLES);
			break;
		case 3:
			ourModel3.Draw(myShader, GL_TRIANGLES);
			break;
		default:
			ourModel1.Draw(myShader, GL_TRIANGLES);
		}
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

	if (key == GLFW_KEY_1) {
		modelSelect = 1;
		std::cout << modelSelect;
	}
	if (key == GLFW_KEY_2) {
		modelSelect = 2;
		std::cout << modelSelect;

	}
	if (key == GLFW_KEY_3) {
		modelSelect = 3;
		std::cout << modelSelect;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		WIREFRAME = !WIREFRAME;
	}
	if (key == GLFW_KEY_LEFT) {
		rotateX += 0.1f;
	}
	else if (key == GLFW_KEY_RIGHT) {
		rotateX -= 0.1f;
	}
	//EJERCICIO 2
	if (key == GLFW_KEY_UP) {
		rotateY += 0.1f;
	}
	if (key == GLFW_KEY_DOWN) {
		rotateY -= 0.1f;
	}
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

}

void MouseScroll(GLFWwindow* window, double xScroll, double yScroll) {
	camera->MouseScroll(window, xScroll, yScroll);
}