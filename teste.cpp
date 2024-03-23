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
#include <SOIL/SOIL.h>



// Para compilar:  g++ teste.cpp -o teste -lGL -lGLU -lglut -lassimp -lSOIL
// Para executar: ./teste


// Para mover o carrinho aperte no botões W,A,S,D


#define MAX_DIMENSION 1000
GLfloat luz_pontual[] = {0.3,0.5, 0.5, 1.0 };

float cameraX = -8.0f;
float cameraY = 0.0f;
float cameraZ = 3.0f;

// Variáveis para armazenar a posição do carrinho

float carX = 3.0f;
float carY = 20.0f;
float carZ = 0.0f;
float angulo = 0.0f; // Variável para definir o angulo do carrinho enquanto sobe ou descer a ladeira
int direcaoMovimento = 0; // Variável para definir se o carrinho está indo ou voltando
float distanciaLadeira = 0; // Variável para definir a distancia do carro para a ladeira proxima.

int** matrizImagem;
int largura;
int altura;

const char* carrinhoPath = "carrinho.obj"; // Caminho para o arquivo OBJ do carrinho
const float scaleFactor = 0.01f; // Fator de escala para ajustar o tamanho do modelo^


GLuint texName; // Variável para armazenar o nome da textura

/*-----------------Carrega textura do Carro---------------------*/

GLuint texNameCarrinho; // Variável para armazenar o nome da textura do carrinho

void loadCarTexture() {
    // Carrega a imagem da textura do carrinho usando a SOIL
    int width, height, channels;
    unsigned char* image = SOIL_load_image("texturaCar.png", &width, &height, &channels, SOIL_LOAD_RGBA);
    if (!image) {
        std::cerr << "Erro ao carregar a imagem da textura do carrinho." << std::endl;
        return;
    }
    // Gera uma textura OpenGL
    glGenTextures(1, &texNameCarrinho);
    glBindTexture(GL_TEXTURE_2D, texNameCarrinho);

    // Define os parâmetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carrega a imagem para a textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // Libera a memória alocada pela SOIL
    SOIL_free_image_data(image);
}

/*--------------------------------------------------------------*/
void loadSandTexture() {
    // Carrega a imagem JPEG usando a SOIL
    int width, height, channels;
    unsigned char* image = SOIL_load_image("solo.jpg", &width, &height, &channels, SOIL_LOAD_RGBA);
    if (!image) {
        std::cerr << "Erro ao carregar a imagem de areia." << std::endl;
        return;
    }
    // Gera uma textura OpenGL
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);

    // Define os parâmetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carrega a imagem para a textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // Libera a memória alocada pela SOIL
    SOIL_free_image_data(image);
}
void desenhar_luz(){
	
   glPushAttrib (GL_LIGHTING_BIT);
   
   GLfloat mat_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };
   GLfloat mat_emission[] = { 1.0, 1.0, 0.0, 1.0 };
          
   //atribui características ao material
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
 
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
    
   glPushMatrix();
   glTranslatef(luz_pontual[0],luz_pontual[1],luz_pontual[2]); 
   glTranslatef(0.0, 0.0, 10.0); // Translada a esfera 2 unidades no eixo Z (aqui, estamos transladando-a para frente)

   
   glEnable(GL_LIGHTING);
   glColor3f (1.0, 1.0, 0.0);
   glutSolidSphere(2.0,50,50);
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

void anguloDoCarrinho(){
    // Futura movimentação que faz o carrinho inclinar para subir e descer em elevações do plano
}

void preverElevacao(){
    // Função para prever se existe alguma elevação no caminho para o carrinho.
}


double distanciaEuclidiana(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Função que prever alguma elevação no terreno e sinalizar quando estiver perto
int preverElevacao(float x, float y){
    int intX = x;
    int intY = y;
    printf("%d, %d %lf\n", matrizImagem[intX+3][intY], matrizImagem[intX][intY], distanciaLadeira);
    if (distanciaLadeira > 0.09){
        if(distanciaLadeira <= 2.4){
            return 1;
        }else{
            return 0;
        }
    }else{
        for(int i=1; i <= 3; i++){
            angulo = 0.0f;
            if(matrizImagem[intX+i][intY] != matrizImagem[intX][intY]){
                printf("tem ladeira\n");
                distanciaLadeira = distanciaEuclidiana(intX+i,intY, intX, intY)-0.0;
                if(distanciaLadeira == 1){
                    angulo = 25.956f;
                }
                if(distanciaLadeira == 2){
                    angulo = 7.416f;
                }
                return 1;
            }
        }
        carZ = matrizImagem[intX][intY];
        angulo = 0.0f;
        return 0;
    }
}



void renderModel(const aiScene* scene) {
    if (!scene) {
        std::cerr << "Erro ao carregar o modelo do carrinho." << std::endl;
        return;
    }

    // Defina o ângulo de rotação para rotacionar o carrinho 90 graus em torno do eixo Y
    float anguloRotacao = 90.0f;

    // Inicie a matriz de transformação atual
    glPushMatrix();
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(0.0f, 0.0f, 1.0f); // Defina a cor do carrinho

    // Translação e rotação do carrinho
    glTranslatef(carX, carY, carZ);
    glRotatef(anguloRotacao, 0.0f, 1.0f, 0.0f);
    glRotatef(90.0f, 0.0, 0.0, 1.0f);

    // Aumenta o ângulo do carro e sobe um pouco no eixo Z se estiver indo para frente em uma ladeira
    if (direcaoMovimento == 1) {
        if (preverElevacao(carX, carY)) {
            distanciaLadeira -= 0.1;
            angulo += 1.875f;
            carZ += 0.042f;
            glRotatef(-angulo, 1.0f, 0.0, 0.0);
        } else {
            distanciaLadeira -= 0.1;
        }
    } else {
        // Diminui o ângulo do carro se estiver voltando para trás em uma ladeira
        if (direcaoMovimento == -1) {
            if (preverElevacao(carX, carY)) {
                if (angulo == 0) {
                    angulo = 45.0f;
                    carZ -= 0.042f;
                    glRotatef(-angulo, 1.0f, 0.0f, 0.0f);
                } else {
                    distanciaLadeira += 0.1;
                    angulo -= 1.875f;
                    carZ -= 0.042f;
                    glRotatef(-angulo, 1.0f, 0.0f, 0.0f);
                }
            }
        } else {
            // Não realize nenhuma alteração no ângulo se estiver movendo a câmera ou indo para os lados
            if (direcaoMovimento == 0) {
                glRotatef(-angulo, 1.0f, 0.0f, 0.0f);
            }
        }
    }

    // Carro
    glBindTexture(GL_TEXTURE_2D, texNameCarrinho); // Use a textura do carro

    // Aplicar as coordenadas de textura do modelo do carro
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            const aiFace& face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                unsigned int index = face.mIndices[k];
                const aiVector3D& vertex = mesh->mVertices[index];
                const aiVector3D& uv = mesh->mTextureCoords[0][index]; // Coordenadas de textura do modelo
                glTexCoord2f(uv.x, uv.y); // Aplica as coordenadas de textura do modelo do carro
                glVertex3f(vertex.x * scaleFactor, vertex.y * scaleFactor, vertex.z * scaleFactor);
            }
        }
    }
    glEnd();

    glPopAttrib();
    glPopMatrix(); // Restaure a matriz de transformação anterior
}



int** lerImagemPGM(const char* nomeArquivo, int* largura, int* altura) {
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

    // Ajusta a direção do olhar para a direção em que o carrinho está indo
    float lookAtX = carX - cameraX; // Ajusta para trás do carrinho
    float lookAtY = carY - cameraY; // Ajusta para lado do carrinho
    float lookAtZ = carZ - cameraZ; // Mantém a posição Z do carrinho

    // Configuração da câmera para simular o carrinho
    gluLookAt(carX + cameraX, carY + cameraY, carZ + cameraZ, // Posição da câmera
              lookAtX, lookAtY, lookAtZ, // Ponto para o qual a câmera está olhando
              0.0, 0.0, 1.0); // Vetor "para cima"

    // Cor de fundo (branco)
    glClearColor(0.0f, 0.0f, 0.9f, 0.0f);

    desenhar_luz();
    
    // Configurar o material do carrinho
    GLfloat mat_ambient[] = {2.0f, 0.2f, 0.2f, 1.0f};
    GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 0.5f};
    GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 0.5f};
    GLfloat mat_shininess[] = {50.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
     
    // Definir a cor do carrinho
    glColor3f(1.0f, 0.0f, 0.0f); // Vermelho

    

    // Cor das arestas dos triângulos (azul escuro)
    glColor3f(0.0f, 0.0f, 0.5f);
     Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(carrinhoPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

    // Ativa o uso de textura
    glEnable(GL_TEXTURE_2D);

    // Configura o modo de combinação da textura (nesse caso, GL_REPLACE)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Define a textura atual como a textura de areia
    glBindTexture(GL_TEXTURE_2D, texName);
    

    renderModel(scene);
    

    glutSwapBuffers();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindTexture(GL_TEXTURE_2D, texName); // Aplica textura do terreno
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 39; ++i) {
        for (int j = 0; j < 39; ++j) {
            // Coordenadas dos vértices dos triângulos
            int x1 = i;
            int y1 = j;
            int x2 = i + 1;
            int y2 = j;
            int x3 = i;
            int y3 = j + 1;
            int x4 = i + 1;
            int y4 = j + 1;
            float z1 = matrizImagem[x1][y1];
            float z2 = matrizImagem[x2][y2];
            float z3 = matrizImagem[x3][y3];
            float z4 = matrizImagem[x4][y4];

            // Coordenadas de textura correspondentes aos vértices
            float u1 = static_cast<float>(x1) / 39.0f;
            float v1 = static_cast<float>(y1) / 39.0f;
            float u2 = static_cast<float>(x2) / 39.0f;
            float v2 = static_cast<float>(y2) / 39.0f;
            float u3 = static_cast<float>(x3) / 39.0f;
            float v3 = static_cast<float>(y3) / 39.0f;
            float u4 = static_cast<float>(x4) / 39.0f;
            float v4 = static_cast<float>(y4) / 39.0f;

            // Triângulo 1
            glTexCoord2f(u1, v1);
            glVertex3f(x1, y1, z1);
            glTexCoord2f(u2, v2);
            glVertex3f(x2, y2, z2);
            glTexCoord2f(u3, v3);
            glVertex3f(x3, y3, z3);

            // Triângulo 2
            glTexCoord2f(u2, v2);
            glVertex3f(x2, y2, z2);
            glTexCoord2f(u4, v4);
            glVertex3f(x4, y4, z4);
            glTexCoord2f(u3, v3);
            glVertex3f(x3, y3, z3);
        }
    }
    glEnd();
// Desative o uso de textura após desenhar a malha
    glDisable(GL_TEXTURE_2D);
     // Troca o buffer de desenho com o buffer de exibição
    glutSwapBuffers();

glEnd();


    //visualização dos eixos
glBegin(GL_LINES);
    glColor3f (1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(1.0, 0.0, 1.0);

    glColor3f (0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 1.0, 1.0);
  
    glColor3f (0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 2.0);
glEnd();

    glFlush();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (double)w / (double)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        cameraX -= 0.1f;
        direcaoMovimento = 0;
        break;
    case GLUT_KEY_RIGHT:
        cameraX += 0.1f;
        direcaoMovimento = 0;
        break;
    case GLUT_KEY_UP:
        cameraY += 0.1f;
        direcaoMovimento = 0;
        break;
    case GLUT_KEY_DOWN:
        cameraY -= 0.1f;
        direcaoMovimento = 0;
        break;
    }
    glutPostRedisplay();
}

void movimentaCarrinho(unsigned char key, int x, int y) {
    switch (key) {
        case 's':
        case 'S':
            carX -= 0.1f;
            direcaoMovimento = -1;
            break;
        case 'w':
        case 'W':
            carX += 0.1f;
            direcaoMovimento = 1;
            break;
        case 'a':
        case 'A':
            carY += 0.1f;
            direcaoMovimento = 0;
            break;
        case 'd':
        case 'D':
            direcaoMovimento = 0;
            carY -= 0.1f;
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    matrizImagem = lerImagemPGM("imagem.ppm", &largura, &altura);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Malha de Triângulos Vazados OpenGL");
    
    glEnable(GL_DEPTH_TEST);
    iluminar();
    loadSandTexture();

    loadCarTexture();


    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(movimentaCarrinho);

    glutMainLoop();
    return 0;
}