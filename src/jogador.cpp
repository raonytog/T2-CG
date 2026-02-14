#include "../includes/jogador.h"

void Jogador::DesenhaBraco(GLfloat raio, GLfloat R, GLfloat G, GLfloat B, GLfloat theta_braco)
{
    GLfloat SCALE_COMPRIMENTO = 2, SCALE_ESPESSURA = 4, SCALE_ALTURA = 1;
    glPushMatrix();

        glTranslatef(0.0f, raio * BRACO_DISTANCIA / 2, ALTURA_MEMBROS);
        glRotatef(theta_braco, 0.0f, 0.0f, 1.0f); 

        DesenhaRectXPos(BRACO_COMPRIMENTO * SCALE_COMPRIMENTO, 
                        BRACO_ESPESSURA * SCALE_ESPESSURA, 
                        ALTURA_MEMBROS * SCALE_ALTURA, 
                        R, G, B);
    glPopMatrix();
}

void Jogador::DesenhaPerna(GLfloat raio, GLfloat animacao, GLfloat R, GLfloat G, GLfloat B)
{

    GLfloat SCALE_COMPRIMENTO = 2, SCALE_ESPESSURA = 4, SCALE_ALTURA = 1;
    glPushMatrix();
        glTranslatef(0.0f, raio * PERNA_DISTANCIA / 2, 0.0f);
        DesenhaRectXPos(PERNA_COMPRIMENTO * SCALE_COMPRIMENTO * cos(animacao),
                        PERNA_ESPESSURA * SCALE_ESPESSURA, 
                        ALTURA_MEMBROS * SCALE_ALTURA, 
                        R, G, B);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0f, -raio * PERNA_DISTANCIA / 2, 0.0f);
        DesenhaRectXPos(PERNA_COMPRIMENTO * SCALE_COMPRIMENTO * cos(animacao),
                        PERNA_ESPESSURA * SCALE_ESPESSURA, 
                        ALTURA_MEMBROS * SCALE_ALTURA, 
                        R, G, B);
    glPopMatrix();
}

void Jogador::DesenhaCorpo(GLfloat R, GLfloat G, GLfloat B) {
    GLfloat SCALE_COMPRIMENTO = 2, SCALE_ESPESSURA = 8, SCALE_ALTURA = 1;
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, ALTURA_MEMBROS);
        DesenhaRectXPos(PERNA_COMPRIMENTO * SCALE_COMPRIMENTO,
                        PERNA_ESPESSURA * SCALE_ESPESSURA, 
                        ALTURA_MEMBROS * SCALE_ALTURA, 
                        R, G , B);
    glPopMatrix();
}

void Jogador::DesenhaCabeca(GLfloat raio, GLfloat R, GLfloat G, GLfloat B) {
    glColor3d(R, G, B);
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, ALTURA_MEMBROS*2.5);
        glutSolidSphere(raio/2, 20, 20);
    glPopMatrix();
}

void Jogador::DesenhaJogador(GLfloat x, GLfloat y, GLfloat z, GLfloat unidade,
                            GLfloat R, GLfloat G, GLfloat B,
                            GLfloat animacao, GLfloat theta, GLfloat theta_braco, int vidas)
{
    glPushMatrix();

    glTranslatef(x, y, z);

    if (vidas > 0)
    {
        glRotatef(theta, 0.0f, 0.0f, 1.0f);

        DesenhaPerna(raio, animacao, R, G, B);
        DesenhaBraco(raio, R, G, B, theta_braco);
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
    if (this->gZ == 0.0f) 
    {
        this->velZ = VELOCIDADE_PULO;
    }
}

void Jogador::AtualizaFisica(GLfloat t_d)
{
    // Aplica gravidade na velocidade
    this->velZ -= GRAVIDADE * t_d;

    // Aplica velocidade na posição Z
    this->gZ += this->velZ * t_d;

    // Colisão com o chão
    if (this->gZ < 0.0f)
    {
        this->gZ = 0.0f;
        this->velZ = 0.0f;
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
    this->animacao += dist * t_d / (2 * PI * PERNA_COMPRIMENTO);
    this->animacao = normalizaAnguloRadianos(this->animacao);
}

void Jogador::AnimacaoReset(GLfloat dist, GLfloat t_d)
{
    // volta a uma posição neutra quando o jogador não está andando ou girando
    float abs_diff = dist * t_d / (2.0f * PI * PERNA_COMPRIMENTO);
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
}

void Jogador::RodaBracoMouse(GLfloat d_theta_braco, GLfloat theta_alvo, GLfloat t_d)
{
    float theta_braco_alvo = theta_alvo - this->gTheta;
    theta_braco_alvo = normalizaAnguloGraus(theta_braco_alvo);

    if (this->gThetaBraco < theta_braco_alvo)
    {
        this->gThetaBraco += d_theta_braco * t_d;
        this->gThetaBraco = std::min(this->gThetaBraco, std::min(45.0f, theta_braco_alvo));
    }
    else
    {
        this->gThetaBraco -= d_theta_braco * t_d;
        this->gThetaBraco = std::max(this->gThetaBraco, std::max(-45.0f, theta_braco_alvo));
    }
}

GLfloat Jogador::X()
{
    return this->gX;
}

GLfloat Jogador::Y()
{
    return this->gY;
}

GLfloat Jogador::Z()
{
    return this->gZ;
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

GLfloat Jogador::XBaseBraco()
{
    return this->gX - this->raio * BRACO_DISTANCIA * sin(this->gTheta * PI / 180);
}

GLfloat Jogador::YBaseBraco()
{
    return this->gY + this->raio * BRACO_DISTANCIA * cos(this->gTheta * PI / 180);
}

GLfloat Jogador::XPontaBraco()
{
    return this->gX - this->raio * BRACO_DISTANCIA * sin(this->gTheta * PI / 180)
    + this->raio * BRACO_COMPRIMENTO * cos((this->gTheta + this->gThetaBraco) * PI / 180);
}

GLfloat Jogador::YPontaBraco()
{
    return this->gY + this->raio * BRACO_DISTANCIA * cos(this->gTheta * PI / 180)
    + this->raio * BRACO_COMPRIMENTO * sin((this->gTheta + this->gThetaBraco) * PI / 180);
}

GLfloat Jogador::Theta()
{
    return this->gTheta;
}

GLfloat Jogador::ThetaBraco()
{
    return normalizaAnguloGraus(this->gTheta + this->gThetaBraco);
}

GLfloat Jogador::Altura()
{
    return ALTURA_MEMBROS*3;
}

int Jogador::Vidas()
{
    return this->vidas;
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

        std::array<std::pair<GLfloat, GLfloat>, 4> pontos = tiro.Pontos();

        // verifica se o tiro atingiu o jogador
        // confere cada extremidade do retângulo
        // essa é uma simplificação que assume
        // que a bala sempre é menor do que qualquer jogador
        // achei válido assumir isso dado o contexto do jogo
        bool colisao = false;
        for (int i = 1; i < 4; i++)
        {
            GLfloat dist_x = pontos[i].first - this->gX;
            GLfloat dist_y = pontos[i].second - this->gY;
            GLfloat dist = sqrt(pow(dist_x, 2.0f) + pow(dist_y, 2.0f));

            if (dist < this->raio)
            {
                colisao = true;
                break;
            }
        }

        if (colisao)
        {
            it = tiros.erase(it);
            this->vidas--;
        }
        else
            it++;
    }

    return this->vidas;
}