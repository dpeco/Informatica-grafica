#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "sha"
#include <GL\glew.h>
#include <gtc\matrix_transform.hpp>
#include <glm.hpp>
#include <assimp\types.h>

class Mesh {
	Mesh(vector<Vertex> vert, vector<GLuint> ind, vector<Texture> text);
}