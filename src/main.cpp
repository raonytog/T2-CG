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
#define DISTANCIA_CORACAO (GLfloat) 8

#define RIGHT_MINIMAP 1
#define LEFT_MINIMAP 0

//Key status
int key_status[256];

// MousePos
GLfloat mouse_x = 0.0f;
GLfloat mouse_Y = 0.0f;

// Controle de rotação lateral da câmera (Botão Direito)
bool botaoDireitoPressionado = false;
GLfloat mouse_x_anterior = 0.0f;
GLfloat rotacao_horizontal_camera = 0.0f; // -180 a 180

GLfloat mouse_y_anterior = 0.0f;
GLfloat rotacao_vertical_camera = 0.0f; // -90 a 90

// Window dimensions
const GLint width = 800;
const GLint splitted_width = width/2;
const GLint height = 700; // 500(tela normal)+200(visao permanente)

// dimensao das larguras de cada jogador
const GLint largura_inicial_j1 = 0;
const GLint largura_inicial_j2 = width/2;

// Viewing dimensions
const GLint viewing_width = 400;
const GLint viewing_height = 500;

const GLint height_permanent_view = 200;

const GLint fov = 60;
const GLfloat aspect_ratio = (GLfloat)viewing_width / (GLfloat)viewing_height;

// Luz Global
const GLfloat light_position[] = {400, 0, 600, 1.0f};

// Luz Jogador 1 (Vermelha)
GLfloat light_position_j_1[] = {0, 0, 0, 1.0f};
GLfloat light_difusa__j_1[] = {0, 0, 0, 1.0f};

// Luz Jogador 2 (Verde)
GLfloat light_position_j_2[] = {0, 0, 0, 1.0f};
GLfloat light_difusa__j_2[] = {0, 0, 0, 1.0f};

const GLfloat luz_difusa[]   = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat luz_especular[]= { 1.0f, 1.0f, 1.0f, 1.0f };

// Objetos
Arena *arena = nullptr;

Jogador *j_1 = nullptr;
Jogador *j_2 = nullptr;

std::vector<Tiro> tiros;

// Textura de fundo
GLuint BACKGROUND_TEXTURE = 0;
GLuint FLOOR_TEXTURE = 0;
GLuint WALL_TEXTURE = 0;

bool lighting_enabled = true;
GLfloat camera_zoom = 5.0f;

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

void DesenhaMiniMapa(Jogador *j1, Jogador *j2, int side) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();

        GLint mapSize = width / 4;
        switch (side)
        {
        case LEFT_MINIMAP: 
            glViewport(width/2 - mapSize, 0,
                       mapSize, mapSize);
            break;

        case RIGHT_MINIMAP:
            glViewport(width - mapSize, 0,
                       mapSize, mapSize);
            break;

        }

        float r = arena->Raio();
        gluOrtho2D(-r * 1.1, r * 1.1, -r * 1.1, r * 1.1); 

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();

            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);

            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINE_LOOP);
                for (int i = 0; i < 360; i++) {
                    float theta = i * M_PI / 180.0f;
                    glVertex2f(r * cos(theta), r * sin(theta));
                }
            glEnd();

            glColor3f(0.0f, 0.0f, 0.0f);
            glPointSize(8.0f);
            for (Obstaculo o : arena->getObstaculosVector())
            {
                glBegin(GL_POINTS);
                    glVertex2f(o.x, o.y);
                glEnd();
            }

            // j_1 verde
            glColor3f(0.0f, 1.0f, 0.0f);
            glPointSize(5.0f); // Ponto grande
            glBegin(GL_POINTS);
                    glVertex2f(j1->X(), j1->Y());
            glEnd();

            // j_2 vermelho
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_POINTS);
                glVertex2f(j2->X(), j2->Y());
            glEnd();
            
            if (lighting_enabled) glEnable(GL_LIGHTING);
            glEnable(GL_DEPTH_TEST);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void SetaLuzPersonagens(void)
{
    // --- j_1 ---
    light_position_j_1[0] = j_1->X();
    light_position_j_1[1] = j_1->Y();
    light_position_j_1[2] = j_1->Z() + j_1->Altura() + ALTURA_MEMBROS;
    light_position_j_1[3] = 1.0f;

    light_difusa__j_1[0] = j_1->R();
    light_difusa__j_1[1] = j_1->G();
    light_difusa__j_1[2] = j_1->B();
    light_difusa__j_1[3] = 1.0f;

    // --- j_2 ---
    light_position_j_2[0] = j_2->X();
    light_position_j_2[1] = j_2->Y();
    light_position_j_2[2] = j_2->Z() + j_2->Altura() + ALTURA_MEMBROS; 
    light_position_j_2[3] = 1.0f;

    light_difusa__j_2[0] = j_2->R();
    light_difusa__j_2[1] = j_2->G();
    light_difusa__j_2[2] = j_2->B();
    light_difusa__j_2[3] = 1.0f;
}

struct Circulo
{
    float x, y;
    float raio;
    std::string cor;
};

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
        const char* tag_nome = p_elemento->Name();

        if (std::strcmp(tag_nome, "circle") == 0)
        {
            Circulo circulo;
            
            p_elemento->QueryFloatAttribute("cx", &circulo.x);
            p_elemento->QueryFloatAttribute("cy", &circulo.y);
            p_elemento->QueryFloatAttribute("r", &circulo.raio);
            
            circulo.y = -circulo.y;
            const char* fill = p_elemento->Attribute("fill");
            if (fill) circulo.cor = fill;
            else      circulo.cor = "none";
            ret.push_back(circulo);
        }
        p_elemento = p_elemento->NextSiblingElement();
    }

    return ret;
}

void DesenhaCoracoes(int vidas_1, int vidas_2)
{
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

bool first_pov_cam()
{
    return key_status[(int) 'v'] == 0 && key_status[(int) 'b'] == 0;
}

bool gun_pov_cam()
{
    return key_status[(int) 'v'] == 1;
}

bool third_pov_cam()
{
    return key_status[(int) 'b'] == 1;
}

void ConfiguraCameraJogador(Jogador* p) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect_ratio, 0.1, 2000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    GLfloat x, y, z;                x = p->X(), y = p->Y(), z = p->Z();
    GLfloat rJogador, h;            rJogador = p->RaioColisao(), h = p->Altura();
    GLfloat rArena;                 rArena = arena->Raio();
    
    float theta_rad = (p->Theta() + rotacao_horizontal_camera) * M_PI / 180.0f;
    float phi_rad   = rotacao_vertical_camera * M_PI / 180.0f;

    float eyeX,  eyeY,  eyeZ;       eyeX  = eyeY  = eyeZ  = 0;
    float lookX, lookY, lookZ;      lookX = lookY = lookZ = 0;
    float upX,   upY,   upZ;        upX   = upY   = upZ   = 0;

    // posicao da camera
    if (first_pov_cam())
    {
        // posicao
        eyeX = x + rJogador;
        eyeY = y;
        eyeZ = z + h;

        // direcao
        lookX = eyeX + cos(theta_rad);
        lookY = eyeY + sin(theta_rad);
        lookZ = eyeZ; 
    }

    else if (gun_pov_cam()) 
    {
        eyeX = 0;
        eyeY = 0;
        eyeZ = 0;
    }

    else if (third_pov_cam())
    {
        float distanciaAtras = rJogador * camera_zoom;
        float alturaCamera = h * 2.0f;

        // Calcula a posição atrás do jogador baseada no ângulo atual
        // posicao
        eyeX = x - distanciaAtras * cos(theta_rad) * cos(phi_rad);
        eyeY = y - distanciaAtras * sin(theta_rad) * sin(phi_rad);
        eyeZ = z + alturaCamera; + distanciaAtras  * sin(phi_rad);

        float distanciaCentro = sqrt(eyeX*eyeX + eyeY*eyeY);
        if (distanciaCentro > arena->Raio()) {
            // Se a câmera sair do mapa, desativa esta visão ou 
            // fixa a câmera na borda da arena:
            float escala = rArena / distanciaCentro;
            eyeX *= escala;
            eyeY *= escala;
        }

        // direcao
        lookX = x;
        lookY = y;
        lookZ = z + h; 
    }

    // up vector
    upZ = 1.0f;

    // definicao da camera
    gluLookAt(eyeX, eyeY, eyeZ,
              lookX, lookY, lookZ,
              upX, upY, upZ);
}

void DesenhaHUD()
{
    glViewport(0, 0, width, height);

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
            // Reabilita luz apenas se o usuário não tiver desligado
            if (lighting_enabled) glEnable(GL_LIGHTING);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void ConfiguraLuzes() {
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // j_1 (vermelho)
    glLightfv(GL_LIGHT1, GL_POSITION, light_position_j_1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_difusa__j_1);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.75f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.005f); 
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);

    // j_2 (verde)
    glLightfv(GL_LIGHT2, GL_POSITION, light_position_j_2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_difusa__j_2);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.75f);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.005f); 
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0f);
}

void renderPlayerScene(Jogador *p1, Jogador *p2) 
{
    ConfiguraCameraJogador(p1);
    ConfiguraLuzes();
    arena->Desenha(WALL_TEXTURE, FLOOR_TEXTURE); 
    p1->Desenha();
    p2->Desenha();
}

void DesenhaVisaoPermanente(Jogador* p, int x_offset) {
    // Define o viewport na área superior (200px de altura)
    glViewport(x_offset, viewing_height, viewing_width, height_permanent_view);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Ajusta o aspect ratio para 400/200 = 2.0
    gluPerspective(fov, (GLfloat)viewing_width / (GLfloat)height_permanent_view, 0.1, 2000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Dados do jogador
    GLfloat x = p->X(), y = p->Y(), z = p->Z();
    GLfloat r = p->RaioColisao(), h = p->Altura();
    
    // Calcula ângulo em radianos para posicionar a câmara
    // Nota: A visão permanente geralmente não deve rodar com o rato (free look) se for "olhos do boneco",
    // mas se quiser que siga o rato, use (p->Theta() + rotacao_horizontal_camera)
    // Aqui usarei apenas p->Theta() para ser fiel à direção do corpo, mas pode alterar se preferir.
    float theta_rad = p->Theta() * M_PI / 180.0f;

    // Posiciona a câmara ligeiramente à frente do centro do jogador para não ver o interior da cabeça
    float offset_frente = r * 0.5f;

    float eyeX = x + offset_frente * cos(theta_rad);
    float eyeY = y + offset_frente * sin(theta_rad);
    float eyeZ = z + h * 0.9f; // Quase no topo da altura (olhos)

    // O ponto para onde olha é à frente da posição do olho
    float lookX = eyeX + cos(theta_rad);
    float lookY = eyeY + sin(theta_rad);
    float lookZ = eyeZ; // Olha em frente (horizonte)

    gluLookAt(eyeX, eyeY, eyeZ,
              lookX, lookY, lookZ,
              0, 0, 1); // Up vector é Z

    // Desenha a cena
    ConfiguraLuzes();
    arena->Desenha(WALL_TEXTURE, FLOOR_TEXTURE);
    
    // Desenha os jogadores (o próprio jogador pode não aparecer se a câmara estiver dentro dele, 
    // mas desenhamos para garantir que vemos o adversário)
    j_1->Desenha();
    j_2->Desenha();
}


void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DesenhaVisaoPermanente(j_1, largura_inicial_j1);
    DesenhaVisaoPermanente(j_2, largura_inicial_j2);

    glViewport(largura_inicial_j1, 0, splitted_width, viewing_height);
    renderPlayerScene(j_1, j_2);
    DesenhaMiniMapa(j_1, j_2, LEFT_MINIMAP);
    
    glViewport(splitted_width, 0, largura_inicial_j2, viewing_height);
    renderPlayerScene(j_2, j_1);
    DesenhaMiniMapa(j_2, j_1, RIGHT_MINIMAP);

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

    mouse_x = j_2->X();
    mouse_Y = j_2->Y();

    resetKeyStatus();
    key_status[(int) 'r'] = 1;
    
    // Configura as posições e cores das luzes novamente
    SetaLuzPersonagens();

    // reseta as cameras
    botaoDireitoPressionado = false;
    mouse_x_anterior = 0.0f;
    rotacao_horizontal_camera = 0.0f; // -180 a 180

    mouse_y_anterior = 0.0f;
    rotacao_vertical_camera = 0.0f; // -90 a 90
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
        case 'n':
        case 'N':
            lighting_enabled = !lighting_enabled;
            if (lighting_enabled) glEnable(GL_LIGHTING);
            else glDisable(GL_LIGHTING);
            break;
        case 'r':
        case 'R':
            if (key_status[(int) 'r'] == 0) 
            {
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
        case 231: 
        case 199: 
        case ';': 
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
            case 'X':
            key_status[(int) 'x'] = 1;
            break;
        case '.':
            key_status[(int) '.'] = 1;
            break;
        case '1': 
            key_status[(int) '1'] = 1;
            break;
        case '2': 
            key_status[(int) '2'] = 1;
            break;
        case 'v': 
        case 'V':
            key_status[(int) 'v'] = !key_status[(int) 'v'];
            key_status[(int) 'b'] = 0;
            break;
        case 'B':
        case 'b': 
            key_status[(int) 'b'] = !key_status[(int) 'b'];
            key_status[(int) 'v'] = 0;
            break;
        case '+':
        case '=':
            if (camera_zoom > 1.0f) camera_zoom -= 0.1;
            break;
        case '-':
            if (camera_zoom < 10.0f) camera_zoom += 0.1f;
            break;
    }
    glutPostRedisplay();
}

void keyUp(unsigned char key, int x, int y)
{
    if (key != 'v' && key != 'V' && key != 'b' && key != 'B') {
        key_status[(int)(key)] = 0;
    }

    if (key == ';') 
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
            GLfloat tiro_z = j_1->Altura();
            GLfloat tiro_tamanho = j_1->EspessuraBraco();
            GLfloat tiro_theta = j_1->ThetaBraco();

            tiros.emplace_back(tiro_x, tiro_y, tiro_z, tiro_tamanho, tiro_theta, 1);

            j_1->ResetaTimer();
        }
    }

    // Lógica para o Botão Direito
    if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            botaoDireitoPressionado = true;
            mouse_x_anterior = x;
            mouse_y_anterior = y;
        } else {
            botaoDireitoPressionado = false;
        }
    }
}

void mouseMove(int x, int y) {
    mouse_x = viewing_width * ((float) x / width - 0.5f);
    mouse_Y = -viewing_height * ((float) y / height - 0.5f);

    if (botaoDireitoPressionado && third_pov_cam()) {
        float sensibilidade = 0.5f;
        
        // Rotação Horizontal
        float deltaX = x - mouse_x_anterior;
        rotacao_horizontal_camera += deltaX * sensibilidade;
        if (rotacao_horizontal_camera > 180.0f) rotacao_horizontal_camera = 180.0f;
        if (rotacao_horizontal_camera < -180.0f) rotacao_horizontal_camera = -180.0f;

        // Rotação Vertical (Movimento do mouse em Y)
        float deltaY = y - mouse_y_anterior;
        rotacao_vertical_camera += deltaY * sensibilidade;

        if (rotacao_vertical_camera > 89.9f) rotacao_vertical_camera = 89.9f;
        if (rotacao_vertical_camera < -89.9f) rotacao_vertical_camera = -89.9f;

        mouse_x_anterior = x;
        mouse_y_anterior = y;
        
        glutPostRedisplay();
    }
}

void idle(void)
{
    static GLdouble previous_time = glutGet(GLUT_ELAPSED_TIME);
    GLdouble current_time, d_t;
    current_time = glutGet(GLUT_ELAPSED_TIME);
    d_t = current_time - previous_time;
    previous_time = current_time;

    if (estado == -1) { quit(); exit(0); }
    else if (estado > 0) return;

    if (d_t > 10.0f) d_t = 10.0f;
    else if (d_t < 0.0f) d_t = 0.0f;

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
        key_status[(int) '1'] = 0;
    }

    if (key_status[(int) '2'])
    {
        j_2->DecrementaVida();
        key_status[(int) '2'] = 0;
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
    glEnable(GL_LIGHT2); // Habilita a luz do segundo jogador
    glEnable(GL_DEPTH_TEST);

    
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz_difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz_especular);
 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(fov, aspect_ratio, 1, 2000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    FLOOR_TEXTURE = CarregaTextura("textures/floor.jpg");
    WALL_TEXTURE = CarregaTextura("textures/wall.jpg");
}

void inicializaObjetos()
{
    float arena_x = 0.0f;
    float arena_y = 0.0f;
    float arena_r = 0.0f;

    // arena
    for (auto& c : circulos) {
        if (c.cor == "blue")
        {
            arena_x = c.x;
            arena_y = c.y;
            arena_r = c.raio;
            break;
        }
    }

    for (auto& c : circulos) {
        c.x -= arena_x;
        c.x *= std::min(viewing_height, viewing_width) / (2 * arena_r);
        c.y -= arena_y;
        c.y *= std::min(viewing_height, viewing_width) / (2 * arena_r);
        c.raio *= std::min(viewing_height, viewing_width) / (2 * arena_r);
    }

    float x_1 = 0.0f, y_1 = 0.0f, z_1 = 0.0f, r_1 = 0.0f;
    float x_2 = 0.0f, y_2 = 0.0f, z_2 = 0.0f, r_2 = 0.0f;

    // jogadores
    for (auto& c : circulos) {
        if (c.cor == "red")   { x_1 = c.x;  y_1 = c.y;  r_1 = c.raio; }
        if (c.cor == "green") { x_2 = c.x;  y_2 = c.y;  r_2 = c.raio; }
        if (r_1 > 0 && r_2 > 0) break;
    }

    GLfloat theta_1 = normalizaAnguloGraus(atan2(y_2 - y_1, x_2 - x_1) * 180.0 / PI);
    GLfloat theta_2 = normalizaAnguloGraus(theta_1 + 180.0f);

    arena = new Arena(0.0f, 0.0f, 0.0f, std::min(viewing_width, viewing_height) / 2.0f, 0.0f, 0.0f, 1.0f);
    j_1 = new Jogador(x_1, y_1, z_1, r_1, 1.0f, 0.0f, 0.0f, theta_1, 3);
    j_2 = new Jogador(x_2, y_2, z_2, r_2, 0.0f, 1.0f, 0.0f, theta_2, 3);

    // objetos
    for (auto& c : circulos) {
        if (c.cor == "black") { arena->adicionaObstaculo(c.x, c.y, 0, c.raio); }
    }

    mouse_x = j_2->X();
    mouse_Y = j_2->Y();

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

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
 
    glutInitWindowSize(width, height);
    glutInitWindowPosition(150, 50);
    glutCreateWindow("Tranformations 3D");

    inicializaObjetos();
 
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