#ifndef ARENA_H
#define	ARENA_H

#include "jogador.h"
#include "tiro.h"
#include "utils.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <math.h>
#include <string.h>
#include <vector>


#define DETALHE_ARENA 120
#define DETALHE_OBSTACULO 36
// #define ALTURA_OBSTACULO ALTURA_MEMBROS*3
// #define ALTURA_ARENA 5*ALTURA_OBSTACULO

#define FRACAO_JOGADOR_PISANDO_OPONENTE 0.9
// por exemplo, se o valor for 0.9, isso significa que se o jogador acima
// estiver com seu Z entre 90 e 100% da altura do jogador abaixo, ele será ajustado verticalmente
// e não horizontalmente, podendo ficar acima do outro jogador

struct Obstaculo
{
    float x, y, z;
    float raio;
    float altura;
};

class Arena {
    GLfloat gX, gY, gZ; 
    GLfloat R, G, B;
    GLfloat raio;
    GLfloat altura;
    std::vector<Obstaculo> obstaculos;
private:
    // ATUALIZADO: Recebe texParede e texPiso
    void DesenhaArena(GLfloat x, GLfloat y, GLfloat z, GLfloat raio, GLfloat altura,
                      GLfloat R, GLfloat G, GLfloat B, GLuint texParede, GLuint texPiso);

public:
    friend class Jogador;

    Arena(GLfloat x, GLfloat y, GLfloat z, GLfloat raio, GLfloat altura, GLfloat R, GLfloat G, GLfloat B)
    {
        this->gX = x;   this->gY = y;   this->gZ = z;
        this->R = R;    this->G = G;    this->B = B;
        this->raio = raio;
        this->altura = altura;
    };
    
    // ATUALIZADO: Recebe texParede e texPiso
    void Desenha(GLuint texParede, GLuint texPiso)
    { 
        DesenhaArena(gX, gY, gZ, raio, altura, R, G, B, texParede, texPiso);
    };

    void adicionaObstaculo(float x, float y, float z, float r)
    {
        this->obstaculos.emplace_back(Obstaculo{x, y, z, r, altura / 4.0f});
    }

    void DesenhaObstaculosMiniMapa();

    GLfloat X();
    GLfloat Y();
    GLfloat Z();
    GLfloat Raio();
    std::vector<Obstaculo> getObstaculosVector() 
    {
        return this->obstaculos;
    }

    void colisaoJogador(Jogador *j_1, Jogador *j_2);
    void colisaoTiro(std::vector<Tiro>& tiros);
};

#endif /* ARENA_H */