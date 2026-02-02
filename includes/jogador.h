#ifndef JOGADOR_H
#define	JOGADOR_H

#include "tiro.h"
#include "utils.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <algorithm>
#include <math.h>
#include <stdio.h> // debug
#include <string.h>
#include <utility>
#include <vector>

// todas as distâncias a seguir são proporcionais ao raio da cabeça
#define DETALHE_CABECA 36
#define DETALHE_OMBRO 36

#define PERNA_DISTANCIA 0.5f // distância do centro da cabeça, não entre uma perna e outra
#define PERNA_COMPRIMENTO 1.6f
#define PERNA_ESPESSURA 0.75f

#define BRACO_DISTANCIA 1.3f
#define BRACO_COMPRIMENTO 1.3f
#define BRACO_ESPESSURA 0.5f

#define OMBRO_LONGITUDINAL 0.6f
#define OMBRO_LATERAL 1.6f

#define DIST_COLISAO 1.0f

#define TIMER_TIRO 500.0f // ms

class Jogador {
    GLfloat gX; 
    GLfloat gY;
    GLfloat raio;
    GLfloat R;
    GLfloat G;
    GLfloat B;
    GLfloat animacao;
    GLfloat gTheta;
    GLfloat gThetaBraco;
    int vidas;
    GLfloat timerTiro;
private:
    void DesenhaBraco(GLfloat raio, GLfloat R, GLfloat G, GLfloat B, GLfloat theta_braco);
    void DesenhaPerna(GLfloat raio, GLfloat animacao);
    void DesenhaJogador(GLfloat x, GLfloat y, GLfloat raio,
                        GLfloat R, GLfloat G, GLfloat B,
                        GLfloat animacao, GLfloat theta, GLfloat theta_braco, int vidas);
    void DesenhaSuperior(GLfloat x, GLfloat y, GLfloat raio,
                         GLfloat R, GLfloat G, GLfloat B,
                         GLfloat animacao, GLfloat theta, GLfloat theta_braco, int vidas);
    void DesenhaInferior(GLfloat x, GLfloat y, GLfloat raio,
                         GLfloat R, GLfloat G, GLfloat B,
                         GLfloat animacao, GLfloat theta, GLfloat theta_braco, int vidas);

public:
    Jogador(GLfloat x, GLfloat y, GLfloat raio,
            GLfloat R, GLfloat G, GLfloat B, GLfloat theta, int vidas)
    {
        this->gX = x; 
        this->gY = y;
        this->raio = raio;
        this->R = R;
        this->G = G;
        this->B = B;
        this->animacao = 0.0f;
        this->gTheta = theta;
        this->gThetaBraco = 0.0f;
        this->vidas = vidas;
        this->timerTiro = 0.0f;
    };

    void Desenha()
    { 
        DesenhaJogador(gX, gY, raio, R, G, B, animacao, gTheta, gThetaBraco, vidas);
    };
    void DesenhaParteSuperior()
    { 
        DesenhaSuperior(gX, gY, raio, R, G, B, animacao, gTheta, gThetaBraco, vidas);
    };
    void DesenhaParteInferior()
    { 
        DesenhaInferior(gX, gY, raio, R, G, B, animacao, gTheta, gThetaBraco, vidas);
    };

    void Move(GLfloat dist, GLfloat t_d);
    void Roda(GLfloat d_theta, GLfloat t_d);
    void Animacao(GLfloat dist, GLfloat t_d);
    void AnimacaoReset(GLfloat dist, GLfloat t_d);
    void RodaBraco(GLfloat d_theta_braco, GLfloat t_d);
    void RodaBracoMouse(GLfloat d_theta_braco, GLfloat theta_alvo, GLfloat t_d);

    GLfloat X();
    GLfloat Y();
    void MoveX(GLfloat dX);
    void MoveY(GLfloat dX);

    GLfloat EspessuraBraco();
    GLfloat RaioColisao();

    GLfloat XBaseBraco();
    GLfloat YBaseBraco();
    GLfloat XPontaBraco();
    GLfloat YPontaBraco();
    GLfloat Theta();
    GLfloat ThetaBraco();
    int Vidas();

    bool PodeAtirar();
    void DecrementaTimer(GLfloat d_t);
    void ResetaTimer();

    int colisaoTiro(std::vector<Tiro>& tiros, int id_jogador);
};

#endif /* JOGADOR_H */