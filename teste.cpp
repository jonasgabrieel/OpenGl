#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>

#define MAX_DIMENSION 1000

float cameraX = 10.0f;
float cameraY = 30.0f;
float cameraZ = 20.0f;

int** matrizImagem;
int largura;
int altura;

const char* carrinhoPath = "Car.obj"; // Caminho para o arquivo OBJ do carrinho
const float scaleFactor = 1.0f; // Fator de escala para ajustar o tamanho do modelo



void renderModel(const aiScene* scene) {
    if (!scene) {
        std::cerr << "Erro ao carregar o modelo do carrinho." << std::endl;
        return;
    }

    

 
    
    float anguloRotacao = 90.0f; // Rotacionar o carrinho 90 graus em torno do eixo verde (Y)

    glPushMatrix(); // Inicie a matriz de transformação atual
   
    // Renderize o modelo do carrinho
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[i];
        glBegin(GL_TRIANGLES);
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            const aiFace& face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                unsigned int index = face.mIndices[k];
                const aiVector3D& vertex = mesh->mVertices[index];
                glVertex3f(vertex.x * scaleFactor, vertex.y * scaleFactor, vertex.z * scaleFactor);
            }
        }
        glEnd();
    }

    glPopMatrix(); // Restaure a matriz de transformação anterior
}



int** lerImagemPPM(const char* nomeArquivo, int* largura, int* altura) {
    FILE *arquivo;
    char tipo[3];
    int maxValor;
    int i, j;

    arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return NULL;
    }
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>

#define MAX_DIMENSION 1000
GLfloat luz_pontual[] = {0.3, 0.5, 0.5, 1.0 };

float cameraX = 10.0f;
float cameraY = 30.0f;
float cameraZ = 20.0f;

int** matrizImagem;
int largura;
int altura;

const char* carrinhoPath = "carro.obj"; // Caminho para o arquivo OBJ do carrinho
const float scaleFactor = 0.01f; // Fator de escala para ajustar o tamanho do modelo


void desenhar_luz(){
	
   glPushAttrib (GL_LIGHTING_BIT);
   
   GLfloat mat_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
   GLfloat mat_emission[] = { 1.0, 1.0, 0.0, 1.0 };
          
   //atribui características ao material
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
 
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
    
   glPushMatrix();
   glTranslatef(luz_pontual[0],luz_pontual[1],luz_pontual[2]); 
   
   glEnable(GL_LIGHTING);
   glColor3f (1.0, 1.0, 0.0);
   glutSolidSphere(0.05,50,50);
   glDisable(GL_LIGHTING);
   
   glPopAttrib();
   glPopMatrix();
}

void iluminar(){
   //LUZ
   // fontes de luz são iniciadas com cor preta
   // número de fontes de luz afeta performance
    
   //LUZ 0
  
   //define características a serem associadas à fonte de luz 0	
   //fonte de luz direcional (por que a coordenada homogênea w == 0?)
   GLfloat light0_position[] = { 0.0, 1.0, 0.0, 0.0 };
   GLfloat light0_diffuse[] = { 0.1, 0.1, 0.1, 1.0 };
   
   //atribui características para a fonte de luz 0
   //cor padrão: branco
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
   
   //LUZ 1
   
   //fonte de luz pontual (por que a coordenada homogênea w == 1?)
   //define características a serem associadas à fonte de luz 1	
   GLfloat light1_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
   GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light1_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
   
   //atribui as características para a fonte de luz 1
   //(experimentem remover alguns dos componentes abaixo)
   glLightfv(GL_LIGHT1, GL_POSITION, luz_pontual);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
   glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient); 
   
   //"acende" cada uma das luzes configuradas
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
}



void renderModel(const aiScene* scene) {
    if (!scene) {
        std::cerr << "Erro ao carregar o modelo do carrinho." << std::endl;
        return;
    }
    
    float anguloRotacao = 90.0f; // Rotacionar o carrinho 90 graus em torno do eixo verde (Y)

    glPushMatrix(); // Inicie a matriz de transformação atual

    glRotatef(anguloRotacao, 0.0f, 1.0f, 0.0f);
    glRotatef(90.0f, 0.0, 0.0 , 1.0f);

    // GlTranslatef(Para os lados do terreno, Para cima e baixo, Para frente e tras);
    glTranslatef(20.0f, 0.0f, 3.0f);
    // Renderize o modelo do carrinho
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[i];
        glBegin(GL_TRIANGLES);
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            const aiFace& face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                unsigned int index = face.mIndices[k];
                const aiVector3D& vertex = mesh->mVertices[index];
                glVertex3f(vertex.x * scaleFactor, vertex.y * scaleFactor, vertex.z * scaleFactor);
            }
        }
        glEnd();
    }

    glPopMatrix(); // Restaure a matriz de transformação anterior
}



int** lerImagemPPM(const char* nomeArquivo, int* largura, int* altura) {
    FILE *arquivo;
    char tipo[3];
    int maxValor;
    int i, j;

    arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return NULL;
    }

    fscanf(arquivo, "%s", tipo);

    if (tipo[0] != 'P' || tipo[1] != '2') {
        printf("Este código só suporta arquivos PGM no formato P2.\n");
        fclose(arquivo);
        return NULL;
    }

    fscanf(arquivo, "%d %d", largura, altura);
    fscanf(arquivo, "%d", &maxValor);

    if (*largura > MAX_DIMENSION || *altura > MAX_DIMENSION) {
        printf("A dimensão da imagem é muito grande para ser armazenada na matriz.\n");
        fclose(arquivo);
        return NULL;
    }

    int** matriz = (int**)malloc(*altura * sizeof(int*));

    for (i = 0; i < *altura; ++i) {
        matriz[i] = (int*)malloc(*largura * sizeof(int));
        for (j = 0; j < *largura; ++j) {
            fscanf(arquivo, "%d", &matriz[i][j]);
        }
    }

    fclose(arquivo);

    return matriz;
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Configuração da câmera
    gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 0.0, 10.0);

    // Cor de fundo (branco)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Cor das arestas dos triângulos (azul escuro)
    glColor3f(0.0f, 0.0f, 0.5f);
     Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(carrinhoPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
    
    desenhar_luz();

    renderModel(scene);

    glutSwapBuffers();

glBegin(GL_LINES);
for (int i = 0; i < 38; ++i) {
    for (int j = 0; j < 38; ++j) {
        // Coordenadas dos vértices dos triângulos
        int x1 = i;
        int y1 = j;
        int x2 = i + 1;
        int y2 = j;
        int x3 = i + 1;
        int y3 = j + 1;
        float z1 = matrizImagem[x1][y1];
        float z2 = matrizImagem[x2][y2];
        float z3 = matrizImagem[x3][y3];

        // Desenhar as linhas das arestas dos triângulos
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);

        glVertex3f(x2, y2, z2);
        glVertex3f(x3, y3, z3);

        glVertex3f(x3, y3, z3);
        glVertex3f(x1, y1, z1);
        
        // Desenhar linha do triângulo adjacente
        glVertex3f(x1, y1, z1);
        glVertex3f(x1, y1 + 1, matrizImagem[x1][y1 + 1]);
        
        glVertex3f(x1, y1 + 1, matrizImagem[x1][y1 + 1]);
        glVertex3f(x3, y3, z3);
    }
}
glEnd();


    //visualização dos eixos
glBegin(GL_LINES);
    glColor3f (1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);

    glColor3f (0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
  
    glColor3f (0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
glEnd();

    glFlush();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        cameraX -= 0.1f;
        break;
    case GLUT_KEY_RIGHT:
        cameraX += 0.1f;
        break;
    case GLUT_KEY_UP:
        cameraY += 0.1f;
        break;
    case GLUT_KEY_DOWN:
        cameraY -= 0.1f;
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    matrizImagem = lerImagemPPM("imagem.ppm", &largura, &altura);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Malha de Triângulos Vazados OpenGL");
    
    glEnable(GL_DEPTH_TEST);
    iluminar();


    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}
    fscanf(arquivo, "%s", tipo);

    if (tipo[0] != 'P' || tipo[1] != '2') {
        printf("Este código só suporta arquivos PGM no formato P2.\n");
        fclose(arquivo);
        return NULL;
    }

    fscanf(arquivo, "%d %d", largura, altura);
    fscanf(arquivo, "%d", &maxValor);

    if (*largura > MAX_DIMENSION || *altura > MAX_DIMENSION) {
        printf("A dimensão da imagem é muito grande para ser armazenada na matriz.\n");
        fclose(arquivo);
        return NULL;
    }

    int** matriz = (int**)malloc(*altura * sizeof(int*));

    for (i = 0; i < *altura; ++i) {
        matriz[i] = (int*)malloc(*largura * sizeof(int));
        for (j = 0; j < *largura; ++j) {
            fscanf(arquivo, "%d", &matriz[i][j]);
        }
    }

    fclose(arquivo);

    return matriz;
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Configuração da câmera
    gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 0.0, 10.0);

    // Cor de fundo (branco)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Cor das arestas dos triângulos (azul escuro)
    glColor3f(0.0f, 0.0f, 0.5f);
     Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(carrinhoPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
    
    renderModel(scene);

    glutSwapBuffers();

glBegin(GL_LINES);
for (int i = 0; i < 38; ++i) {
    for (int j = 0; j < 38; ++j) {
        // Coordenadas dos vértices dos triângulos
        int x1 = i;
        int y1 = j;
        int x2 = i + 1;
        int y2 = j;
        int x3 = i + 1;
        int y3 = j + 1;
        float z1 = matrizImagem[x1][y1];
        float z2 = matrizImagem[x2][y2];
        float z3 = matrizImagem[x3][y3];

        // Desenhar as linhas das arestas dos triângulos
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);

        glVertex3f(x2, y2, z2);
        glVertex3f(x3, y3, z3);

        glVertex3f(x3, y3, z3);
        glVertex3f(x1, y1, z1);
        
        // Desenhar linha do triângulo adjacente
        glVertex3f(x1, y1, z1);
        glVertex3f(x1, y1 + 1, matrizImagem[x1][y1 + 1]);
        
        glVertex3f(x1, y1 + 1, matrizImagem[x1][y1 + 1]);
        glVertex3f(x3, y3, z3);
    }
}
glEnd();


    //visualização dos eixos
    glBegin(GL_LINES);
        glColor3f (1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);
        
        glColor3f (0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 1.0, 0.0);
      
        glColor3f (0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 1.0);    
    glEnd();

    glFlush();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        cameraX -= 0.1f;
        break;
    case GLUT_KEY_RIGHT:
        cameraX += 0.1f;
        break;
    case GLUT_KEY_UP:
        cameraY += 0.1f;
        break;
    case GLUT_KEY_DOWN:
        cameraY -= 0.1f;
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    matrizImagem = lerImagemPPM("imagem.ppm", &largura, &altura);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Malha de Triângulos Vazados OpenGL");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}