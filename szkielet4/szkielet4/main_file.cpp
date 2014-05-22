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
glm::vec3 m_eye = glm::vec3(5.0f, 0.0f, 5.0f);  //Punkt w którym znajduje siê obserwator
glm::vec3 m_center = glm::vec3(6.0f, 0.0f, 6.0f); //Punkt na który patrzy obserwator
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

GLuint bufTexCoords;
GLuint tex0;


//Czajnik
/*float *vertices=teapotVertices;
float *colors=teapotColors;
float *normals=teapotNormals;
int vertexCount=teapotVertexCount;*/

int maze_size = 30;
Maze M = Maze(maze_size, maze_size);

bool fly = false;
int p_i = 1, p_j = 1;

GLuint readTexture(char* filename) {
	GLuint tex;
	TGAImg img;
	glActiveTexture(GL_TEXTURE0);
	if (img.Load(filename) == IMG_OK) {
		glGenTextures(1, &tex); //Zainicjuj uchwyt tex 
		glBindTexture(GL_TEXTURE_2D, tex); //Przetwarzaj uchwyt tex 
		if (img.GetBPP() == 24) //Obrazek 24bit 
			glTexImage2D(GL_TEXTURE_2D, 0, 3, img.GetWidth(), img.GetHeight(), 0,
			GL_RGB, GL_UNSIGNED_BYTE, img.GetImg());
		else if (img.GetBPP() == 32) //Obrazek 32bit 
			glTexImage2D(GL_TEXTURE_2D, 0, 4, img.GetWidth(), img.GetHeight(), 0,
			GL_RGBA, GL_UNSIGNED_BYTE, img.GetImg());
		else {
			printf("Nieobs³ugiwany format obrazka w pliku: %s \n", filename);
		}
	}
	else {
		printf("B³¹d przy wczytywaniu pliku: %s\n", filename);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	return tex;
}

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

	glUniform4f(shaderProgram->getUniformLocation("pos"), m_eye.x, m_eye.y, m_eye.z, 1);
	glUniform4f(shaderProgram->getUniformLocation("La"), 0.5, 0.5, 0.5, 1);
	glUniform4f(shaderProgram->getUniformLocation("Ma"), 0.5, 0.5, 0.5, 1);
	glUniform4f(shaderProgram->getUniformLocation("Ls"), 0.0, 1.0, 1.0, 1);
	glUniform4f(shaderProgram->getUniformLocation("Ms"), 0.0, 1.0, 1.0, 1);
	glUniform4f(shaderProgram->getUniformLocation("Ld"), 1.0, 1.0, 1.0, 1);
	glUniform4f(shaderProgram->getUniformLocation("m_eye"), m_eye.x, m_eye.y, m_eye.z, 1);
	glUniform4f(shaderProgram->getUniformLocation("m_center"), m_center.x, m_center.y, m_center.z, 1);
	glUniform4f(shaderProgram->getUniformLocation("m_up"), m_up.x, m_up.y, m_up.z, 1);

	glUniform1i(shaderProgram->getUniformLocation("textureMap0"), 0);

	//Uaktywnienie VAO i tym samym uaktywnienie predefiniowanych w tym VAO powi¹zañ slotów atrybutów z tablicami z danymi
	glBindVertexArray(vao);

	//Narysowanie obiektu
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);


	//glDrawArrays(GL_QUADS, 0, 4);

	//Posprz¹tanie po sobie (niekonieczne w sumie je¿eli korzystamy z VAO dla ka¿dego rysowanego obiektu)
	glBindVertexArray(0);
}

bool Collision(glm::vec3 move,int &i,int &j)
{
	int k1 = ((move + m_eye).x - m_eye.x > 0.1);
	int k2 = ((move + m_eye).z - m_eye.z > 0.1);

	i = int(floor((m_eye + move).x + 2 * k1)) / 4;
	j = int(floor((m_eye + move).z + 2 * k2)) / 4;

	return M[i][j] == '#';
}

void Movement(unsigned char key, int x, int y) 
{
	//Obrót zrealizowany jest jako rotacja wektora od oczu obserwatora do punktu na który patrzy wokó³ wektora wskazuj¹cego 'w górê' od obserwatora, 
	//a nastêpnie wyliczenie nowego punktu skupienia wzroku obserwatora jako przesuniêcie punktu oczu o zrotowany wczeœniej wektor.

	//Przesuniêcie to translacja punktu po³o¿enia obserwatora i punktu skupienia jego wzroku o znormalizowany wektor od oczu do obserwowanego punktu
	//(w taki sposób modu³ tego wektora nie ulega zmianie).
	
	glm::vec4 vrot;
	glm::vec3 move;

	int i, j;


	if (key == 'a' || key == 'd')
	{
		vrot = glm::rotate(glm::mat4(1.0f), key == 'a' ? 3.0f : -3.0f, m_up)*glm::vec4(m_center - m_eye, 0);
		m_center = m_eye + glm::vec3(vrot);
	}
	else if (key == 'w' || key == 's')
	{
		move = glm::normalize(m_center - m_eye);
		move *= 0.5;
		
		if (key == 's')move *= -1;
	
		if (!Collision(move,i,j) || fly)
		{
			m_eye.x += move.x;
			m_center.x += move.x;

			m_eye.z += move.z;
			m_center.z += move.z;
		
			if (fly)
			{
				m_eye.y += move.y;  
				m_center.y += move.y;
			}
			else
			{

				M[i][j] = '$';
				M[p_i][p_j] = '.';
				p_i = i; p_j = j;
			}
		}
	}
	else if (key == 'm')
	{
		M.Show(); 
		printf("%d %d\n", p_i, p_j);
	}
	else if (key == 'f')fly = !fly;
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
		temp = glm::rotate(glm::mat4(1.0f), -relx*mouse_speed, m_up)*glm::vec4(m_center - m_eye, 0);
		m_center = m_eye + glm::vec3(temp);

		int rely = mouse_y - mouse_y_prev;
		glm::vec4 temp2;
		glm::vec4 temp3;
		glm::vec3 look = glm::normalize(m_center - m_eye);
		glm::vec3 vert_rot = glm::cross(m_up, look);
	
		temp2 = glm::rotate(glm::mat4(1.0f), rely*mouse_speed, vert_rot)*glm::vec4(m_center - m_eye, 0);
		temp3 = glm::rotate(glm::mat4(1.0f), rely*mouse_speed, vert_rot)*glm::vec4(m_up, 0);
		m_center = m_eye + glm::vec3(temp2);
	}

	mouse_x_prev = mouse_x;
	mouse_y_prev = mouse_y;
}

void MouseMotion(int mouse_x, int mouse_y)
{

	mouse_x_prev = mouse_x;
	mouse_y_prev = mouse_y;
}

void DrawMaze()
{
	for (int i = 0; i < maze_size; i++)
		for (int j = 0; j < maze_size; j++)
			if (M[i][j] == '#')
				for (int kx = 0; kx < 2; kx++)
					for (int kz = 0; kz < 2; kz++)
					{
						matM = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f*i + kx*2.0f, 0, 4.0f*j + kz*2.0f));
						drawObject();
					}
}

//Procedura rysuj¹ca
void displayFrame() {
	//Wyczyœæ bufor kolorów i bufor g³êbokoœci
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Wylicz macierz rzutowania
	matP = glm::perspective(cameraAngle, (float)windowWidth / (float)windowHeight, 0.5f, 100.0f);

	//Wylicz macierz widoku
	matV = glm::lookAt(m_eye, m_center, m_up);


	//Pod³oga
	matM = glm::scale(glm::mat4(1.0f), glm::vec3(2*maze_size, 1.0f, 2*maze_size));
	matM = glm::translate(matM, glm::vec3(0.984f, -2.0f, 0.984f));
	drawObject();
	
	//Sufit
	/*matM = glm::scale(glm::mat4(1.0f), glm::vec3(2 * maze_size, 1.0f, 2 * maze_size));
	matM = glm::translate(matM, glm::vec3(0.984f, 2.0f, 0.984f));
	drawObject();
	*/
	
	DrawMaze();

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
	bufTexCoords = makeBuffer(cubeTexCoords, cubeVertexCount, sizeof(float)* 2);
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

	assignVBOtoAttribute("texCoord", bufTexCoords, 2);
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
	tex0 = readTexture("metal.tga");
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
