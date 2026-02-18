#include "../includes/tiro.h"

void Tiro::DesenhaTiro(GLfloat gX, GLfloat gY, GLfloat gZ, GLfloat raio)
{
    // printf("%lf %lf %lf %lf\n", gX, gY, gZ, raio);
    glPushMatrix();

    glTranslatef(gX, gY, gZ);

    DesenhaEsfera(raio, 1.0f, 1.0f, 1.0f, 20);

    glPopMatrix();
}

void Tiro::Move(GLfloat dist, GLfloat t_d)
{
    this->gX += dist * t_d * cos(-gTheta * M_PI / 180.0f) * cos(-gThetaVert * M_PI / 180.0f);
    this->gY += dist * t_d * sin(-gTheta * M_PI / 180.0f) * cos(-gThetaVert * M_PI / 180.0f);
    this->gZ += dist * t_d * sin(-gThetaVert * M_PI / 180.0f);
}

GLfloat Tiro::X()
{
    return this->gX;
}

GLfloat Tiro::Y()
{
    return this->gY;
}

GLfloat Tiro::Z()
{
    return this->gZ;
}

GLfloat Tiro::Raio()
{
    return this->raio;
}

int Tiro::IdJogador()
{
    return this->idJogador;
}