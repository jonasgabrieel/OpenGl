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
#include <SDL2/SDL_mixer.h>

// Para compilar: g++ carrinho.cpp -o carrinho -lassimp -lGL -lGLU -lglut -lSOIL -lSDL2 -lSDL2_mixer
// Para executar: ./carrinho


// Para mover o carrinho aperte no botões W,A,S,D
#define MAX_DIMENSION 1000
GLfloat luz_pontual[] = {0.3,0.5, 0.5, 1.0 };

// Variáveis para armazenar a posição da camera
float cameraX = -7.0f;
float cameraY = 0.0f;
float cameraZ = 0.8f;

// Variáveis para armazenar a posição do carrinho
float carX = 2.0f;
float carY = 20.0f;
float carZ = 0.0f;

int direcaoMovimento = 0; // Variável para definir se o carrinho está indo ou voltando

int** matrizImagem;
int largura;
int altura;

const char* carrinhoPath = "carrinho.obj"; // Caminho para o arquivo OBJ do carrinho
const char* cactoPath = "cacto.obj";
const float scaleFactor = 0.01f; // Fator de escala para ajustar o tamanho do modelo^

GLuint texName; // Variável para armazenar o nome da textura

/*-----------------Funções Auxiliares---------------------*/

void desenhar_luz(){
	
   glPushAttrib (GL_LIGHTING_BIT);
   
   GLfloat mat_diffuse[] = { 0.0, 1.0, 0.0, 0.0 };
   GLfloat mat_emission[] = { 1.0, 1.0, 0.0, 1.0 };
          
   //atribui características ao material
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
 
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
    
   glPushMatrix();
   glTranslatef(luz_pontual[0],luz_pontual[1],luz_pontual[2]); 
   glTranslatef(40.0, 20.0, 10.0); // Translada a esfera 2 unidades no eixo Z (aqui, estamos transladando-a para frente)

   
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
   GLfloat light0_position[] = { 40.0, 20.0, -10.0, 1.0 };
   GLfloat light0_diffuse[] = { 0.1, 0.1, 0.1, 1.0 };
   
   //atribui características para a fonte de luz 0
   //cor padrão: branco
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
   
   //LUZ 1
   
   //fonte de luz pontual (por que a coordenada homogênea w == 1?)
   //define características a serem associadas à fonte de luz 1	
   GLfloat light1_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
   GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light1_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
   
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

// Prever Mudança de Altura
// A função analisa se é uma elevação ou depressão
void preverElevacao(int x, int y, int i, int j, int addSubidaX, int addDescidaX, int addSubidaY, int addDescidaY){
    int linha = x + i;
    int coluna = y + j;
    if( linha < 0 || coluna < 0){
        return;
    }else{
        // Verificar se a Elevação tem a diferença de 1 metro
        if(matrizImagem[linha][coluna] - matrizImagem[x][y] == 1 ){
            carZ += 1.0; // Subir 1 metro
            carX += i + addSubidaX; // AddSubida é apenas para ajusta a subida
            carY += j + addSubidaY;
        }else{
            // Verificar se a depressão tem a diferença de 1 metro
            if(matrizImagem[linha][coluna] - matrizImagem[x][y] == -1){
                carZ -= 1.0; // Descer 1 metro
                carX += i + addDescidaX; // AddDescida é apenas para ajusta a descida
                carY += j + addDescidaY;
            }
        }
    }
}


// Prever Obstaculos
// A função analisa se o carrinho esta perto do fim do mapa ou se existe uma elevação muito grande ou depressão muito pequena
int preverObstaculo(int x, int y, int i, int j){
    int linha = x + i;
    int coluna = y + j;
    // Sinaliza o fim do mapa
    if((linha > 39 || linha < 0) || (coluna > 79 || coluna < 0)){
        return 0;
    }else{
        // Sinaliza a diferença de altura muito elevada ou muito baixa
        if( (matrizImagem[linha][coluna] - matrizImagem[x][y] > 1 || matrizImagem[linha][coluna] - matrizImagem[x][y] < -1)){
            return 0;
        }else{
            // Nenhum obstaculo
            return 1;
        }
    }
}

// Ler Imagem PGM
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

// Função para liberar a memória alocada para a matriz
void liberarMatriz(int** matriz, int altura) {
    if (matriz != NULL) {
        for (int i = 0; i < altura; ++i) {
            free(matriz[i]);
        }
        free(matriz);
    }
}

/*--------------------------------------------------------------/



/---------------------------------Funções Principais----------------------------------/


/-----------------Carrega textura do Carro---------------------*/

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carrega a imagem para a textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
   
    // Libera a memória alocada pela SOIL
    SOIL_free_image_data(image);
}

/*--------------------------------------------------------------/

/-----------------Carrega textura do Malha---------------------*/

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
/*--------------------------------------------------------------/



/-----------------Redesenha o carrinho e suas posições---------------------*/

void renderModel(const aiScene* scene) {
    if (!scene) {
        std::cerr << "Erro ao carregar o modelo do carrinho." << std::endl;
        return;
    }



    // Inicie a matriz de transformação atual
    glPushMatrix();
    glPushAttrib(GL_CURRENT_BIT);

    // Translação e rotação do carrinho
    printf("Direcao:%d\n", direcaoMovimento);
    int x = carX;
    int y = carY;

    // O carrinho vai para o NORTE
    if(direcaoMovimento == 1){
        if(preverObstaculo(carX+1,carY, 1, 0) ){
            carX += 1;
            preverElevacao(carX, carY, 1, 0, 1, 1, 0, 0);
            glTranslatef(carX,carY,carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        }else{
            glTranslatef(carX,carY,carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        }
    }

    // O carrinho vai para o SUL
    if(direcaoMovimento == 2){
        if(preverObstaculo(carX-1,carY, -1, 0)){
            carX -= 1;
            preverElevacao(carX, carY, -1, 0, -1, -1, 0, 0);
            glTranslatef(carX, carY, carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        }else{
            glTranslatef(carX, carY, carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);  
        }
    }

    // O carrinho vai para o OESTE
    if(direcaoMovimento == 3 ){
        if(preverObstaculo(carX,carY+1, 0, 1)){
            carY += 1;
            preverElevacao(carX,carY, 0, 1, 0, 0, 1, 1);
            glTranslatef(carX, carY, carZ);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(180.0f, 0.0, 0.0 , 1.0f);
        }else{
            glTranslatef(carX, carY, carZ);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(180.0f, 0.0, 0.0 , 1.0f); 
        }
    }

    // O carrinho vai para o LESTE
    if(direcaoMovimento == 4){
        if(preverObstaculo(carX,carY-1, 0, -1)){
            carY -= 1;
            preverElevacao(carX, carY, 0, -1, 0, 0, -1, -1); 
            glTranslatef(carX, carY, carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 
        }else{
            glTranslatef(carX, carY, carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 
        }
    }

    // O carrinho vai para o NORDOESTE
    if(direcaoMovimento == 5){
        if(preverObstaculo(carX+1,carY+1, 1, 1)){
            carX += 1;
            carY += 1;
            preverElevacao(carX, carY, 1, 1, 1, 1, 1, 1);
            glTranslatef(carX,carY,carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(135.0f, 0.0f, 1.0f, 0.0f);
        }else{
            glTranslatef(carX,carY,carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(135.0f, 0.0f, 1.0f, 0.0f); 
        }
    }

    // O carrinho vai para o SUDOESTE
    if(direcaoMovimento == 6){
        if(preverObstaculo(carX+1,carY-1, 1, -1)){
            carX += 1;
            carY -= 1;
            preverElevacao(carX, carY, 1, -1, 1, 1, -1, -1);
            glTranslatef(carX,carY,carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        }else{
            glTranslatef(carX,carY,carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(45.0f, 0.0f, 1.0f, 0.0f);  
        }
    }

    // O carrinho vai para o SUDESTE
    if(direcaoMovimento == 7){
        if(preverObstaculo(carX-1,carY+1, -1, 1)){
            carX -= 1;
            carY += 1;
            preverElevacao(carX, carY, -1, 1, -1, -1, 1, 1);
            glTranslatef(carX,carY,carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(-135.0f, 0.0f, 1.0f, 0.0f);
        }else{
            glTranslatef(carX,carY,carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(-135.0f, 0.0f, 1.0f, 0.0f);  
        }
    }

    // O carrinho vai para o NORDESTE
    if(direcaoMovimento == 8){
        if(preverObstaculo(carX-1,carY-1, -1, -1)){
            carX -= 1;
            carY -= 1;
            preverElevacao(carX, carY, -1, -1, -1, -1, -1, -1); 
            glTranslatef(carX,carY,carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);
        }else{
           glTranslatef(carX,carY,carZ);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(-45.0f, 0.0f, 1.0f, 0.0f); 
        }
    }
     //definir as propriedades do material para o shading
    GLfloat materialAmbiente[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat materialDifuso[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat materialEspecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat materialBrilho[] = {8.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbiente);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDifuso);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialEspecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, materialBrilho);



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

                glTexCoord2f(uv.x, 1.0f - uv.y); // Aplica as coordenadas de textura do modelo do carro
                glVertex3f(vertex.x * scaleFactor, vertex.y * scaleFactor, vertex.z * scaleFactor);
            }
        }
    }
    glEnd();
    glPopAttrib();
    glPopMatrix(); // Restaure a matriz de transformação anterior
}

void renderCactus(const aiScene* scene) {

    GLuint texNameCacto;
    glGenTextures(1, &texNameCacto);
    glBindTexture(GL_TEXTURE_2D, texNameCacto);
    int width, height;
    unsigned char* image = SOIL_load_image("cacto.jpeg", &width, &height, 0, SOIL_LOAD_RGB);
    if (!image) {
        std::cerr << "Erro ao carregar a textura do cacto." << std::endl;
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Inicie a matriz de transformação atual
    glPushMatrix();
    glPushAttrib(GL_CURRENT_BIT);

    // Definir as propriedades do material para o shading
    GLfloat materialAmbiente[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat materialDifuso[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat materialEspecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat materialBrilho[] = {500.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbiente);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDifuso);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialEspecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, materialBrilho);

    glTranslatef(30.0, 15.0, 0.0);

    // Aplicar as coordenadas de textura do modelo do cacto
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            const aiFace& face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                unsigned int index = face.mIndices[k];
                const aiVector3D& vertex = mesh->mVertices[index];
                const aiVector3D& uv = mesh->mTextureCoords[0][index]; // Coordenadas de textura do modelo
                glTexCoord2f(uv.x, uv.y); // Aplica as coordenadas de textura do modelo do cacto
                glVertex3f(vertex.x * scaleFactor, vertex.y * scaleFactor, vertex.z * scaleFactor);
            }
        }
    }
    glEnd();

    glPopAttrib();
    glPopMatrix(); // Restaure a matriz de transformação anterior
}

/*--------------------------------------------------------------/


/-----------------Renderizar a cena na tela---------------------*/
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    // Habilita o culling de faces só renderiza o que for visivél para o usuário
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Ajusta a direção do olhar para a direção em que o carrinho está indo
    float lookAtX = carX - cameraX; // Ajusta para trás do carrinho
    float lookAtY = carY - cameraY; // Ajusta para lado do carrinho
    float lookAtZ = carZ + 3.0; // Mantém a posição Z do carrinho

    // Configuração da câmera para simular o carrinho
    gluLookAt(carX + cameraX, carY + cameraY, carZ + 2.0, // Posição da câmera
              lookAtX, lookAtY, lookAtZ, // Ponto para o qual a câmera está olhando
              0.0, 0.0, 1.0); // Vetor "para cima"

    // Cor de fundo (branco)
    glClearColor(0.0f, 0.7f, 1.0f, 1.0f); // Azul ligeiramente mais escuro

    desenhar_luz();
    
     Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(carrinhoPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

    Assimp::Importer importa;
    const aiScene* cacto  = importa.ReadFile(cactoPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

    // Ativa o uso de textura
    glEnable(GL_TEXTURE_2D);

    // Configura o modo de combinação da textura (nesse caso, GL_REPLACE)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Define a textura atual como a textura de areia
    glBindTexture(GL_TEXTURE_2D, texName);
    

    renderModel(scene);
    renderCactus(cacto);
    

    glutSwapBuffers();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindTexture(GL_TEXTURE_2D, texName); // Aplica textura do terreno
    glBegin(GL_TRIANGLES);
    // Definir as propriedades do material
    GLfloat materialAmbiente[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat materialDifuso[] = {0.5f, 0.5f, 0.5f, 0.5f};
    GLfloat materialEspecular[] = {1.0f, 1.0f, 1.0f, 0.5f};
    GLfloat materialBrilho[] = {32.0f};

    // Aplicar as propriedades do material à malha
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbiente);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDifuso);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialEspecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, materialBrilho);
    for (int i = 0; i < 39; ++i) {
        for (int j = 0; j < 79; ++j) {
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
            float u1 = static_cast<float>(x1) / 79.0f;
            float v1 = static_cast<float>(y1) / 79.0f;
            float u2 = static_cast<float>(x2) / 79.0f;
            float v2 = static_cast<float>(y2) / 79.0f;
            float u3 = static_cast<float>(x3) / 79.0f;
            float v3 = static_cast<float>(y3) / 79.0f;
            float u4 = static_cast<float>(x4) / 79.0f;
            float v4 = static_cast<float>(y4) / 79.0f;

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
/*--------------------------------------------------------------/


/-------------------------------Movimentação do Carrinho-------------------------------*/
void movimentaCarrinho(unsigned char key, int x, int y) {
    switch (key) {
        case 'x':
        case 'X':
            direcaoMovimento = 2;
            glutPostRedisplay();
            break;
        case 'w':
        case 'W':
            direcaoMovimento = 1;
            glutPostRedisplay();
            break;
        case 'a':
        case 'A':
            direcaoMovimento = 3;
            glutPostRedisplay();
            break;
        case 'd':
        case 'D':
            direcaoMovimento = 4;
            glutPostRedisplay();
            break;
        case 'q':
        case 'Q':
            direcaoMovimento = 5;
            glutPostRedisplay();
            break;
        case 'e':
        case 'E':
            direcaoMovimento = 6;
            glutPostRedisplay();
            break;
        case 'z':
        case 'Z':
            direcaoMovimento = 7;
            glutPostRedisplay();
            break;
        case 'c':
        case 'C':
            direcaoMovimento = 8;
            glutPostRedisplay();
            break;
    }

}

// main
 
int main(int argc, char** argv) {
      // Inicialize o SDL Mixer
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    // Carregue e reproduza a música
    Mix_Music* music = Mix_LoadMUS("estrada.mp3");
    Mix_PlayMusic(music, -1); // -1 para reprodução infinita

    glutInit(&argc, argv);
    matrizImagem = lerImagemPGM("imagem.ppm", &largura, &altura);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Malha de Triângulos Vazados OpenGL");
    
    glEnable(GL_DEPTH_TEST);
    iluminar();
    loadSandTexture();
    loadCarTexture();

    glShadeModel (GL_SMOOTH);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(movimentaCarrinho);

    glutMainLoop();
    // Libera a memória alocada para a matriz
    liberarMatriz(matrizImagem, altura);
      // Libere a música e finalize o SDL Mixer
    Mix_FreeMusic(music);
    Mix_Quit();
    return 0;
}