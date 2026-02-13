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
#define ALTURA_OBSTACULO ALTURA_MEMBROS*3

struct Obstaculo
{
    float x, y, z;
    float raio;
};

class Arena {
    GLfloat gX, gY, gZ; 
    GLfloat R, G, B;
    GLfloat raio;
    std::vector<Obstaculo> obstaculos;
private:
    void DesenhaArena(GLfloat x, GLfloat y, GLfloat z, GLfloat raio,
                      GLfloat R, GLfloat G, GLfloat B);

public:
    friend class Jogador;

    Arena(GLfloat x, GLfloat y, GLfloat z, GLfloat raio, GLfloat R, GLfloat G, GLfloat B)
    {
        this->gX = x;   this->gY = y;   this->gZ = z;
        this->R = R;    this->G = G;    this->B = B;
        this->raio = raio;
    };
    void Desenha()
    { 
        DesenhaArena(gX, gY, gZ, raio, R, G, B);
    };

    void adicionaObstaculo(float x, float y, float z, float r)
    {
        this->obstaculos.emplace_back(Obstaculo{x, y, z, r});
    }

    GLfloat X();
    GLfloat Y();
    GLfloat Z();
    GLfloat Raio();

    void colisaoJogador(Jogador *j_1, Jogador *j_2);
    void colisaoTiro(std::vector<Tiro>& tiros);
};

#endif /* ARENA_H */