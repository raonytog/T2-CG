#ifndef TIRO_H
#define	TIRO_H

#include "utils.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <array>
#include <math.h>
#include <stdio.h> // debug
#include <utility>

// definidos em razão do tamanho inserido na função
#define TIRO_RAIO 1.0f

class Tiro {
    GLfloat gX, gY, gZ;
    GLfloat raio;
    GLfloat gTheta, gThetaVert;
    int idJogador; // id do jogador que atirou
private:
    void DesenhaTiro(GLfloat gX, GLfloat gY, GLfloat gZ, GLfloat raio);

public:
    Tiro(GLfloat x, GLfloat y, GLfloat z, GLfloat tamanho, GLfloat theta, GLfloat theta_vert, int id_jogador)
    {
        // printf("%lf %lf %lf\n", x, y, z);
        this->gX = x;   this->gY = y;   this->gZ = z;
        this->raio = tamanho * TIRO_RAIO;
        this->gTheta = theta;   this->gThetaVert = theta_vert;
        this->idJogador = id_jogador;
    };

    void Desenha()
    { 
        DesenhaTiro(gX, gY, gZ, raio);
    };

    void Move(GLfloat dist, GLfloat t_d);

    GLfloat X();
    GLfloat Y();
    GLfloat Z();
    GLfloat Raio();
    int IdJogador();
};

#endif /* TIRO_H */