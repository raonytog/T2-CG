#include "../includes/tiro.h"

void Tiro::DesenhaTiro(GLfloat x, GLfloat y, GLfloat d_x, GLfloat d_y, GLfloat theta)
{
    glPushMatrix();

    glTranslatef(x, y, 0.0f);
    glRotatef(theta, 0.0f, 0.0f, 1.0f); 

    DesenhaRectCentrado(d_x, d_y, 1.0f, 1.0f, 1.0f);

    glPopMatrix();
}

void Tiro::Move(GLfloat dist, GLfloat t_d)
{
    this->gX += dist * t_d * cos(this->gTheta * PI / 180);
    this->gY += dist * t_d * sin(this->gTheta * PI / 180);
}

GLfloat Tiro::X()
{
    return this->gX;
}

GLfloat Tiro::Y()
{
    return this->gY;
}

int Tiro::IdJogador()
{
    return this->idJogador;
}

std::array<std::pair<GLfloat, GLfloat>, 4> Tiro::Pontos()
{
    std::array<std::pair<GLfloat, GLfloat>, 4> pontos;

    pontos[0] = {this->gX + this->dX * sin(gTheta * 180.0f / PI) / 2.0f,
                 this->gY + this->dY * cos(gTheta * 180.0f / PI) / 2.0f};
    pontos[1] = {this->gX + this->dX * sin(gTheta * 180.0f / PI) / 2.0f,
                 this->gY - this->dY * cos(gTheta * 180.0f / PI) / 2.0f};
    pontos[2] = {this->gX - this->dX * sin(gTheta * 180.0f / PI) / 2.0f,
                 this->gY - this->dY * cos(gTheta * 180.0f / PI) / 2.0f};
    pontos[3] = {this->gX - this->dX * sin(gTheta * 180.0f / PI) / 2.0f,
                 this->gY + this->dY * cos(gTheta * 180.0f / PI) / 2.0f};

    return pontos;
}