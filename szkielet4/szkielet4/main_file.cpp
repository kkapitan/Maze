#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <stdio.h>
#include "tga.h"
#include "shaderprogram.h"
#include "cube.h"
#include "teapot.h"

#include "maze.h"

//Macierze
glm::mat4  matP;//rzutowania
glm::mat4  matV;//widoku
glm::mat4  matM;//modelu

//Wektory i punkty
glm::vec3 m_eye = glm::vec3(0.0f, 0.0f, 7.0f);  //Punkt w którym znajduje siê obserwator
glm::vec3 m_center = glm::vec3(0.0f, 0.0f, 0.0f); //Punkt na który patrzy obserwator
glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f); //Wektor od czubka g³owy obserwatora w górê

//Ustawienia okna i rzutowania
int windowPositionX = 100;
int windowPositionY = 100;
int windowWidth = 400;
int windowHeight = 400;
float cameraAngle = 45.0f;

//kontrola myszki; poprzednie wartosci x, y; zmienne przycisków
int mouse_x_prev = 200;
int mouse_y_prev = 200;
int left_button = 0;
int right_button = 0;
int middle_button = 0;
float mouse_speed = 0.3f;

//Zmienne do animacji
float speed = 0; //120 stopni/s
int lastTime = 0;
float angle = 0;
float angle2 = 4 * atan(1);

//Uchwyty na shadery
ShaderProgram *shaderProgram; //WskaŸnik na obiekt reprezentuj¹cy program cieniuj¹cy.

//Uchwyty na VAO i bufory wierzcho³ków
GLuint vao;
GLuint bufVertices; //Uchwyt na bufor VBO przechowuj¹cy tablicê wspó³rzêdnych wierzcho³ków
GLuint bufColors;  //Uchwyt na bufor VBO przechowuj¹cy tablicê kolorów
GLuint bufNormals; //Uchwyt na bufor VBO przechowuj¹cy tablickê wektorów normalnych

//"Model" który rysujemy. Dane wskazywane przez poni¿sze wskaŸniki i o zadanej liczbie wierzcho³ków s¹ póŸniej wysowane przez program.
//W programie s¹ dwa modele, z których jeden mo¿na wybraæ komentuj¹c/odkomentowuj¹c jeden z poni¿szych fragmentów.

//Kostka

float *vertices = cubeVertices;
float *colors = cubeColors;
float *normals = cubeNormals;
int vertexCount = cubeVertexCount;

//Czajnik
/*float *vertices=teapotVertices;
float *colors=teapotColors;
float *normals=teapotNormals;
int vertexCount=teapotVertexCount;*/


//Procedura rysuj¹ca jakiœ obiekt. Ustawia odpowiednie parametry dla vertex shadera i rysuje.
void drawObject() {
	//W³¹czenie programu cieniuj¹cego, który ma zostaæ u¿yty do rysowania
	//W tym programie wystarczy³oby wywo³aæ to raz, w setupShaders, ale chodzi o pokazanie, 
	//¿e mozna zmieniaæ program cieniuj¹cy podczas rysowania jednej sceny
	shaderProgram->use();

	//Przeka¿ do shadera macierze P,V i M.
	//W linijkach poni¿ej, polecenie:
	//  shaderProgram->getUniformLocation("P") 
	//pobiera numer slotu odpowiadaj¹cego zmiennej jednorodnej o podanej nazwie
	//UWAGA! "P" w powy¿szym poleceniu odpowiada deklaracji "uniform mat4 P;" w vertex shaderze, 
	//a matP w glm::value_ptr(matP) odpowiada deklaracji  "glm::mat4 matP;" TYM pliku.
	//Ca³a poni¿sza linijka przekazuje do zmiennej jednorodnej P w vertex shaderze dane ze zmiennej matP
	//zadeklarowanej globalnie w tym pliku. 
	//Pozosta³e polecenia dzia³aj¹ podobnie.
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"), 1, false, glm::value_ptr(matP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"), 1, false, glm::value_ptr(matV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"), 1, false, glm::value_ptr(matM));

	//Uaktywnienie VAO i tym samym uaktywnienie predefiniowanych w tym VAO powi¹zañ slotów atrybutów z tablicami z danymi
	glBindVertexArray(vao);

	//Narysowanie obiektu
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);


	//glDrawArrays(GL_QUADS, 0, 4);

	//Posprz¹tanie po sobie (niekonieczne w sumie je¿eli korzystamy z VAO dla ka¿dego rysowanego obiektu)
	glBindVertexArray(0);
}

void Movement(unsigned char key, int x, int y) //Prymitywne poruszanie siê 
{
	//Obrót zrealizowany jest jako rotacja wektora od oczu obserwatora do punktu na który patrzy wokó³ wektora wskazuj¹cego 'w górê' od obserwatora, 
	//a nastêpnie wyliczenie nowego punktu skupienia wzroku obserwatora jako przesuniêcie punktu oczu o zrotowany wczeœniej wektor.

	//Przesuniêcie to translacja punktu po³o¿enia obserwatora i punktu skupienia jego wzroku o znormalizowany wektor od oczu do obserwowanego punktu
	//(w taki sposób modu³ tego wektora nie ulega zmianie).

	glm::vec4 temp;
	glm::vec3 temp2;

	if (key == 'a')
	{
		temp = glm::rotate(glm::mat4(1.0f), 3.0f, m_up)*glm::vec4(m_center - m_eye, 0);
		m_center = m_eye + glm::vec3(temp);
	}
	else if (key == 'd')
	{
		temp = glm::rotate(glm::mat4(1.0f), -3.0f, m_up)*glm::vec4(m_center - m_eye, 0);
		m_center = m_eye + glm::vec3(temp);
	}
	else if (key == 'w')
	{
		temp2 = glm::normalize(m_center - m_eye);
		m_eye += temp2;
		m_center += temp2;
	}
	else if (key == 's')
	{
		temp2 = glm::normalize(m_center - m_eye);
		m_eye -= temp2;
		m_center -= temp2;
	}
}

void MouseButtons(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		left_button = 1;
	else left_button = 0;

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		right_button = 1;
	else right_button = 0;

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
		middle_button = 1;
	else middle_button = 0;

}

void MouseActiveMotion(int mouse_x, int mouse_y)
{
	if (right_button)
	{
		int relx = mouse_x - mouse_x_prev;
		glm::vec4 temp;
		temp = glm::rotate(glm::mat4(1.0f), relx*mouse_speed, m_up)*glm::vec4(m_center - m_eye, 0);
		m_center = m_eye + glm::vec3(temp);

		int rely = mouse_y - mouse_y_prev;
		glm::vec4 temp2;
		glm::vec4 temp3;
		glm::vec3 look = glm::normalize(m_center - m_eye);
		glm::vec3 vert_rot = glm::cross(m_up, look);
		//cout << vert_rot.x << " " << vert_rot.y << " " << vert_rot.z << " = " << m_up.x << " " << m_up.y << " " << m_up.z << " X " << look.x << " " << look.y << " " << look.z << endl;
		temp2 = glm::rotate(glm::mat4(1.0f), rely*mouse_speed, vert_rot)*glm::vec4(m_center - m_eye, 0);
		temp3 = glm::rotate(glm::mat4(1.0f), rely*mouse_speed, vert_rot)*glm::vec4(m_up, 0);
		m_center = m_eye + glm::vec3(temp2);
		//m_up = glm::vec3(temp3);

	}

	mouse_x_prev = mouse_x;
	mouse_y_prev = mouse_y;
}

void MouseMotion(int mouse_x, int mouse_y)
{

	mouse_x_prev = mouse_x;
	mouse_y_prev = mouse_y;
}

//Procedura rysuj¹ca
void displayFrame() {
	//Wyczyœæ bufor kolorów i bufor g³êbokoœci
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Wylicz macierz rzutowania
	matP = glm::perspective(cameraAngle, (float)windowWidth / (float)windowHeight, 1.0f, 100.0f);

	//Wylicz macierz widoku
	matV = glm::lookAt(m_eye, m_center, m_up);

	//Wylicz macierz modelu
	matM = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.5, 1, 0));


	//Narysuj obiekt
	drawObject();

	//Tylny bufor na przedni
	glutSwapBuffers();
}

GLuint makeBuffer(void *data, int vertexCount, int vertexSize) {
	GLuint handle;

	glGenBuffers(1, &handle);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), który bêdzie zawiera³ tablicê danych
	glBindBuffer(GL_ARRAY_BUFFER, handle);  //Uaktywnij wygenerowany uchwyt VBO 
	glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, data, GL_STATIC_DRAW);//Wgraj tablicê do VBO

	return handle;
}

//Procedura tworz¹ca bufory VBO zawieraj¹ce dane z tablic opisuj¹cych rysowany obiekt.
void setupVBO() {
	bufVertices = makeBuffer(vertices, vertexCount, sizeof(float)* 4); //Wspó³rzêdne wierzcho³ków
	bufColors = makeBuffer(colors, vertexCount, sizeof(float)* 4);//Kolory wierzcho³ków
	bufNormals = makeBuffer(normals, vertexCount, sizeof(float)* 4);//Wektory normalne wierzcho³ków
}

void assignVBOtoAttribute(char* attributeName, GLuint bufVBO, int variableSize) {
	GLuint location = shaderProgram->getAttribLocation(attributeName); //Pobierz numery slotów dla atrybutu
	glBindBuffer(GL_ARRAY_BUFFER, bufVBO);  //Uaktywnij uchwyt VBO 
	glEnableVertexAttribArray(location); //W³¹cz u¿ywanie atrybutu o numerze slotu zapisanym w zmiennej location
	glVertexAttribPointer(location, variableSize, GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu location maj¹ byæ brane z aktywnego VBO
}

//Procedura tworz¹ca VAO - "obiekt" OpenGL wi¹¿¹cy numery slotów atrybutów z buforami VBO
void setupVAO() {
	//Wygeneruj uchwyt na VAO i zapisz go do zmiennej globalnej
	glGenVertexArrays(1, &vao);

	//Uaktywnij nowo utworzony VAO
	glBindVertexArray(vao);

	assignVBOtoAttribute("vertex", bufVertices, 4); //"vertex" odnosi siê do deklaracji "in vec4 vertex;" w vertex shaderze
	assignVBOtoAttribute("color", bufColors, 4); //"color" odnosi siê do deklaracji "in vec4 color;" w vertex shaderze
	assignVBOtoAttribute("normal", bufNormals, 4); //"normal" odnosi siê do deklaracji "in vec4 normal;" w vertex shaderze

	glBindVertexArray(0);
}

//Procedura uruchamiana okresowo. Robi animacjê.
void nextFrame(void) {
	int actTime = glutGet(GLUT_ELAPSED_TIME);
	int interval = actTime - lastTime;
	lastTime = actTime;
	angle += speed*interval / 1000.0;
	if (angle>360) angle -= 360;
	glutPostRedisplay();
}

//Procedura wywo³ywana przy zmianie rozmiaru okna
void changeSize(int w, int h) {
	//Ustawienie wymiarow przestrzeni okna
	glViewport(0, 0, w, h);
	//Zapamiêtanie nowych wymiarów okna dla poprawnego wyliczania macierzy rzutowania
	windowWidth = w;
	windowHeight = h;
}

//Procedura inicjuj¹ca biblotekê glut
void initGLUT(int *argc, char** argv) {
	glutInit(argc, argv); //Zainicjuj bibliotekê GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //Alokuj bufory kolorów (podwójne buforowanie) i bufor kolorów

	glutInitWindowPosition(windowPositionX, windowPositionY); //Wska¿ pocz¹tkow¹ pozycjê okna
	glutInitWindowSize(windowWidth, windowHeight); //Wska¿ pocz¹tkowy rozmiar okna
	glutCreateWindow("OpenGL 3.3"); //Utwórz okno i nadaj mu tytu³

	glutReshapeFunc(changeSize); //Zarejestruj procedurê changeSize jako procedurê obs³uguj¹ca zmianê rozmiaru okna
	glutDisplayFunc(displayFrame); //Zarejestruj procedurê displayFrame jako procedurê obs³uguj¹ca odœwierzanie okna
	glutIdleFunc(nextFrame); //Zarejestruj procedurê nextFrame jako procedurê wywo³ywan¹ najczêœciêj jak siê da (animacja)
}


//Procedura inicjuj¹ca bibliotekê glew
void initGLEW() {
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "%s\n", glewGetErrorString(err));
		exit(1);
	}

}



//Wczytuje vertex shader i fragment shader i ³¹czy je w program cieniuj¹cy
void setupShaders() {
	shaderProgram = new ShaderProgram("vshader.txt", NULL, "fshader.txt");
}

//procedura inicjuj¹ca ró¿ne sprawy zwi¹zane z rysowaniem w OpenGL
void initOpenGL() {
	setupShaders();
	setupVBO();
	setupVAO();
	glEnable(GL_DEPTH_TEST);
}

//Zwolnij pamiêæ karty graficznej z shaderów i programu cieniuj¹cego
void cleanShaders() {
	delete shaderProgram;
}

void freeVBO() {
	glDeleteBuffers(1, &bufVertices);
	glDeleteBuffers(1, &bufColors);
	glDeleteBuffers(1, &bufNormals);
}

void freeVAO() {
	glDeleteVertexArrays(1, &vao);
}


int main(int argc, char** argv) {

	Maze M = Maze(30, 30);
	M.Show();

	initGLUT(&argc, argv);
	initGLEW();
	initOpenGL();
	glutKeyboardFunc(Movement);
	glutWarpPointer(200, 200);
	glutMotionFunc(MouseActiveMotion);
	glutPassiveMotionFunc(MouseMotion);
	glutMouseFunc(MouseButtons);
	glutMainLoop();

	freeVAO();
	freeVBO();
	cleanShaders();
	return 0;
}
