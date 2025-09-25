#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Definição do tipo de objeto celeste
typedef struct {
    float posX, posY, posZ;     // Posição
    float velX, velY, velZ;     // Velocidade
    float mass;                // Massa em kg
    float radius;              // Raio em unidades GL
    float rotationAngle;       // Ângulo de rotação em torno do próprio eixo
    float rotationSpeed;       // Velocidade de rotação
    GLuint texture;            // Textura do objeto
    float r, g, b;             // Cor do objeto (para backup se não tiver textura)
    bool fixed;                // Se o objeto está fixo no espaço (não se move pela gravidade)
    char name[50];             // Nome do objeto celeste
} CelestialObject;

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);

}

void display(void){
   // Limpa o buffer de cores e profundidade
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   glutSwapBuffers();
}

void reshape(){

}

void keyboard(unsigned char key, int x, int y){
   switch (key) {
      case 27:
         exit(0);
         break;
      default:
         break;
   }
}

int main(int argc, char** argv){
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();

   return 0;
}