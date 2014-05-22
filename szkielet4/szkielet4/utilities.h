#ifndef utilities_h
#define utilities_h

#include<vector>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "tga.h"
#include <stdio.h>

GLuint readTexture(char* filename);
int read_obj(const char *path,float *out_vertices,float *out_uvs, float *out_normals);


#endif