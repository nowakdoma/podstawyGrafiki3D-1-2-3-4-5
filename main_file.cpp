/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"


float speed = 0;
int liczbaSegmentow = 2;

void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_DOWN) speed = -PI;
		if (key == GLFW_KEY_UP) speed = PI;
	}

	if (action == GLFW_RELEASE) {
		speed = 0;
	}
}


void drawCube(GLFWwindow* window, glm::mat4 M, glm::vec3 scale) {

	M = glm::scale(M, scale);
	glUniform4f(spLambert->u("color"), 0.38, 0.15, 1, 1);
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(M));  //Załadowanie macierzy modelu do programu cieniującego
	Models::cube.drawSolid(); //Narysowanie obiektu
}

void drawFinger(GLFWwindow* window, glm::mat4 M, float angle) {

	glm::vec3 fingerSize = glm::vec3(1.0f, 0.25f, 0.5f);

		M = glm::rotate(M, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		M = glm::translate(M, glm::vec3(1.0f, 0.0f, 0.0f));

	drawCube(window, M, fingerSize);

	for (int i = 1; i < liczbaSegmentow; i++) {

		M = glm::translate(M, glm::vec3(1.0f, 0.0f, 0.0f));
		M = glm::rotate(M, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		M = glm::translate(M, glm::vec3(1.0f, 0.0f, 0.0f));

		drawCube(window, M, fingerSize);
	}

}


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
    initShaders();
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, key_callback); // ponowne włączenie obsługi klawiatury
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************	
}


//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 2.0f, -20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 

	spLambert->use();
	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P)); 
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V)); 

	glm::mat4 M = glm::mat4(1.0f);

	drawCube(window, M, glm::vec3(0.5f, 0.25f, 0.5f));

	glm::mat4 M1 = glm::translate(M, glm::vec3(0.5f, 0.0f, 0.0f));
	drawFinger(window, M1 , angle);

	glm::mat4 M2 = glm::rotate(M, PI/2, glm::vec3(0.0f, 1.0f, 0.0f));
	M2 = glm::translate(M2, glm::vec3(0.5f, 0.0f, 0.0f));
	drawFinger(window, M2, angle);

	glm::mat4 M3 = glm::rotate(M, PI, glm::vec3(0.0f, 1.0f, 0.0f));
	M3 = glm::translate(M3, glm::vec3(0.5f, 0.0f, 0.0f));
	drawFinger(window, M3, angle);

	glm::mat4 M4 = glm::rotate(M, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	M4 = glm::translate(M4, glm::vec3(0.5f, 0.0f, 0.0f));
	drawFinger(window, M4, angle);

	glfwSwapBuffers(window);
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	float angle = 0;

	//Główna pętla	
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{		
		angle += speed * glfwGetTime();
		if (angle > 0) angle = 0; //ograniczenie kątów zginania 
		if (angle < -PI/(liczbaSegmentow + 1)) angle = -PI/(liczbaSegmentow + 1); 
		glfwSetTime(0);
		drawScene(window, angle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
