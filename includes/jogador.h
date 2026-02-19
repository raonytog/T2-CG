#ifndef JOGADOR_H
#define	JOGADOR_H

#include "tiro.h"
#include "utils.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <algorithm>
#include <math.h>
#include <stdio.h> // debug
#include <string.h>
#include <utility>
#include <vector>

// todas as distâncias a seguir são proporcionais ao raio da cabeça
#define DETALHE_CABECA 36
#define DETALHE_OMBRO 36

#define ESCALA 1.0f

#define SEGMENTO_PERNA_DISTANCIA 0.8f // distância do centro da base da perna ao eixo z, não entre uma perna e outra
#define SEGMENTO_PERNA_ALTURA 1.0f
#define SEGMENTO_PERNA_ESPESSURA 0.2f // distância do centro do braço à ponta (raio do cilindro inscrito no prisma retangular)

#define BRACO_DISTANCIA 0.8f // distância do centro da base do braço ao eixo z
#define BRACO_COMPRIMENTO 2.0f
#define BRACO_ESPESSURA 0.2f // distância do centro do braço à ponta (raio do cilindro inscrito no prisma retangular)

#define OMBRO_ALTURA 4.0f
#define OMBRO_LONGITUDINAL 0.2f // distância da ponta longitudinal do ombro ao eixo z
#define OMBRO_LATERAL 1.0f // distância da ponta lateral do ombro ao eixo z

#define TRONCO_ALTURA 2.0f

#define CABECA_ALTURA 4.8f // centro da cabeça
#define CABECA_RAIO 0.8f // raio da cabeça em relação ao raio limite do jogador

#define ALTURA 5.6f

#define DIST_COLISAO 1.0f

#define TIMER_TIRO 500.0f // ms

#define GRAVIDADE 0.0005f
#define VELOCIDADE_PULO 0.1f

class Jogador {
    GLfloat gX, gY, gZ;
    GLfloat raio, gVelZ;
    GLint no_ar, pulando, pisando_oponente;
    GLfloat gR, gG, gB;
    GLfloat animacao;
    GLfloat gTheta;
    GLfloat gThetaBraco;
    GLfloat gThetaBracoVert;
    int vidas;
    GLfloat timerTiro;
private:
    void DesenhaBraco(GLfloat raio, GLfloat R, GLfloat G, GLfloat B, GLfloat theta_braco, GLfloat theta_braco_vert);
    void DesenhaPerna(GLfloat raio, GLfloat animacao, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaCorpo(GLfloat R, GLfloat G, GLfloat B);
    void DesenhaCabeca(GLfloat raio, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaJogador(GLfloat x, GLfloat y, GLfloat z, GLfloat raio,
                        GLfloat R, GLfloat G, GLfloat B,
                        GLfloat animacao, GLfloat theta,
                        GLfloat theta_braco, GLfloat theta_braco_vert, int vidas);
    // void DesenhaSuperior(GLfloat x, GLfloat y, GLfloat z, GLfloat raio,
    //                      GLfloat R, GLfloat G, GLfloat B,
    //                      GLfloat animacao, GLfloat theta, GLfloat theta_braco, int vidas);
    // void DesenhaInferior(GLfloat x, GLfloat y, GLfloat z, GLfloat raio,
    //                      GLfloat R, GLfloat G, GLfloat B,
    //                      GLfloat animacao, GLfloat theta, GLfloat theta_braco, int vidas);

public:
    Jogador(GLfloat x, GLfloat y, GLfloat z, GLfloat raio,
            GLfloat R, GLfloat G, GLfloat B, GLfloat theta, int vidas)
    {
        this->gX = x;   this->gY = y;   this->gZ = z;
        this->gR = R;    this->gG = G;    this->gB = B;
        this->raio = raio * ESCALA;
        this->animacao = 0.0f;
        this->gTheta = theta;
        this->gThetaBraco = 0.0f;
        this->gThetaBracoVert = 0.0f;
        this->vidas = vidas;
        this->timerTiro = 0.0f;
        this->gVelZ = 0.0f;
        this->no_ar = 0;
        this->pulando = 0;
        this->pisando_oponente = 0;
    };

    void Desenha()
    { 
        DesenhaJogador(gX, gY, gZ, raio, gR, gG, gB, animacao, gTheta, gThetaBraco, gThetaBracoVert, vidas);
    };
    // void DesenhaParteSuperior()
    // { 
    //     DesenhaSuperior(gX, gY, gZ, raio, gR, gG, gB, animacao, gTheta, gThetaBraco, vidas);
    // };
    // void DesenhaParteInferior()
    // { 
    //     DesenhaInferior(gX, gY, gZ, raio, gR, gG, gB, animacao, gTheta, gThetaBraco, vidas);
    // };

    void Move(GLfloat dist, GLfloat t_d);
    void Roda(GLfloat d_theta, GLfloat t_d);
    void Animacao(GLfloat dist, GLfloat t_d);
    void AnimacaoReset(GLfloat dist, GLfloat t_d);
    void RodaBraco(GLfloat d_theta_braco, GLfloat t_d);
    void RodaBracoVert(GLfloat d_theta_braco_vert, GLfloat t_d);
    void RodaBracoMouse(GLfloat d_theta_braco, GLfloat theta_alvo_horiz,
                        GLfloat theta_alvo_vert, GLfloat t_d);
    void AjustaBraco();

    GLfloat X()    { return this->gX;    }
    GLfloat Y()    { return this->gY;    }
    GLfloat Z()    { return this->gZ;    }
    GLfloat Raio() { return this->raio;  }
    GLfloat R()    { return this->gR;    }
    GLfloat G()    { return this->gG;    }
    GLfloat B()    { return this->gB;    }
    GLfloat velZ() { return this->gVelZ; }
    GLfloat NoAr() { return this->no_ar; }

    void MoveX(GLfloat dX);
    void MoveY(GLfloat dX);
    // void MoveZ(GLfloat dX);
    void Pula();
    void ParaPulo();
    void Pousa(GLfloat z);
    void AtualizaFisica(GLfloat t_d);

    GLfloat EspessuraBraco();
    GLfloat RaioColisao();

    GLfloat XBaseBraco();
    GLfloat YBaseBraco();

    GLfloat XPontaBraco();
    GLfloat YPontaBraco();
    GLfloat ZPontaBraco();

    GLfloat EyeR() { return this->raio * CABECA_RAIO; }
    GLfloat EyeZ() { return this->raio * CABECA_ALTURA; }

    GLfloat Theta();
    GLfloat ThetaBraco();
    GLfloat ThetaBracoVert();

    GLfloat Altura();
    int Vidas();

    void DecrementaVida();
    bool PodeAtirar();
    void DecrementaTimer(GLfloat d_t);
    void ResetaTimer();

    int colisaoTiro(std::vector<Tiro>& tiros, int id_jogador);
};

#endif /* JOGADOR_H */