// Onda.cpp: definisce il punto di ingresso dell'applicazione console.
//

// Onda.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "ShaderMaker.h"

// Include GLEW
#include <GL/glew.h>
#include <GL\freeglut.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;
using namespace std;

extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

float fov = 45.0f;
int larghezza = 800, ww;
int altezza = 800, hh;



GLuint programId;
float angolox = 0.0, angoloy = 0.0, angoloz = 0.0, zcam = 8, angoloL = 0.0;

vector <GLuint> normalIndices;
vector <vec3> normals;
mat4 view, Projection, model, MVP;
GLuint MatrixID, ModelMatrixID, ViewMatrixID, loc;
vec3 lightPos = vec3(0, 4, 0);
GLuint LightID;
GLuint VAO_MODELLO;
typedef struct {
	float x, y, z;
} Point;
typedef struct {
	float r, g, b, a;
} ColorRGBA;
typedef struct {
	GLuint i, j, k;
} Indici;

vector <vec3>  verticesPiano;
vector <ColorRGBA>vertex_colorsPiano;
vector <GLuint> index_verticesPiano;



void crea_piano(vector <vec3> *vertices, vector <GLuint> *index_vertices)
{

	int N = 256;

	int i, j;

	for (i = 0; i <N; i++)
	{
		for (j = 0; j <N; j++)
		{
			vertices->push_back(vec3((float)i / N, (float)i / N, (float)j / N));
		}
	}
	int cont = -1;


	for (i = 0; i <= pow(N, 2) - (N + 1); i++) {


		j = i % (N);

		if (j != N - 1)
		{
			index_vertices->push_back(i);
			index_vertices->push_back(i + 1);
			index_vertices->push_back(i + N + 1);

			index_vertices->push_back(i + N + 1);
			index_vertices->push_back(i);
			index_vertices->push_back(i + N);

		}

	}



}

GLuint crea_VAO_Vector(GLuint VAO, vector <vec3> vertici, vector <GLuint> indici)
{
	GLuint vertexbuffer, colorbuffer, elements_buffer;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertici.size() * sizeof(vec3), &vertici[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // attributo
		3,                 // numero di elementi per vertice,  (x,y,z)
		GL_FLOAT,          // il tipo di ogni elemento
		GL_FALSE,          // non normalizza il vaore
		0,                 // non ci sondati extra tra ogni posizione 
		0                  // offset del primo elemento
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO di tipo indici
	glGenBuffers(1, &elements_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indici.size() * sizeof(GLuint), &indici[0], GL_STATIC_DRAW);

	return VAO;

}

void mouse_rotella(int wheel, int direction, int x, int y)
{
	// Increment/decrement the global pointSize depending on the direction 
	// of rotation of the mouse wheel.
	(direction > 0) ? fov++ : fov--;

	glutPostRedisplay();
}

void drawScene(void)
{
	glUniform1f(loc, glutGet(GLUT_ELAPSED_TIME));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	mat4 View = lookAt(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
	Projection = glm::perspective(radians(fov), (float)(ww) / (float)(hh), 0.1f, 100.0f);
	model = mat4(1.0);
	model = translate(model, vec3(-4.0, 0.0, -2.0));
	model = scale(model, vec3(8.0, 8.0, 8.0));
	//model = rotate(model, radians(90.0f), vec3(1.0, 0.0, 0.0));
	mat4 MV = Projection * View*model;
	MatrixID = glGetUniformLocation(programId, "MV");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, value_ptr(MV));

	glBindVertexArray(VAO_MODELLO);
	glDrawElements(GL_TRIANGLES, index_verticesPiano.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glutSwapBuffers();

}
// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{


	case 'x':
		angolox += 5;
		//printf("angolo %f \n", angolox);
		break;
	case 'y':
		angoloy += 5;
		//printf("angolo %f \n", angoloy);
		break;
	case 'z':
		angoloz += 5;
		//printf("angolo %f \n", angoloz);
		break;
	case 'T':
		zcam += 0.1;
		//printf("zcam %f \n", zcam);
		break;
	case 't':
		zcam -= 0.1;
		//printf("angolo %f \n", zcam);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	ww = w;
	hh = h;
}

GLuint crea_VAO_Puntatori(GLuint VAO, Point* Vertici, int nv, ColorRGBA* Colori_vertici, Indici* Indici_vertici, int ni)
{
	GLuint vertexbuffer, colorbuffer, indices_buffer;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Cominciamo a Costruire i VBO di vertici, colori e indici dei vertici

	//Genero , rendo attivo, riempio ed assegno al layer 0 la geometria della mia primitva
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, nv * sizeof(Point), Vertici, GL_STATIC_DRAW);

	//Adesso carico il VBO dei vertici nel layer 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //Disattiva il buffer precedentemente attivato 


									  //Genero , rendo attivo, riempio ed assegno al layer 1 i colori di ogni vertice della primitiva
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, nv * sizeof(ColorRGBA), Colori_vertici, GL_STATIC_DRAW);
	//Adesso carico il VBO del buffer dei colori nel layer 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //Disattiva il buffer precedentemente attivato 

									  //Genero un VBO per gli indici
	glGenBuffers(1, &indices_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni * sizeof(Indici), Indici_vertici, GL_STATIC_DRAW);
	return VAO;
}
void INIT_VAO(void)
{
	GLenum ErrorCheckValue = glGetError();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	char* vertexShader = (char*)"vertexShader.glsl";
	char* fragmentShader = (char*)"fragmentShader.glsl";

	int N = 2;
	vector <vec3> piano;
	vector <GLuint> indici_piano;
	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);


	crea_piano(&verticesPiano, &index_verticesPiano);

	/*	for (int i = 0; i < piano.size(); i++)
	printf("x %f y %f z %f \n", piano[i].x, piano[i].y, piano[i].z);


	for (int i = 0; i <(N)*(N) ; i++) {
	if (i%(N)==0 && (i!=0))
	{
	}
	else
	{
	indici_piano.push_back(i);
	indici_piano.push_back(i + N + 1);
	indici_piano.push_back(i + N);


	indici_piano.push_back(i + N + 1);
	indici_piano.push_back(i);
	indici_piano.push_back(i + 1);
	}


	}

	int i =N*N;
	indici_piano.push_back(i);
	indici_piano.push_back(i + N + 1);
	indici_piano.push_back(i + N);


	indici_piano.push_back(i + N + 1);
	indici_piano.push_back(i);
	indici_piano.push_back(i + 1);
	for (int i = 0; i < indici_piano.size(); i++)
	printf("indice %d --> %d \n", i,indici_piano[i]);


	*/

	//Creao il VAO
	VAO_MODELLO = crea_VAO_Vector(VAO_MODELLO, verticesPiano, index_verticesPiano);


}

void main(int argc, char **argv)
{

	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(larghezza, altezza);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Visualizzazione OBJ Models");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutMouseWheelFunc(mouse_rotella);
	glutIdleFunc(drawScene);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_VAO();
	loc = glGetUniformLocation(programId, "time");

	glutMainLoop();

}


