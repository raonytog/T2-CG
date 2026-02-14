#ifndef FORMAS_H
#define	FORMAS_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>

#include <math.h>

#define ARENA 120
#define OBSTACULO 36

#define PI (float) M_PI

float normalizaAnguloGraus(float angulo);
float normalizaAnguloRadianos(float angulo);
float limite(float val, float a, float b);

void DesenhaRectCentrado(GLfloat d_x, GLfloat d_y, GLfloat R, GLfloat G, GLfloat B);
void DesenhaRectXPos(GLfloat d_x, GLfloat d_y, GLfloat d_z, GLfloat R, GLfloat G, GLfloat B);
void DesenhaRectYPos(GLfloat d_x, GLfloat d_y, GLfloat d_z,  GLfloat R, GLfloat G, GLfloat B);
void DesenhaRectPos(GLfloat d_x, GLfloat d_y, GLfloat d_z,  GLfloat R, GLfloat G, GLfloat B);
void DesenhaCirc(GLfloat radius, GLfloat R, GLfloat G, GLfloat B, int detalhe, GLuint textureID);
void DesenhaCilindro(GLfloat radius, GLfloat z, GLfloat R, GLfloat G, GLfloat B, int detalhe, GLuint textureID);
void DesenhaElipse(GLfloat r_x, GLfloat r_y, GLfloat R, GLfloat G, GLfloat B, int detalhe);
void DesenhaXis(GLfloat d_x, GLfloat d_y, GLfloat R, GLfloat G, GLfloat B);
void DesenhaCoracao(GLfloat d_x, GLfloat d_y, GLfloat R, GLfloat G, GLfloat B, int detalhe);

GLuint CarregaTextura(const char* nomeArquivo);
void debug();

#endif /* FORMAS_H */