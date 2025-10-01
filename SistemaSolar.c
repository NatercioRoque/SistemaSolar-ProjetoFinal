#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Define M_PI se não estiver definido
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Criar textura de tabuleiro de xadrez */
#define checkImageWidth 64
#define checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

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
float cameraMove = 0.5;

// Variáveis de rotação da câmera
float cameraYaw = 180.0f;   // Rotação em torno do eixo Y (esquerda-direita)
float cameraPitch = 0.0f;   // Rotação em torno do eixo X (cima-baixo)

//Variáveis para movimentação da câmera
float fwdX, fwdY, fwdZ; 
float rightX, rightY, rightZ;        
float upX, upY, upZ;      

const float pi = 3.14159265358979323846;

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

// Variáveis de física
float timeStep = 0.1f;     // Fator de escala de tempo para ajustar velocidade da simulação
float rotationAngles[MAX_OBJECTS]; // Ângulos de rotação para cada planeta

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

// Velocidades orbitais de cada planeta (em graus por segundo)
float orbitalSpeeds[] = {
    0.0f,   // Sol (não orbita)
    4.1f,   // Mercúrio (mais rápido)
    3.0f,   // Vênus
    2.5f,   // Terra
    2.0f,   // Marte
    1.0f,   // Júpiter
    0.7f,   // Saturno
    0.5f,   // Urano
    0.4f    // Netuno (mais lento)
};

GLfloat light0_ambient[] = { 0.2 , 0.2 , 0.2 , 1.0 };
GLfloat light0_diffuse[] = { 1.0 , 1.0 , 1.0 , 1.0 };
GLfloat light0_specular[] = { 1.0 , 1.0 , 1.0 , 1.0 };
GLfloat light0_position[] = { 0.0 , 0.0 , 0.0 , 1.0 };
//GLfloat spot_direction[] = { -1.0 , -1.0 , 0.0 };

GLfloat materialAmbient[]  = { 0.2, 0.2, 0.2, 1.0 };
GLfloat materialDiffuse[]  = { 0.7, 0.7, 0.7, 1.0 }; 
GLfloat materialSpecular[] = { 0.0, 0.0, 0.0, 1.0 }; 
GLfloat materialShininess[] = { 1.0 }; 

// Protótipos de funções
void updatePhysics();
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

//Função para carregar texturas para os objetos
void loadTexture ( const char * filename , GLuint * textureID) {
   int width , height , nrChannels ;
   unsigned char * data = stbi_load ( filename , & width , & height ,
      & nrChannels , 0);
   if ( data ) {
      glGenTextures (1 , textureID );                  
      glBindTexture ( GL_TEXTURE_2D , *textureID );

      // Set texture wrapping and filtering parameters
      glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S ,
      GL_REPEAT );
      glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T ,
      GL_REPEAT );
      glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER ,
      GL_LINEAR_MIPMAP_LINEAR );
      glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,
      GL_LINEAR );

      // Load the texture data ( check if it 's RGB or RGBA )
      if ( nrChannels == 3) {
         gluBuild2DMipmaps ( GL_TEXTURE_2D , GL_RGB , width ,
         height , GL_RGB , GL_UNSIGNED_BYTE , data );
         } else if ( nrChannels == 4) {
         gluBuild2DMipmaps ( GL_TEXTURE_2D , GL_RGBA , width ,
         height , GL_RGBA , GL_UNSIGNED_BYTE , data );
      }
      stbi_image_free ( data );
   } else {
      puts("Falied to load texture");
   }
}



//Função para definir as propriedades luminosas dos objetos
void lightConfig(){

   //Liga iluminação
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   //Define as propriedades de reflexão e posição
   glLightfv ( GL_LIGHT0 , GL_AMBIENT , light0_ambient );
   glLightfv ( GL_LIGHT0 , GL_DIFFUSE , light0_diffuse );
   glLightfv ( GL_LIGHT0 , GL_SPECULAR , light0_specular );
   glLightfv ( GL_LIGHT0 , GL_POSITION , light0_position );

   //Define as propriedades luminosas do material
   glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
   glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);

   //Mescla as propriedades de luz com a coloração do material
   glEnable(GL_COLOR_MATERIAL);
   glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);


}

void cameraAdjust(){

   //Cálculo dos vetores da câmera forward, right e up
   fwdX = -sin(cameraYaw * pi / 180.0) * cos(cameraPitch * pi / 180.0);           
   fwdY = sin(cameraPitch * pi / 180.0);
   fwdZ = cos(cameraYaw * pi / 180.0) * cos(cameraPitch * pi / 180.0);

   rightX = -cos(cameraYaw * pi / 180.0);
   rightY = 0.0;
   rightZ = -sin(cameraYaw * pi / 180.0);
   //Up é calculado através do produto vetorial entre forward e right
   upX = fwdY * rightZ - fwdZ * rightY;
   upY = fwdZ * rightX - fwdX * rightZ;
   upZ = fwdX * rightY - fwdY * rightX;

   //Normalização dos vetores, para que seus módulos tenhar valor 1
   float lenFwd = sqrt(fwdX*fwdX + fwdY*fwdY + fwdZ*fwdZ);
   fwdX == fwdX/lenFwd;
   fwdY == fwdY/lenFwd;
   fwdZ == fwdZ/lenFwd;
    
   float lenRight = sqrt(rightX*rightX + rightY*rightY + rightZ*rightZ);
   rightX == rightX/lenRight;
   rightY == rightY/lenRight;
   rightZ == rightZ/lenRight;
   
   float lenUp = sqrt(upX*upX + upY*upY + upZ*upZ);
   upX == upX/lenUp;
   upY == upY/lenUp;
   upZ == upZ/lenUp;

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glRotatef(cameraPitch, 1.0, 0.0, 0.0);
   glRotatef(cameraYaw, 0.0, 1.0, 0.0);
   glTranslatef(cameraX, cameraY, cameraZ);

   glutPostRedisplay();

}

void updatePhysics() {
    // Atualizar a posição de cada planeta (exceto o Sol)
    for (int i = 1; i < objectCount; i++) {
        // Incrementar o ângulo de rotação específico deste planeta
        rotationAngles[i] += orbitalSpeeds[i] * timeStep * 0.2f;
        if (rotationAngles[i] > 360.0f) {
            rotationAngles[i] -= 360.0f;
        }
        
        // Atualizar a posição do planeta para orbitar ao redor do Sol
        objects[i].posX = orbitalRadii[i] * cos(rotationAngles[i] * M_PI / 180.0f);
        objects[i].posZ = orbitalRadii[i] * sin(rotationAngles[i] * M_PI / 180.0f);
        // Manter os planetas no plano XZ
        objects[i].posY = 0.0f;
        
        // Também atualizar a rotação do próprio planeta
        objects[i].rotationAngle += objects[i].rotationSpeed * timeStep;
        if (objects[i].rotationAngle > 360.0f) {
            objects[i].rotationAngle -= 360.0f;
        }
    }
}

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);
   
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   lightConfig();

    stbi_set_flip_vertically_on_load(true);

   loadTexture("Texturas/sun.jpg", &sunTexName);
   loadTexture("Texturas/mercury.jpg", &mercuryTexName);
   loadTexture("Texturas/venus.jpg", &venusTexName);
   loadTexture("Texturas/earth.jpg", &earthTexName);
   loadTexture("Texturas/mars.jpg", &marsTexName);
   loadTexture("Texturas/jupiter.jpg", &jupiterTexName);
   loadTexture("Texturas/satrun.jpg", &saturnTexName);
   loadTexture("Texturas/uranus.jpg", &uranusTexName);
   loadTexture("Texturas/neptune.jpg", &neptuneTexName);

   // Limpar o array de objetos celestes
   objectCount = 0;
   
   // Inicializar ângulos de rotação
   for (int i = 0; i < MAX_OBJECTS; i++) {
      rotationAngles[i] = 0.0f;
   }
   
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
   // Ajustar rotação do Sol
   objects[objectCount-1].rotationSpeed = 0.5f;  // Rotação mais lenta para o Sol
   
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
   objects[objectCount-1].rotationSpeed = 0.1f;  // Mercúrio é lento (58.6 dias terrestres)
   
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
   objects[objectCount-1].rotationSpeed = 0.05f;  // Vênus é muito lento (243 dias terrestres) e retrógrado
   
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
   objects[objectCount-1].rotationSpeed = 2.0f;  // Rotação da Terra (1 dia)
   
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
   objects[objectCount-1].rotationSpeed = 1.9f;  // Marte (24.6 horas)
   
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
   objects[objectCount-1].rotationSpeed = 5.0f;  // Júpiter é rápido (9.9 horas)
   
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
   objects[objectCount-1].rotationSpeed = 4.5f;  // Saturno é rápido (10.7 horas)
   
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
   objects[objectCount-1].rotationSpeed = -3.0f;  // Urano tem rotação retrógrada (17.2 horas)
   
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
   objects[objectCount-1].rotationSpeed = 3.5f;  // Netuno (16.1 horas)
}

void display(void){
   // Atualiza a física
   updatePhysics();

   // Limpa o buffer de cores e profundidade
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //Define a posição de onde a luz é emitida
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

   // Desenha os objetos celestes
   for (int i = 0; i < objectCount; i++) {
      
      if(i==0){
         GLfloat emission[] = {1.0, 1.0, 0.1, 1.0};
         glMaterialfv(GL_FRONT, GL_EMISSION, emission);
      }else if(i==1){
         GLfloat noEmission[] = {0.0, 0.0, 0.0, 1.0};
         glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
      }
      
      glPushMatrix();

      // Posiciona o objeto
      glTranslatef(objects[i].posX, objects[i].posY, objects[i].posZ);

      //Aplicação da textura para o objeto
        if (objects[i].texture > 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, objects[i].texture);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);   //Rotaciona a esfera para a exibição adequada
        } else {
            glDisable(GL_TEXTURE_2D);
        }

      //Definição de cor sólida do corpo celeste, para caso não haja textura aplicada
      glColor3f(objects[i].r, objects[i].g, objects[i].b);

      // Criar uma esfera para o objeto
      GLUquadric* quadric = gluNewQuadric();
      gluQuadricTexture(quadric, GL_TRUE);   //Mapeia e desenha a textura
      gluQuadricNormals(quadric, GLU_SMOOTH);

      // Desenhar a esfera
      gluSphere(quadric, objects[i].radius, 32, 32);
      gluDeleteQuadric(quadric);

      glPopMatrix();
   }

   glutSwapBuffers();

   // Solicitar redesenho para animação
   glutPostRedisplay();
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
      case 'w': //Camera se move para frente
         cameraX += fwdX * cameraMove;
         cameraY += fwdY * cameraMove;
         cameraZ += fwdZ * cameraMove;
         cameraAdjust();
         break;
      case 's': //Camera se move para trás
         cameraX -= fwdX * cameraMove;
         cameraY -= fwdY * cameraMove;
         cameraZ -= fwdZ * cameraMove;
         cameraAdjust();
         break;
      case 'a': //Camera se move para a esquerda
         cameraX -= rightX * cameraMove;
         cameraY -= rightY * cameraMove;
         cameraZ -= rightZ * cameraMove;
         cameraAdjust();
         break;
      case 'd': //Camera se move para a direita
         cameraX += rightX * cameraMove;
         cameraY += rightY * cameraMove;
         cameraZ += rightZ * cameraMove;
         cameraAdjust();
         break;
      case '4': //Camera gira para a esquerda
         cameraYaw -= 5.0;
         cameraAdjust();
         break;
      case '6': //Camera gira para a direita
         cameraYaw += 5.0;
         cameraAdjust();
         break;
      case '5': //Camera gira para baixo
         cameraPitch += 5.0;
         cameraAdjust();
         break;
      case '8': //Camera gira para cima
         cameraPitch -= 5.0;
         cameraAdjust();
         break;
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