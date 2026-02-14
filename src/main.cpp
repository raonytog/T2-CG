#include "../includes/arena.h"
#include "../includes/jogador.h"
#include "../includes/tiro.h"
#include "../includes/utils.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "../includes/tinyxml2.h"
using namespace tinyxml2;

#include <algorithm>
#include <math.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>

#define V_JOGADOR (GLfloat) 0.1
#define OMEGA_JOGADOR (GLfloat) .1
#define OMEGA_BRACO (GLfloat).1
#define V_TIRO (GLfloat) 0.2

#define DETALHE_CORACAO 18
#define TAMANHO_CORACAO (GLfloat) 24
#define DISTANCIA_CORACAO (GLfloat) 8 // distância entre um coração e outro e distância da borda

#define CAM_POV 0
#define CAM_GUN_POV 1
#define CAM_3TH_POV 2
//Key status
int key_status[256];

// MousePos
GLfloat mouse_x = 0.0f;
GLfloat mouse_Y = 0.0f;

// Window dimensions
const GLint width = 800;
const GLint height = 500;

// Viewing dimensions
const GLint viewing_width = 400;
const GLint viewing_height = 500;

const GLint fov = 60;
const GLfloat aspect_ratio = (GLfloat)viewing_width / (GLfloat)viewing_height;

const GLfloat light_position[] = {400, 0, 400, 0.75};

GLfloat light_position_j_1[] = {0, 0, 0, 0.75};
GLfloat light_difusa__j_1[] = {0, 0, 0, 0.75};

GLfloat light_position_j_2[] = {0, 0, 0, 0.75};
GLfloat light_difusa__j_2[] = {0, 0, 0, 0.75};

// const GLfloat luz_ambiente[] = { 0.2f, 0.2f, 0.2f, 1.0f }; 
const GLfloat luz_difusa[]   = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat luz_especular[]= { 1.0f, 1.0f, 1.0f, 1.0f };

// Objetos
Arena *arena = nullptr;

Jogador *j_1 = nullptr;
Jogador *j_2 = nullptr;

std::vector<Tiro> tiros;

int estado = 0;
// -1 para quit
// 0 se o jogo ainda está ocorrendo,
// 1 se o jogador 1 ganhou,
// 2 se o jogador 2 ganhou,
// 3 se foi empate

// Mensagem de vitória
static char str[32];
void * font = GLUT_BITMAP_HELVETICA_18;
void MensagemDeVitoria(GLfloat x, GLfloat y)
{
    glColor3f(1.0, 1.0, 1.0);

    // cria a string a ser impressa
    char *tmp_str;
    if (estado == 3)
        sprintf(str, "Os jogadores empataram");
    else
        sprintf(str, "O jogador %d venceu", estado);

    // define a posicao onde vai comecar a imprimir
    float x_offset = glutBitmapLength(font, (const unsigned char *)str) / 2.0f;
    float y_offset = 9.0f; // pois a fonte é tamanho 18

    glRasterPos2f(x - x_offset, y - y_offset);

    // imprime um caractere por vez
    tmp_str = str;
    while(*tmp_str)
    {
        glutBitmapCharacter(font, *tmp_str);
        tmp_str++;
    }
}

void SetaLuzPersonagens(void)
{
    // luz do j1
    light_position_j_1[0] = j_1->X();
    light_position_j_1[1] = j_1->Y();
    light_position_j_1[2] = j_1->Z();
    light_position_j_1[3] = 0.75f;

    light_difusa__j_1[0] = j_1->R();
    light_difusa__j_1[1] = j_1->G();
    light_difusa__j_1[2] = j_1->B();
    light_difusa__j_1[3] = 1;

    // luz do j2
    light_position_j_2[0] = j_2->X();
    light_position_j_2[2] = j_2->Y();
    light_position_j_2[2] = j_2->Z();
    light_position_j_2[3] = 0.75f;

    light_difusa__j_1[0] = j_1->R();
    light_difusa__j_1[1] = j_1->G();
    light_difusa__j_1[2] = j_1->B();
    light_difusa__j_1[3] = 1;
}

struct Circulo
{
    float x;
    float y;
    float raio;
    std::string cor;
};

// dados do SVG
// inicialmente de acordo com os valores no SVG
// mas são ajustados na primeira chamada da main
// para serem convertidos às dimensões do jogo
std::vector<Circulo> circulos;

std::vector<Circulo> LeSVG(const std::string& nome_arquivo) {
    std::vector<Circulo> ret;

    XMLDocument doc;
    XMLError erro = doc.LoadFile(nome_arquivo.c_str());
    if (erro != XML_SUCCESS)
    {
        fprintf(stderr, "erro ao carregar o arquivo SVG: %d\n", erro);
        return ret;
    }

    XMLElement* p_root = doc.FirstChildElement("svg");
    if (p_root == nullptr)
    {
        fprintf(stderr, "erro: Arquivo SVG não contém a tag raiz <svg>.\n");
        return ret;
    }
    
    XMLElement* p_elemento = p_root->FirstChildElement();

    while (p_elemento != nullptr)
    {
        // lê o arquivo SGV até retornar todos os círculos
        const char* tag_nome = p_elemento->Name();

        if (std::strcmp(tag_nome, "circle") == 0)
        {
            Circulo circulo;
            
            p_elemento->QueryFloatAttribute("cx", &circulo.x);
            p_elemento->QueryFloatAttribute("cy", &circulo.y);
            p_elemento->QueryFloatAttribute("r", &circulo.raio);
            
            const char* fill = p_elemento->Attribute("fill");
            if (fill)
                circulo.cor = fill;
            else
                circulo.cor = "none";
            ret.push_back(circulo);
        }
        p_elemento = p_elemento->NextSiblingElement();
    }

    return ret;
}

void DesenhaCoracoes(int vidas_1, int vidas_2)
{
    // desenha corações do jogador 1 da esquerda pra direita
    GLfloat x_atual = -viewing_width / 2.0f + DISTANCIA_CORACAO + TAMANHO_CORACAO / 2.0f;
    GLfloat y_atual = viewing_height / 2.0f - DISTANCIA_CORACAO - TAMANHO_CORACAO / 2.0f;

    for (int i = 0; i < vidas_1; i++)
    {
        glPushMatrix();
        glTranslatef(x_atual, y_atual, 0.0f);
        DesenhaCoracao(TAMANHO_CORACAO, TAMANHO_CORACAO, 1.0f, 0.0f, 0.0f, DETALHE_CORACAO);
        glPopMatrix();

        x_atual += DISTANCIA_CORACAO + TAMANHO_CORACAO;
    }

    // desenha corações do jogador 2 da direita pra esquerda
    x_atual = viewing_width / 2.0f - DISTANCIA_CORACAO - TAMANHO_CORACAO / 2.0f;
    for (int i = 0; i < vidas_2; i++)
    {
        glPushMatrix();
        glTranslatef(x_atual, y_atual, 0.0f);
        DesenhaCoracao(TAMANHO_CORACAO, TAMANHO_CORACAO, 0.0f, 1.0f, 0.0f, DETALHE_CORACAO);
        glPopMatrix();

        x_atual -= DISTANCIA_CORACAO + TAMANHO_CORACAO;
    }
}

// Coloca a câmera atrás do jogador e olhando para ele
void PosicionaCamera(Jogador* p) {
    // Pegamos a posição e o ângulo do jogador
    float x = p->X();
    float y = p->Y();
    float r = p->RaioColisao();
    // Assumindo que o ângulo no seu Jogador está em graus e precisa converter para radianos
    // Se já tiver getter para radianos, use-o.
    // O deslocamento -50 coloca a câmera "atrás" (ajuste conforme a escala do seu mundo)
    float dist_camera = 50.0f; 
    float altura_camera = 30.0f;
    float theta_rad = p->Theta() * M_PI / 180.0f; 

    // Posição do Olho (Câmera)
    // Calcula x/y atrás do jogador baseado no ângulo
    // float cam_x = x - dist_camera * cos(theta_rad);
    // float cam_y = y - dist_camera * sin(theta_rad);
    // float cam_z = altura_camera; // Altura da câmera (Z é up)

    float cam_x = x;
    float cam_y = y;
    float cam_z = ALTURA_MEMBROS*3;

    // Ponto para onde a câmera olha (Centro do Jogador)
    float look_x = x;
    float look_y = y;
    float look_z = 0.0f; // Olha para o chão/centro do boneco

    // Vetor Up (Onde fica o "céu")
    // Como estamos em um mundo 2D virando 3D, geralmente Z é o Up
    gluLookAt(cam_x, cam_y, cam_z,  // Eye
              look_x, look_y, look_z,  // Center
              0.0f, 0.0f, 1.0f);       // Up
}

void DrawAxes(double size) {
    GLfloat mat_ambient_r[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient_g[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat mat_ambient_b[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);

    //x axis red
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_r);
        glColor3fv(mat_ambient_r);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //y axis green
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION,  mat_ambient_g);
        glColor3fv(mat_ambient_g);
        glRotatef(90,0,0,1);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //z axis blue
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_b);
        glColor3fv(mat_ambient_b);
        glRotatef(-90,0,1,0);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();
}

// Em src/main.cpp

void ConfiguraCameraJogador(Jogador* p, int cam) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect_ratio, 0.1, 2000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // 1. Posição do Olho (Câmera)
    float eyeX = p->X() + p->RaioColisao();
    float eyeY = p->Y();
    float eyeZ = p->Z() + ALTURA_MEMBROS * 2.5f;

    float theta_rad = p->Theta() * M_PI / 180.0f;

    // 2. Look vector 
    float lookX = eyeX + cos(theta_rad);
    float lookY = eyeY + sin(theta_rad);
    float lookZ = eyeZ; // Olha reto no horizonte

    // 3. Vetor Up (O topo da cabeça aponta para Z positivo)
    gluLookAt(eyeX, eyeY, eyeZ,  // Olho
              lookX, lookY, lookZ,  // Foco
              0.0f, 0.0f, 1.0f);    // Up Vector
}

// Em src/main.cpp

void DesenhaHUD()
{
    // reseta o viewport para a tela toda
    glViewport(0, 0, width, height);

    // Configura projeção 2D (Ortogonal)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(-width/2, width/2, 
                   -height/2, height/2);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();

            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);
            DesenhaCoracoes(j_1->Vidas(), j_2->Vidas());

            if (estado > 0) { MensagemDeVitoria(0, 50); }

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void luzJogador(int qualJogador) {
    if (qualJogador == 1) 
    {
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT1, GL_POSITION, light_position_j_1);
        return;
    }

    if (qualJogador == 2)
    {
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT1, GL_POSITION, light_position_j_2);
        return;
    }
}

void renderPlayerScene(Jogador *p1, Jogador *p2) 
{
    if (key_status[(int) 'v'] == 0 || key_status[(int) 'b'] == 0) 
    {
        ConfiguraCameraJogador(p1, CAM_POV);

        if (p1->R() == 1) luzJogador(1);
        else if (p1->R() == 0) luzJogador(2);

        arena->Desenha();
        p1->Desenha();
        p2->Desenha();
    }

    if (key_status[(int) 'v'] == 1 && key_status[(int) 'b'] == 0) 
    {
    }

    if (key_status[(int) 'v'] == 0 && key_status[(int) 'b'] == 1) 
    {
    }
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // p1
    glViewport(0, 0, width/2, height);
    renderPlayerScene(j_1, j_2);
    // p2
    glViewport(width/2, 0, width/2, height);
    renderPlayerScene(j_2, j_1);

    DesenhaHUD();
    if (estado > 0) MensagemDeVitoria(0, 0);
    
    glutSwapBuffers();
}

void resetKeyStatus()
{
    int i;
    for (i = 0; i < 256; i++)
       key_status[i] = 0; 
}

void reset()
{
    // procura os valores iniciais dos jogadores
    float x_1 = 0.0f, y_1 = 0.0f, z_1 = 0.0f, r_1 = 0.0f;
    float x_2 = 0.0f, y_2 = 0.0f, z_2 = 0.0f, r_2 = 0.0f;

    for (auto& c : circulos) {
        if (c.cor == "red")
        {
            x_1 = c.x;
            y_1 = c.y;
            r_1 = c.raio;
        }
        if (c.cor == "green")
        {
            x_2 = c.x;
            y_2 = c.y;
            r_2 = c.raio;
        }

        if (r_1 > 0 && r_2 > 0)
            break;
    }

    // ângulo inicial dos jogadores
    GLfloat theta_1 = normalizaAnguloGraus(atan2(y_2 - y_1, x_2 - x_1) * 180.0f / PI);
    GLfloat theta_2 = normalizaAnguloGraus(theta_1 + 180.0f);

    delete arena;
    arena = new Arena(0.0f, 0.0f, 0.0f, std::min(viewing_width, viewing_height) / 2.0f, 0.0f, 0.0f, 1.0f);

    tiros.clear();

    delete j_1;
    delete j_2;
    j_1 = new Jogador(x_1, y_1, z_1, r_1, 1.0f, 0.0f, 0.0f, theta_1, 3);
    j_2 = new Jogador(x_2, y_2, z_2, r_2, 0.0f, 1.0f, 0.0f, theta_2, 3);

    for (auto& c : circulos) {
        if (c.cor == "black")
        {
            arena->adicionaObstaculo(c.x, c.y, 0, c.raio);
        }
    }

    // inicia o jogo com o jogador 1 (controlado pelo mouse) com o braço em direção
    // ao jogador 2
    mouse_x = j_2->X();
    mouse_Y = j_2->Y();

    resetKeyStatus();
    key_status[(int) 'r'] = 1; // mantém o status de r como apertado
                               // para evitar vários resets sequenciais
}

void quit()
{
    delete arena;
    delete j_1;
    delete j_2;
}

void keyPress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'r':
        case 'R':
            if (key_status[(int) 'r'] == 0) // confere se não estava pressionado antes
            {                               // para evitar vários resets sequenciais
                reset();
                estado = 0;
            }
            break;
        case 'w':
        case 'W':
            key_status[(int) 'w'] = 1;
            break;
        case 's':
        case 'S':
            key_status[(int) 's'] = 1;
            break;
        case 'o':
        case 'O':
            key_status[(int) 'o'] = 1;
            break;
        case 'l':
        case 'L':
            key_status[(int) 'l'] = 1;
            break;
        case 'a':
        case 'A':
            key_status[(int) 'a'] = 1;
            break;
        case 'd':
        case 'D':
            key_status[(int) 'd'] = 1;
            break;
        case 'k':
        case 'K':
            key_status[(int) 'k'] = 1;
            break;
        case 231: // ç
        case 199: // Ç
        case ';': // equivalente ao ç no meu wsl
                  // que não sabe que eu tenho um teclado brasileiro
            key_status[231] = 1;
            break;
        case '4':
            key_status[(int) '4'] = 1;
            break;
        case '6':
            key_status[(int) '6'] = 1;
            break;
        case '5':
            key_status[(int) '5'] = 1;
            break;
        case 27:
            estado = -1;
        case 'x':
            key_status[(int) 'x'] = 1;
            break;
        case '.':
            key_status[(int) '.'] = 1;
            break;
        case '1': //debug -> perde vida do p1
            key_status[(int) '1'] = 1;
            break;
        case '2': //debug -> perde vida do p2
            key_status[(int) '2'] = 1;
            break;
        case 'v': //camera a cima da arma
            key_status[(int) 'v'] = 1;
            break;
        case 'b': // terceira pesssoa com incremento/decremento de zoom com + ou -
            key_status[(int) 'b'] = 1;
            break;
    }
    glutPostRedisplay();
}

void keyUp(unsigned char key, int x, int y)
{
    key_status[(int)(key)] = 0;
    if (key == ';') // ver keyPress
    {
        key_status[231] = 0;
    }
    glutPostRedisplay();
}

void mouseClick(int button, int state, int x, int y) {
    if (estado == 0 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (j_1->PodeAtirar())
        {
            GLfloat tiro_x = j_1->XPontaBraco();
            GLfloat tiro_y = j_1->YPontaBraco();
            GLfloat tiro_z = j_1->Altura(); // NAO Ẽ ISSO
            GLfloat tiro_tamanho = j_1->EspessuraBraco();
            GLfloat tiro_theta = j_1->ThetaBraco();

            tiros.emplace_back(tiro_x, tiro_y, tiro_z, tiro_tamanho, tiro_theta, 1);

            j_1->ResetaTimer();
        }
    }
}

void mouseMove(int x, int y) {
    mouse_x = viewing_width * ((float) x / width - 0.5f);
    mouse_Y = -viewing_height * ((float) y / height - 0.5f);
}

void idle(void)
{
    static GLdouble previous_time = glutGet(GLUT_ELAPSED_TIME);
    GLdouble current_time, d_t;
    // pega o tempo que passou do início da aplicação
    current_time = glutGet(GLUT_ELAPSED_TIME);
    // calcula o tempo decorrido desde de o última frame
    d_t = current_time - previous_time;
    // atualiza o tempo do último frame ocorrido
    previous_time = current_time;

    if (estado == -1)
    {
        quit();
        exit(0);
    }
    else if (estado > 0) return;

    // encontrei um problema ao rodar o código em casa onde a cada minuto,
    // mais ou menos, o d_t dá um salto enorme (>1000) e faz com que
    // o jogador tenha um movimento errático, quase se teleportando
    // não encontrei esse problema no labgrad, então imagino que seja algum problema
    // específico do wsl. então estou botando essa linha de código aqui
    // pra evitar esse problema
    if (d_t > 10.0f) d_t = 10.0f;
    else if (d_t < 0.0f) d_t = 0.0f;

    // bools que controlam a animação dos jogadores
    // caso os jogadores estejam se movimentando pra frente, trás ou girando
    // continua com a animação da perna, caso contrário, "reseta" a animação
    bool animacao_frente_1 = false;
    bool animacao_tras_1 = false;
    bool animacao_frente_2 = false;
    bool animacao_tras_2 = false;

    if (key_status[(int) 'a']) {
        if (!key_status[(int) 'd'])
        {
            animacao_frente_1 = true;
            j_1->Roda(OMEGA_JOGADOR, d_t);
        }
    }
    else if (key_status[(int) 'd'])
    {
        animacao_frente_1 = true;
        j_1->Roda(-OMEGA_JOGADOR, d_t);
    }

    if (key_status[(int) 'k'])
    {
        if (!key_status[231])
        {
            animacao_frente_2 = true;
            j_2->Roda(OMEGA_JOGADOR, d_t);
        }
    }
    else if (key_status[231])
    {
        animacao_frente_2 = true;
        j_2->Roda(-OMEGA_JOGADOR, d_t);
    }

    if (key_status[(int) 'w'])
    {
        if (!key_status[(int) 's'])
        {
            animacao_frente_1 = true;
            j_1->Move(V_JOGADOR, d_t);
        }
    }
    else if (key_status[(int) 's'])
    {
        animacao_tras_1 = true;
        j_1->Move(-V_JOGADOR, d_t);
    }

    if (key_status[(int) 'o'])
    {
        if (!key_status[(int) 'l'])
        {
            animacao_frente_2 = true;
            j_2->Move(V_JOGADOR, d_t);
        }
    }
    else if (key_status[(int) 'l'])
    {
        animacao_tras_2 = true;
        j_2->Move(-V_JOGADOR, d_t);
    }

    if (key_status[(int) '5'])
    {
        if (j_2->PodeAtirar())
        {
            GLfloat tiro_x = j_2->XPontaBraco();
            GLfloat tiro_y = j_2->YPontaBraco();
            GLfloat tiro_z = j_2->Altura();
            GLfloat tiro_tamanho = j_2->EspessuraBraco();
            GLfloat tiro_theta = j_2->ThetaBraco();

            tiros.emplace_back(tiro_x, tiro_y, tiro_z, tiro_tamanho, tiro_theta, 2);

            j_2->ResetaTimer();
        }
    }

    if (key_status[(int) 'x'])
    {
        j_1->Pula();
    }

    if (key_status[(int) 'o']) 
    {
        j_2->Pula();
    }

    if (key_status[(int) '1'])
    {
        j_1->DecrementaVida();
    }

    if (key_status[(int) '2'])
    {
        j_2->DecrementaVida();
    }

    for (auto& tiro : tiros) { tiro.Move(V_TIRO, d_t); }

    GLfloat theta_mouse = atan2(mouse_Y - j_1->YBaseBraco(), mouse_x - j_1->XBaseBraco()) * 180.0 / PI;
    j_1->RodaBracoMouse(OMEGA_BRACO, theta_mouse, d_t);

    if (key_status[(int) '4']) { if (!key_status[(int) '6']) j_2->RodaBraco(OMEGA_BRACO, d_t); }
    else if (key_status[(int) '6']) { j_2->RodaBraco(-OMEGA_BRACO, d_t); }

    if (animacao_tras_1) j_1->Animacao(-V_JOGADOR, d_t);
    else if (animacao_frente_1) j_1->Animacao(V_JOGADOR, d_t);
    else j_1->AnimacaoReset(V_JOGADOR, d_t);
    
    if (animacao_tras_2) j_2->Animacao(-V_JOGADOR, d_t);
    else if (animacao_frente_2) j_2->Animacao(V_JOGADOR, d_t);
    else j_2->AnimacaoReset(V_JOGADOR, d_t);

    j_1->DecrementaTimer(d_t);
    j_1->AtualizaFisica(1);

    j_2->DecrementaTimer(d_t);
    j_2->AtualizaFisica(1);

    arena->colisaoJogador(j_1, j_2);

    arena->colisaoTiro(tiros);

    int vidas_1 = j_1->colisaoTiro(tiros, 1);
    int vidas_2 = j_2->colisaoTiro(tiros, 2);

    if (vidas_1 == 0) {
        if (vidas_2 == 0) estado = 3;
        else estado = 2;

    } else if (vidas_2 == 0) estado = 1;
    
    glutPostRedisplay();
}

void init()
{
    resetKeyStatus();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glShadeModel (GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);

    
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    // glLightfv(GL_LIGHT0, GL_AMBIENT, luz_ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz_difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz_especular);
 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(fov, aspect_ratio, 1, 2000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// inicializa arena e jogadores
void inicializaObjetos()
{
    float arena_x = 0.0f;
    float arena_y = 0.0f;
    float arena_r = 0.0f;

    // procura o círculo da arena
    for (auto& c : circulos) {
        if (c.cor == "blue")
        {
            arena_x = c.x;
            arena_y = c.y;
            arena_r = c.raio;
            break;
        }
    }

    // modifica os valores dos círculos para centralizar a arena
    for (auto& c : circulos) {
        c.x -= arena_x;
        c.x *= std::min(viewing_height, viewing_width) / (2 * arena_r);
        c.y -= arena_y;
        c.y *= std::min(viewing_height, viewing_width) / (2 * arena_r);
        c.raio *= std::min(viewing_height, viewing_width) / (2 * arena_r);
    }

    // procura os valores iniciais dos jogadores
    float x_1 = 0.0f, y_1 = 0.0f, z_1 = 0.0f, r_1 = 0.0f;
    float x_2 = 0.0f, y_2 = 0.0f, z_2 = 0.0f, r_2 = 0.0f;

    for (auto& c : circulos) {
        if (c.cor == "red")   { x_1 = c.x;  y_1 = c.y;  r_1 = c.raio; }
        if (c.cor == "green") { x_2 = c.x;  y_2 = c.y;  r_2 = c.raio; }
        if (r_1 > 0 && r_2 > 0) break;
    }

    // ângulo inicial dos jogadores
    GLfloat theta_1 = normalizaAnguloGraus(atan2(y_2 - y_1, x_2 - x_1) * 180.0 / PI);
    GLfloat theta_2 = normalizaAnguloGraus(theta_1 + 180.0f);

    arena = new Arena(0.0f, 0.0f, 0.0f, std::min(viewing_width, viewing_height) / 2.0f, 0.0f, 0.0f, 1.0f);
    j_1 = new Jogador(x_1, y_1, z_1, r_1, 1.0f, 0.0f, 0.0f, theta_1, 3);
    j_2 = new Jogador(x_2, y_2, z_2, r_2, 0.0f, 1.0f, 0.0f, theta_2, 3);

    for (auto& c : circulos) {
        if (c.cor == "black") { arena->adicionaObstaculo(c.x, c.y, 0, c.raio); }
    }

    // inicia o jogo com o jogador 1 (controlado pelo mouse)
    // com o braço em direção ao jogador 2
    mouse_x = j_2->X();
    mouse_Y = j_2->Y();

    // seta as luzes dos personagens 
    SetaLuzPersonagens();
}
 
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("erro: passe somente um arquivo.svg como argumento\n");
        return 1;
    }

    circulos = LeSVG(argv[1]);

    // inicializa openGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
 
    // cria a janela
    glutInitWindowSize(width, height);
    glutInitWindowPosition(150, 50);
    glutCreateWindow("Tranformations 3D");

    // inicializa arena e jogadores
    inicializaObjetos();
 
    // define callbacks
    glutDisplayFunc(renderScene);
    glutKeyboardFunc(keyPress);
    glutIdleFunc(idle);
    glutKeyboardUpFunc(keyUp);

    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMove);
    glutPassiveMotionFunc(mouseMove);
    
    init();
 
    glutMainLoop();
 
    return 0;
}