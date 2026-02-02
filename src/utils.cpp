#include "../includes/utils.h"

float normalizaAnguloGraus(float angulo)
{
    while (angulo > 180.0f) angulo -= 360.0f;
    while (angulo <= -180.0f) angulo += 360.0f;
    return angulo;
}

float normalizaAnguloRadianos(float angulo)
{
    while (angulo >= 2.0f * PI) angulo -= 2.0f * PI;
    while (angulo < 0.0f) angulo += 2.0f * PI;
    return angulo;
}

float limite(float val, float a, float b)
{
    float limite_max = std::max(a, b);
    float limite_min = std::min(a, b);
    return (val > limite_max) ? limite_max : (val < limite_min) ? limite_min : val;
}

void DesenhaRectCentrado(GLfloat d_x, GLfloat d_y, GLfloat R, GLfloat G, GLfloat B)
{
    glColor3f(R, G, B);

    glBegin(GL_QUADS);

    glVertex2f(d_x / 2.0f, d_y / 2.0f);
    glVertex2f(-d_x / 2.0f, d_y / 2.0f);
    glVertex2f(-d_x / 2.0f, -d_y / 2.0f);
    glVertex2f(d_x / 2.0f, -d_y / 2.0f);

    glEnd();
}

void DesenhaRectXPos(GLfloat d_x, GLfloat d_y, GLfloat R, GLfloat G, GLfloat B)
{
    glColor3f(R, G, B);

    glBegin(GL_QUADS);

    glVertex2f(d_x, d_y / 2.0f);
    glVertex2f(0.0f, d_y / 2.0f);
    glVertex2f(0.0f, -d_y / 2.0f);
    glVertex2f(d_x, -d_y / 2.0f);

    glEnd();
}

void DesenhaRectYPos(GLfloat d_x, GLfloat d_y, GLfloat R, GLfloat G, GLfloat B)
{
    glColor3f(R, G, B);

    glBegin(GL_QUADS);

    glVertex2f(d_x / 2.0f, d_y);
    glVertex2f(-d_x / 2.0f, d_y);
    glVertex2f(-d_x / 2.0f, 0.0f);
    glVertex2f(d_x / 2.0f, 0.0f);

    glEnd();
}

void DesenhaRectPos(GLfloat d_x, GLfloat d_y, GLfloat R, GLfloat G, GLfloat B)
{
    glColor3f(R, G, B);

    glBegin(GL_QUADS);

    glVertex2f(d_x, d_y);
    glVertex2f(0.0f, d_y);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(d_x, 0.0f);

    glEnd();
}

void DesenhaCirc(GLfloat radius, GLfloat R, GLfloat G, GLfloat B, int detalhe)
{
    glColor3f(R, G, B);

    glPointSize(1.0f);

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < detalhe; i++) {
        glVertex2f(radius * cos(i * PI * 2 / detalhe), radius * sin(i * PI * 2 / detalhe));
    }
    glEnd();
}

void DesenhaElipse(GLfloat r_x, GLfloat r_y, GLfloat R, GLfloat G, GLfloat B, int detalhe)
{
    glColor3f(R, G, B);

    glPointSize(1.0f);

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < detalhe; i++) {
        glVertex2f(r_x * cos(i * PI * 2 / detalhe), r_y * sin(i * PI * 2 / detalhe));
    }
    glEnd();
}

void DesenhaXis(GLfloat d_x, GLfloat d_y, GLfloat R, GLfloat G, GLfloat B)
{
    // ◢◣◢◣
    // ◥◼◼◤
    // ◢◼◼◣
    // ◥◤◥◤

    glColor3f(R, G, B);

    glPointSize(1.0f);

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(d_x / 4.0f, 0.0f);
    glVertex2f(d_x / 2.0f, d_y / 4.0f);
    glVertex2f(d_x / 4.0f, d_y / 2.0f);
    glVertex2f(0.0f, d_y / 4.0f);
    glVertex2f(-d_x / 4.0f, d_y / 2.0f);
    glVertex2f(-d_x / 2.0f, d_y / 4.0f);
    glVertex2f(-d_x / 4.0f, 0.0f);
    glVertex2f(-d_x / 2.0f, -d_y / 4.0f);
    glVertex2f(-d_x / 4.0f, -d_y / 2.0f);
    glVertex2f(0.0f, -d_y / 4.0f);
    glVertex2f(d_x / 4.0f, -d_y / 2.0f);
    glVertex2f(d_x / 2.0f, -d_y / 4.0f);

    glEnd();
}

void DesenhaCoracao(GLfloat d_x, GLfloat d_y, GLfloat R, GLfloat G, GLfloat B, int detalhe)
{
    // https://www.desmos.com/calculator/b3qsae16mb
    // criei um conjunto de funções paramétricas para
    // modelar o coração de maneira escalável
    GLfloat t;

    glColor3f(R, G, B);

    glPointSize(1.0f);

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < detalhe; i++) {
        t = (float) i / detalhe;
        glVertex2f((d_x / 4.0f) * (-cos(PI * t) + 1), (d_y / 4.0f) * (sin(PI * t) + 1));
    }
    for (int i = 0; i < detalhe; i++) {
        t = 1.0f + (float) i / detalhe;
        glVertex2f((d_x / 4.0f) * (-cos(PI * t) + 1),
                   (d_y / 4.0f) * ((-t + 2) * (sin(PI * t) + 1) + (t - 1) * (-cos(PI * t) - 1)));
    }
    for (int i = 0; i < detalhe; i++) {
        t = 2.0f + (float) i / detalhe;
        glVertex2f((d_x / 4.0f) * (cos(PI * t) - 1),
                   (d_y / 4.0f) * ((-t + 3) * (-cos(PI * t) - 1) + (t - 2) * (-sin(PI * t) + 1)));
    }
    for (int i = 0; i < detalhe; i++) {
        t = 3.0f + (float) i / detalhe;
        glVertex2f((d_x / 4.0f) * (cos(PI * t) - 1), (d_y / 4.0f) * (-sin(PI * t) + 1));
    }
    glEnd();
}