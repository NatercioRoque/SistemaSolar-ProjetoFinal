#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef GL_VERSION_1_1
static GLuint texName;
static GLuint sunTexName;     // Textura para o sol
static GLuint mercuryTexName; // Textura para Mercúrio
static GLuint venusTexName;   // Textura para Vênus
static GLuint earthTexName;   // Textura para Terra
static GLuint marsTexName;    // Textura para Marte
static GLuint jupiterTexName; // Textura para Júpiter
static GLuint saturnTexName;  // Textura para Saturno
static GLuint uranusTexName;  // Textura para Urano
static GLuint neptuneTexName; // Textura para Netuno
#endif

// Variáveis da janela
int windowWidth = 800;
int windowHeight = 600;

// Variáveis de posição da câmera
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 50.0f;

// Variáveis de rotação da câmera
float cameraYaw = 180.0f;   // Rotação em torno do eixo Y (esquerda-direita)
float cameraPitch = 0.0f;   // Rotação em torno do eixo X (cima-baixo)

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

// Array de objetos celestes
#define MAX_OBJECTS 10
CelestialObject objects[MAX_OBJECTS];
int objectCount = 0;

// Raios orbitais para cada planeta (distâncias do Sol)
const float orbitalRadii[] = {
   0.0f,   // Sol (no centro)
   5.0f,   // Mercúrio
   7.0f,   // Vênus
   10.0f,  // Terra
   15.0f,  // Marte
   25.0f,  // Júpiter
   35.0f,  // Saturno
   45.0f,  // Urano
   55.0f   // Netuno
};

// Protótipos de funções
void addCelestialObject(float posX, float posY, float posZ, 
                       float velX, float velY, float velZ,
                       float mass, float radius, GLuint texture,
                       float r, float g, float b, bool fixed,
                       const char* name);

// Adicionar um objeto celeste ao sistema
void addCelestialObject(float posX, float posY, float posZ, 
                       float velX, float velY, float velZ,
                       float mass, float radius, GLuint texture,
                       float r, float g, float b, bool fixed,
                       const char* name) {
    if (objectCount < MAX_OBJECTS) {
        CelestialObject obj = {
            .posX = posX, .posY = posY, .posZ = posZ,
            .velX = velX, .velY = velY, .velZ = velZ,
            .mass = mass,
            .radius = radius,
            .rotationAngle = 0.0f,
            .rotationSpeed = 1.0f, // velocidade padrão de rotação
            .texture = texture,
            .r = r, .g = g, .b = b,
            .fixed = fixed,
            .name = ""
        };
        strcpy(obj.name, name);
        objects[objectCount++] = obj;
    } else {
        printf("Erro: Número máximo de objetos atingido.\n");
    }
}

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   // Limpar o array de objetos celestes
   objectCount = 0;

   // === Criar objetos celestes (Sol e planetas) ===
   
   // Sol (fixo no centro)
   addCelestialObject(
      0.0f, 0.0f, 0.0f,     // posição
      0.0f, 0.0f, 0.0f,     // velocidade
      1.989e30,             // massa do Sol em kg
      3.0f,                 // raio visual
      sunTexName,           // textura
      1.0f, 1.0f, 0.0f,     // cor amarela (backup)
      true,                 // fixo, não se move
      "Sol"                // nome do objeto
   );
   // Mercúrio
   addCelestialObject(
      orbitalRadii[1], 0.0f, 0.0f,     // posição (usar raio orbital global)
      0.0f, 0.0f, 0.0f,     // velocidade (será calculada na física)
      3.3011e23,            // massa em kg
      0.4f,                 // raio visual
      mercuryTexName,       // textura
      0.7f, 0.7f, 0.7f,     // cor cinza (backup)
      true,                 // fixo inicialmente
      "Mercurio"          // nome do objeto
   );
   // Vênus
   addCelestialObject(
      orbitalRadii[2], 0.0f, 0.0f,     // posição (usar raio orbital global)
      0.0f, 0.0f, 0.0f,     // velocidade (será calculada na física)
      4.8675e24,            // massa em kg
      0.9f,                 // raio visual
      venusTexName,         // textura
      0.9f, 0.7f, 0.0f,     // cor laranja-amarelada (backup)
      true,                 // fixo inicialmente
      "Venus"            // nome do objeto
   );
   // Terra
   addCelestialObject(
      orbitalRadii[3], 0.0f, 0.0f,    // posição (usar raio orbital global) 
      0.0f, 0.0f, 0.0f,     // velocidade (será calculada na física)
      5.972e24,             // massa da Terra em kg
      1.0f,                 // raio
      earthTexName,         // textura
      0.0f, 0.5f, 1.0f,     // cor azul (backup)
      true,                 // fixo inicialmente
      "Terra"            // nome do objeto
   );
   // Marte
   addCelestialObject(
      orbitalRadii[4], 0.0f, 0.0f,    // posição (usar raio orbital global)
      0.0f, 0.0f, 0.0f,     // velocidade (será calculada na física)
      6.4171e23,            // massa em kg
      0.5f,                 // raio visual
      marsTexName,          // textura
      1.0f, 0.3f, 0.0f,     // cor vermelha (backup)
      true,                 // fixo inicialmente
      "Marte"            // nome do objeto
   );
   // Júpiter
   addCelestialObject(
      orbitalRadii[5], 0.0f, 0.0f,    // posição (usar raio orbital global)
      0.0f, 0.0f, 0.0f,     // velocidade (será calculada na física)
      1.8982e27,            // massa em kg
      2.0f,                 // raio visual
      jupiterTexName,       // textura
      0.9f, 0.7f, 0.5f,     // cor bege (backup)
      true,                 // fixo inicialmente
      "Jupiter"            // nome do objeto
   );
   // Saturno
   addCelestialObject(
      orbitalRadii[6], 0.0f, 0.0f,    // posição (usar raio orbital global)
      0.0f, 0.0f, 0.0f,     // velocidade (será calculada na física)
      5.6834e26,            // massa em kg
      1.8f,                 // raio visual
      saturnTexName,        // textura
      0.9f, 0.8f, 0.5f,     // cor amarelada (backup)
      true,                 // fixo inicialmente
      "Saturno"            // nome do objeto
   );
   // Urano
   addCelestialObject(
      orbitalRadii[7], 0.0f, 0.0f,    // posição (usar raio orbital global)
      0.0f, 0.0f, 0.0f,     // velocidade (será calculada na física)
      8.6810e25,            // massa em kg
      1.5f,                 // raio visual
      uranusTexName,        // textura
      0.5f, 0.8f, 0.9f,     // cor azul-esverdeada (backup)
      true,                 // fixo inicialmente
      "Urano"            // nome do objeto
   );
   // Netuno
   addCelestialObject(
      orbitalRadii[8], 0.0f, 0.0f,    // posição (usar raio orbital global)
      0.0f, 0.0f, 0.0f,     // velocidade (será calculada na física)
      1.02413e26,           // massa em kg
      1.4f,                 // raio visual
      neptuneTexName,       // textura
      0.0f, 0.0f, 0.8f,     // cor azul escuro (backup)
      true,                 // fixo inicialmente
      "Netuno"            // nome do objeto
   );

}

void display(void){
   // Limpa o buffer de cores e profundidade
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   // Desenha os objetos celestes
   for (int i = 0; i < objectCount; i++) {
      glPushMatrix();

      // Posiciona o objeto
      glTranslatef(objects[i].posX, objects[i].posY, objects[i].posZ);

      // Definir cor (será usada como fator de multiplicação para a textura)
      glColor3f(objects[i].r, objects[i].g, objects[i].b);

      // Criar uma esfera para o objeto
      GLUquadric* quadric = gluNewQuadric();
      gluQuadricTexture(quadric, GL_TRUE);
      gluQuadricNormals(quadric, GLU_SMOOTH);

      // Desenhar a esfera
      gluSphere(quadric, objects[i].radius, 32, 32);
      gluDeleteQuadric(quadric);

      glPopMatrix();
   }

   glutSwapBuffers();
}

void reshape(int w, int h){
   glViewport(0, 0, (GLsizei)w, (GLsizei)h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 1000.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
    
   // Aplicar rotações primeiro (yaw em torno do eixo Y, pitch em torno do eixo X)
   glRotatef(cameraPitch, 1.0f, 0.0f, 0.0f);
   glRotatef(cameraYaw, 0.0f, 1.0f, 0.0f);
   
   // Depois aplicar translação
   glTranslatef(cameraX, cameraY, cameraZ);
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