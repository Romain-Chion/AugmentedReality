// Read and display Blender's obj file

#ifdef WIN32
#include <windows.h>
#endif

#include "glut.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <iostream>

using namespace std;

// Hold one object
struct OBJ_FILE{
	float *vertices;	// vertices array
	int *indices;		// indices array
	float *normals;		// normals array
	int *colors;		// colors array
	int Nb_vertex;		// number of vertices in the object
	int Nb_face;		// number of faces in the object
	int Ngone;			// Not use in this program
};


  /********************************/
 /*          PROTOTYPES          */
/********************************/

int readstr(FILE *f,char *string);
int face_count(char *File_name);
void CreatNormal(OBJ_FILE MyObj);
void DisplayFaces(OBJ_FILE MyObj);
OBJ_FILE Extract_Obj_File(OBJ_FILE MyObj,char *File_name);
GLuint LoadTexture(const char * pic);


  /********************************/
 /*      VARIABLES EXTERNES      */
/********************************/
			
//extern OBJ_FILE MyObj;