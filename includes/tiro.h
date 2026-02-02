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
#define TIRO_COMPRIMENTO 1.0f
#define TIRO_LARGURA 0.5f

class Tiro {
    GLfloat gX; 
    GLfloat gY;
    GLfloat dX;
    GLfloat dY;
    GLfloat gTheta;
    int idJogador; // id do jogador que atirou
private:
    void DesenhaTiro(GLfloat x, GLfloat y, GLfloat d_x, GLfloat d_y, GLfloat theta);

public:
    Tiro(GLfloat x, GLfloat y, GLfloat tamanho, GLfloat theta, int id_jogador)
    {
        this->gX = x; 
        this->gY = y;
        this->dX = tamanho * TIRO_COMPRIMENTO;
        this->dY = tamanho * TIRO_LARGURA;
        this->gTheta = theta;
        this->idJogador = id_jogador;
    };

    void Desenha()
    { 
        DesenhaTiro(gX, gY, dX, dY, gTheta);
    };

    void Move(GLfloat dist, GLfloat t_d);

    GLfloat X();
    GLfloat Y();
    int IdJogador();

    std::array<std::pair<GLfloat, GLfloat>, 4> Pontos();
};

#endif /* TIRO_H */