#include "../includes/jogador.h"

void Jogador::DesenhaBraco(GLfloat raio, GLfloat R, GLfloat G, GLfloat B, GLfloat theta_braco, GLfloat theta_braco_vert)
{
    glPushMatrix();

        glTranslatef(0.0f, raio * BRACO_DISTANCIA, raio * (OMBRO_ALTURA - BRACO_ESPESSURA));

        glRotatef(-theta_braco, 0.0f, 0.0f, 1.0f);
        glRotatef(theta_braco_vert, 0.0f, 1.0f, 0.0f);

        DesenhaPrismaRetangular(raio * BRACO_COMPRIMENTO, 0.0f,
                                raio * BRACO_ESPESSURA, raio * -BRACO_ESPESSURA,
                                raio * BRACO_ESPESSURA, raio * -BRACO_ESPESSURA,
                                R, G, B);
    glPopMatrix();
}

GLfloat pos_sin (GLfloat x)
{
    x = sin(x);
    if (x < 0.0f) x = 0.0f;
    return x;
}

// animacao perna
// 0  1 2  3   4
// | /  |  \   |
// | |  |   \  |

void Jogador::DesenhaPerna(GLfloat raio, GLfloat animacao, GLfloat R, GLfloat G, GLfloat B)
{
    glPushMatrix();
        // aumenta z e roda 180 para que a perna gire em torno do eixo do tronco
        glTranslatef(0.0f, raio * SEGMENTO_PERNA_DISTANCIA, raio * TRONCO_ALTURA);
        glRotatef(180.0f + 45.0f * sin(this->animacao), 0.0f, 1.0f, 0.0f);
        DesenhaPrismaRetangular(raio * SEGMENTO_PERNA_ESPESSURA, raio * -SEGMENTO_PERNA_ESPESSURA,
                                raio * SEGMENTO_PERNA_ESPESSURA, raio * -SEGMENTO_PERNA_ESPESSURA,
                                0.0f, raio * SEGMENTO_PERNA_ALTURA, R, G, B);
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, raio * SEGMENTO_PERNA_ALTURA);
            glRotatef(45.0f * pos_sin(this->animacao + M_PI), 0.0f, 1.0f, 0.0f);
            DesenhaPrismaRetangular(raio * SEGMENTO_PERNA_ESPESSURA, raio * -SEGMENTO_PERNA_ESPESSURA,
                                    raio * SEGMENTO_PERNA_ESPESSURA, raio * -SEGMENTO_PERNA_ESPESSURA,
                                    0.0f, raio * SEGMENTO_PERNA_ALTURA, R, G, B);
        glPopMatrix();
    glPopMatrix();

    glPushMatrix();
        // aumenta z e roda 180 para que a perna gire em torno do eixo do tronco
        glTranslatef(0.0f, raio * -SEGMENTO_PERNA_DISTANCIA, raio * TRONCO_ALTURA);
        glRotatef(180.0f + 45.0f * sin(this->animacao + M_PI), 0.0f, 1.0f, 0.0f);
        DesenhaPrismaRetangular(raio * SEGMENTO_PERNA_ESPESSURA, raio * -SEGMENTO_PERNA_ESPESSURA,
                                raio * SEGMENTO_PERNA_ESPESSURA, raio * -SEGMENTO_PERNA_ESPESSURA,
                                0.0f, raio * SEGMENTO_PERNA_ALTURA, R, G, B);
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, raio * SEGMENTO_PERNA_ALTURA);
            glRotatef(45.0f * pos_sin(this->animacao), 0.0f, 1.0f, 0.0f);
            DesenhaPrismaRetangular(raio * SEGMENTO_PERNA_ESPESSURA, raio * -SEGMENTO_PERNA_ESPESSURA,
                                    raio * SEGMENTO_PERNA_ESPESSURA, raio * -SEGMENTO_PERNA_ESPESSURA,
                                    0.0f, raio * SEGMENTO_PERNA_ALTURA, R, G, B);
        glPopMatrix();
    glPopMatrix();
}

void Jogador::DesenhaCorpo(GLfloat R, GLfloat G, GLfloat B) {
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, raio * TRONCO_ALTURA);
        DesenhaPrismaRetangular(raio * OMBRO_LONGITUDINAL, raio * -OMBRO_LONGITUDINAL,
                                raio * OMBRO_LATERAL, raio * -OMBRO_LATERAL,
                                raio * (OMBRO_ALTURA - TRONCO_ALTURA), 0.0f, R, G, B);
    glPopMatrix();
}

void Jogador::DesenhaCabeca(GLfloat raio, GLfloat R, GLfloat G, GLfloat B) {
    glColor3d(R, G, B);
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, raio * CABECA_ALTURA);
        glutSolidSphere(raio * CABECA_RAIO, 20, 20);
    glPopMatrix();
}

void Jogador::DesenhaJogador(GLfloat x, GLfloat y, GLfloat z, GLfloat unidade,
                            GLfloat R, GLfloat G, GLfloat B,
                            GLfloat animacao, GLfloat theta,
                            GLfloat theta_braco, GLfloat theta_braco_vert, int vidas)
{
    glPushMatrix();

    glTranslatef(x, y, z);

    if (vidas > 0)
    {
        glRotatef(theta, 0.0f, 0.0f, 1.0f);

        DesenhaPerna(raio, animacao, R, G, B);
        DesenhaBraco(raio, R, G, B, theta_braco, theta_braco_vert);
        DesenhaCorpo(R, G, B);
        DesenhaCabeca(raio, R, G, B);
        // DesenhaElipse(raio * OMBRO_LONGITUDINAL, raio * OMBRO_LATERAL, R, G, B, DETALHE_OMBRO);
        // DesenhaCirc(raio, R, G, B, DETALHE_CABECA);
    } else
        DesenhaXis(raio, raio, R, G, B);

    glPopMatrix();
}

void Jogador::Move(GLfloat dist, GLfloat t_d)
{
    this->gX += dist * t_d * cos(this->gTheta * PI / 180);
    this->gY += dist * t_d * sin(this->gTheta * PI / 180);
}

void Jogador::Pula() 
{
    if (!this->no_ar)
    {
        this->gVelZ = raio * VELOCIDADE_PULO;
        this->no_ar = 1;
        this->pulando = 1;
    }
}

void Jogador::ParaPulo() 
{
    // faz com que o jogador começe a cair no ar (a tecla de pulo não está mais sendo pressionada)
    this->pulando = 0;
}

void Jogador::Pousa(GLfloat z)
{
    this->gZ = z;
    this->gVelZ = 0.0f;
    this->no_ar = 0;
}

void Jogador::AtualizaFisica(GLfloat t_d)
{
    // Aplica gravidade na velocidade
    this->gVelZ -= raio * GRAVIDADE * t_d;

    // Aplica velocidade na posição Z
    this->gZ += this->gVelZ * t_d;

    // Colisão com o chão
    if (this->gZ < 0.0f)
    {
        this->gZ = 0.0f;
        this->gVelZ = 0.0f;
        this->no_ar = 0;
    }

    // se parou de pular (soltou a tecla), começa a cair
    if (!this->pulando && this->gVelZ > 0.0f)
    {
        this->gVelZ = 0.00f;
    }
}

void Jogador::Roda(GLfloat d_theta, GLfloat t_d)
{
    this->gTheta += d_theta * t_d;
    this->gTheta = normalizaAnguloGraus(this->gTheta);
}

void Jogador::Animacao(GLfloat dist, GLfloat t_d)
{
    // divide por pi e pelo comprimento da perna
    // para que o movimento da perna seja continuo em função
    // à velocidade do jogador
    this->animacao += dist * t_d / (2 * PI * SEGMENTO_PERNA_ALTURA);
    this->animacao = normalizaAnguloRadianos(this->animacao);
}

void Jogador::AnimacaoReset(GLfloat dist, GLfloat t_d)
{
    // volta a uma posição neutra quando o jogador não está andando ou girando
    float abs_diff = dist * t_d / (2.0f * PI * SEGMENTO_PERNA_ALTURA);
    float delta;

    if (this->animacao < PI)
    {
        if (this->animacao < PI / 2.0f)
            delta = -std::min(this->animacao, abs_diff);
        else
            delta = std::min(PI - this->animacao, abs_diff);
    }
    else if (this->animacao > PI)
    {
        if (this->animacao < 3.0f * PI / 2.0f)
            delta = -std::min(this->animacao - PI, abs_diff);
        else
            delta = std::min(2.0f * PI - this->animacao, abs_diff);
    }

    this->animacao += delta;
}

void Jogador::RodaBraco(GLfloat d_theta_braco, GLfloat t_d)
{
    this->gThetaBraco += d_theta_braco * t_d;
    this->gThetaBraco = limite(this->gThetaBraco, 45.0f, -45.0f);

    this->AjustaBraco();
}

void Jogador::RodaBracoVert(GLfloat d_theta_braco_vert, GLfloat t_d)
{
    this->gThetaBracoVert += d_theta_braco_vert * t_d;
    this->gThetaBracoVert = limite(this->gThetaBracoVert, 45.0f, -45.0f);
    
    this->AjustaBraco();
}

void Jogador::AjustaBraco()
{
    // mantém a orientação do braço em cone
    GLfloat x = this->gThetaBraco / 45.0f;
    GLfloat y = this->gThetaBracoVert / 45.0f;
    GLfloat dist_centro = sqrt(x * x + y * y);

    if (dist_centro > 1.0f)
    {
        this->gThetaBraco /= dist_centro;
        this->gThetaBracoVert /= dist_centro;
    }
}

void Jogador::RodaBracoMouse(GLfloat d_theta_braco, GLfloat theta_alvo_horiz,
                             GLfloat theta_alvo_vert, GLfloat t_d)
{
    GLfloat d_horiz = theta_alvo_horiz - this->gThetaBraco;
    GLfloat d_vert = theta_alvo_vert - this->gThetaBracoVert;
    GLfloat d_total = sqrt(d_horiz * d_horiz + d_vert * d_vert);

    if (d_total == 0.0f) return;

    GLfloat comp_horiz = d_horiz / d_total;
    GLfloat comp_vert = d_vert / d_total;

    this->gThetaBraco += d_theta_braco * t_d * comp_horiz;
    if (theta_alvo_horiz > 0.0f)
    {
        if (this->gThetaBraco > theta_alvo_horiz) this->gThetaBraco = theta_alvo_horiz;
    } else {
        if (this->gThetaBraco < theta_alvo_horiz) this->gThetaBraco = theta_alvo_horiz;
    }

    this->gThetaBracoVert += d_theta_braco * t_d * comp_vert;
    if (theta_alvo_vert > 0.0f)
    {
        if (this->gThetaBracoVert > theta_alvo_vert) this->gThetaBracoVert = theta_alvo_vert;
    } else {
        if (this->gThetaBracoVert < theta_alvo_vert) this->gThetaBracoVert = theta_alvo_vert;
    }
}

void Jogador::MoveX(GLfloat dX)
{
    this->gX += dX;
}

void Jogador::MoveY(GLfloat dY)
{
    this->gY += dY;
}

GLfloat Jogador::EspessuraBraco()
{
    return this->raio * BRACO_ESPESSURA;
}

GLfloat Jogador::RaioColisao()
{
    return this->raio * DIST_COLISAO;
}

// GLfloat Jogador::XBaseBraco()
// {
//     return this->gX - this->raio * BRACO_DISTANCIA * sin(this->gTheta * PI / 180);
// }

// GLfloat Jogador::YBaseBraco()
// {
//     return this->gY + this->raio * BRACO_DISTANCIA * cos(this->gTheta * PI / 180);
// }

// GLfloat Jogador::XPontaBraco()
// {
//     return this->gX - this->raio * BRACO_DISTANCIA * sin(this->gTheta * PI / 180)
//     + this->raio * BRACO_COMPRIMENTO * cos((this->gTheta + this->gThetaBraco) * PI / 180);
// }

// GLfloat Jogador::YPontaBraco()
// {
//     return this->gY + this->raio * BRACO_DISTANCIA * cos(this->gTheta * PI / 180)
//     + this->raio * BRACO_COMPRIMENTO * sin((this->gTheta + this->gThetaBraco) * PI / 180);
// }

GLfloat Jogador::Theta()
{
    return this->gTheta;
}

GLfloat Jogador::ThetaBraco()
{
    return normalizaAnguloGraus(this->gThetaBraco - this->gTheta);
}

GLfloat Jogador::ThetaBracoVert()
{
    return normalizaAnguloGraus(this->gThetaBracoVert);
}

GLfloat Jogador::Altura()
{
    return raio * ALTURA;
}

int Jogador::Vidas()
{
    return this->vidas;
}

void Jogador::DecrementaVida() {
    this->vidas--;
}

bool Jogador::PodeAtirar()
{
    return this->timerTiro == 0.0f;
}

void Jogador::DecrementaTimer(GLfloat d_t)
{
    this->timerTiro = std::max(this->timerTiro - d_t, 0.0f);
}

void Jogador::ResetaTimer()
{
    this->timerTiro = TIMER_TIRO;
}

int Jogador::colisaoTiro(std::vector<Tiro>& tiros, int id_jogador)
{
    std::vector<Tiro>::iterator it;

    for (it = tiros.begin(); it != tiros.end();)
    {
        Tiro& tiro = *it;

        if (id_jogador == tiro.IdJogador())
        {
            it++;
            continue;
        }

        if (tiro.Z() > this->gZ + this->raio * ALTURA)
        {
            it++;
            continue;
        }

        if (tiro.Z() < this->gZ)
        {
            it++;
            continue;
        }

        // verifica se o tiro atingiu o jogador
        GLfloat dist_x = tiro.X() - this->gX;
        GLfloat dist_y = tiro.Y() - this->gY;
        GLfloat dist = sqrt(pow(dist_x, 2.0f) + pow(dist_y, 2.0f));

        if (dist < this->raio)
        {
            it = tiros.erase(it);
            this->vidas--;
        }
        else
            it++;
    }

    return this->vidas;
}

GLfloat Jogador::XPontaBraco()
{
    GLfloat x = this->gX;
    x += raio * BRACO_DISTANCIA * sin(-gTheta * M_PI / 180.0f);
    x += raio * BRACO_COMPRIMENTO * cos((gTheta - gThetaBraco) * M_PI / 180.0f) * cos(-gThetaBracoVert * M_PI / 180.0f);
    return x;
}

GLfloat Jogador::YPontaBraco()
{
    GLfloat y = this->gY;
    y += raio * BRACO_DISTANCIA * cos(-gTheta * M_PI / 180.0f);
    y += raio * BRACO_COMPRIMENTO * sin((gTheta - gThetaBraco) * M_PI / 180.0f) * cos(-gThetaBracoVert * M_PI / 180.0f);
    return y;
}

GLfloat Jogador::ZPontaBraco()
{
    GLfloat z = this->gZ;
    z += raio * (OMBRO_ALTURA - BRACO_ESPESSURA);
    z += raio * BRACO_COMPRIMENTO * sin(-gThetaBracoVert * M_PI / 180.0f);
    return z;
}