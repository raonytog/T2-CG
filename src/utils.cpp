#include "../includes/utils.h"
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "../includes/stb_image.h"

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

void DesenhaRectXPos(GLfloat d_x, GLfloat d_y, GLfloat d_z, GLfloat R, GLfloat G, GLfloat B)
{
    glColor3f(R, G, B);

    glPushMatrix();
        glTranslatef(d_x / 2.0f, 0.0f, d_z / 2.0f);
        glScalef(d_x, d_y, d_z);

        glutSolidCube(1.0f);
    glPopMatrix();
}

void DesenhaRectYPos(GLfloat d_x, GLfloat d_y, GLfloat d_z, GLfloat R, GLfloat G, GLfloat B)
{
    glColor3f(R, G, B);

    glBegin(GL_QUADS);
        glVertex2f(d_x / 2.0f, d_y);
        glVertex2f(-d_x / 2.0f, d_y);
        glVertex2f(-d_x / 2.0f, 0.0f);
        glVertex2f(d_x / 2.0f, 0.0f);
    glEnd();
}

void DesenhaRectPos(GLfloat d_x, GLfloat d_y, GLfloat d_z, GLfloat R, GLfloat G, GLfloat B)
{
    glColor3f(R, G, B);

    glBegin(GL_QUADS);
        glVertex2f(d_x, d_y);
        glVertex2f(0.0f, d_y);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(d_x, 0.0f);
    glEnd();
}

void DesenhaCilindro(GLfloat radius, GLfloat z, GLfloat R, GLfloat G, GLfloat B, int detalhe, GLuint textureID) 
{
    // Se passar textureID 0, desenha a cor sólida, senão desenha textura
    if (textureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1.0f, 1.0f, 1.0f); // Reset cor para branco para não tingir a textura
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(R, G, B);
    }

    float angle;
    
    // Paredes laterais
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= detalhe; i++) {
        angle = i * M_PI * 2 / detalhe;
        glNormal3f(cos(angle), sin(angle), 0.0f);
        
        // Mapeamento:
        float u = (float)i / detalhe; 
        
        if (textureID != 0) glTexCoord2f(u, 0.0f);
        glVertex3f(radius * cos(angle), radius * sin(angle), 0.0f);
        
        if (textureID != 0) glTexCoord2f(u, 1.0f);
        glVertex3f(radius * cos(angle), radius * sin(angle), z);
    }
    glEnd();

    // Tampa superior (Sem textura ou textura plana)
    glDisable(GL_TEXTURE_2D); // Desativa para a tampa (ou faça mapeamento planar se quiser)
    if (textureID != 0) glColor3f(R, G, B); // Usa a cor original para a tampa
    
    glBegin(GL_POLYGON);
    glNormal3f(0.0f, 0.0f, 1.0f);
    for (int i = 0; i < detalhe; i++) {
        angle = i * PI * 2 / detalhe;
        glVertex3f(radius * cos(angle), radius * sin(angle), z);
    }
    glEnd();
}

void DesenhaCirc(GLfloat radius, GLfloat R, GLfloat G, GLfloat B, int detalhe, GLuint textureID)
{
    // Se passar textureID 0, desenha a cor sólida, senão desenha textura
    if (textureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(R, G, B);
    }
    glPointSize(1.0f);

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= detalhe; i++) {
        float u = (float)i / detalhe; 
        if (textureID != 0) glTexCoord2f(u, 0.0f);
            glVertex3f(radius * cos(i * PI * 2 / detalhe), radius * sin(i * PI * 2 / detalhe), 0.0f);
        }
    glEnd();

    glDisable(GL_TEXTURE_2D);
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

// Em src/utils.cpp

GLuint CarregaTextura(const char* nomeArquivo)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Configurações de repetição e filtro
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Filtro MipMap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); 
    
    unsigned char *data = stbi_load(nomeArquivo, &width, &height, &nrChannels, 0);
    
    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        
        // --- CORREÇÃO AQUI ---
        // Em vez de usar glTexImage2D + glGenerateMipmap, usamos gluBuild2DMipmaps
        // Essa função cria a textura E gera os mipmaps automaticamente de forma compatível
        gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format, GL_UNSIGNED_BYTE, data);
        // ---------------------
    }
    else
    {
        printf("Falha ao carregar textura: %s\n", nomeArquivo);
    }
    
    stbi_image_free(data);
    return textureID;
}
void debug() 
{
    printf("--> debug\n");
}