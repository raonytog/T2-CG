#include "../includes/arena.h"

void Arena::DesenhaArena(GLfloat x, GLfloat y, GLfloat z, GLfloat raio,
                         GLfloat R, GLfloat G, GLfloat B, GLuint texParede, GLuint texPiso)
{
    glPushMatrix();
    glTranslatef(x, y, z);

    // chao
    DesenhaCirc(raio, R, G, B, DETALHE_ARENA, texPiso);

    // teto 
    glPushMatrix();
    glTranslatef(0, 0, ALTURA_ARENA);
    DesenhaCirc(raio, R, G, B, DETALHE_ARENA, texPiso);
    glPopMatrix();

    // paredes
    if (texParede != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texParede);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(R, G, B);
    }

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= DETALHE_ARENA; i++) {
        float angle = i * M_PI * 2 / DETALHE_ARENA;
        float u = (float)i / DETALHE_ARENA;
        
        glNormal3f(-cos(angle), -sin(angle), 0.0f);
        
        if (texParede != 0) glTexCoord2f(u, 0.0f);
        glVertex3f(raio * cos(angle), raio * sin(angle), 0.0f);
        
        if (texParede != 0) glTexCoord2f(u, 1.0f);
        glVertex3f(raio * cos(angle), raio * sin(angle), ALTURA_ARENA);
    }
    glEnd();

    if (texParede != 0) glDisable(GL_TEXTURE_2D);

    glPopMatrix();

    // obstáculos 
    for (auto& obst : this->obstaculos)
    {
        glPushMatrix();
            glTranslatef(obst.x, obst.y, 0);
            DesenhaCilindro(obst.raio, ALTURA_OBSTACULO, 1,1,1, DETALHE_OBSTACULO, 0);
        glPopMatrix();
    }
}

void Arena::DesenhaObstaculosMiniMapa()
{
    for (auto& obst : this->obstaculos) {
        glPushMatrix();
        glTranslatef(obst.x, obst.y, 0);
        // Desenha um círculo preenchido ou contorno para o obstáculo
        glBegin(GL_POLYGON);
        for(int i=0; i<360; i+=10){
            float ang = i*M_PI/180;
            glVertex2f(cos(ang)*obst.raio, sin(ang)*obst.raio);
        }
        glEnd();
        glPopMatrix();
    }
}

GLfloat Arena::X()
{
    return this->gX;
}

GLfloat Arena::Y()
{
    return this->gY;
}

GLfloat Arena::Z()
{
    return this->gZ;
}

GLfloat Arena::Raio()
{
    return this->raio;
}

void Arena::colisaoJogador(Jogador *j_1, Jogador *j_2)
{
    // cria um vetores com todos os ajustes que serão necessários pelas colisões de cada jogador
    std::vector<std::pair<GLfloat, GLfloat>> v_1;
    std::vector<std::pair<GLfloat, GLfloat>> v_2;

    // trata colisão entre jogadores
    GLfloat dist_x = j_1->X() - j_2->X();
    GLfloat dist_y = j_1->Y() - j_2->Y();
    GLfloat dist = sqrt(pow(dist_x, 2.0f) + pow(dist_y, 2.0f));
    GLfloat dist_min = j_1->RaioColisao() + j_2->RaioColisao();

    if (dist < dist_min)
    {
        GLfloat ajuste = (dist_min - dist) / 2.0f;
        v_1.push_back({ajuste * dist_x / dist, ajuste * dist_y / dist});
        v_2.push_back({-ajuste * dist_x / dist, -ajuste * dist_y / dist});
    }
    
    // trata colisão com a borda
    GLfloat dist_x_1 = j_1->X() - this->gX;
    GLfloat dist_y_1 = j_1->Y() - this->gY;
    GLfloat dist_centro_1 = sqrt(pow(dist_x_1, 2.0f) + pow(dist_y_1, 2.0f));
    GLfloat dist_max_1 = this->Raio() - j_1->RaioColisao();

    if (dist_centro_1 > dist_max_1)
    {
        GLfloat ajuste = dist_centro_1 - dist_max_1;
        v_1.push_back({-ajuste * dist_x_1 / dist_centro_1, -ajuste * dist_y_1 / dist_centro_1});
    }

    GLfloat dist_x_2 = j_2->X() - this->gX;
    GLfloat dist_y_2 = j_2->Y() - this->gY;
    GLfloat dist_centro_2 = sqrt(pow(dist_x_2, 2.0f) + pow(dist_y_2, 2.0f));
    GLfloat dist_max_2 = this->Raio() - j_2->RaioColisao();

    if (dist_centro_2 > dist_max_2)
    {
        GLfloat ajuste = dist_centro_2 - dist_max_2;
        v_2.push_back({-ajuste * dist_x_2 / dist_centro_2, -ajuste * dist_y_2 / dist_centro_2});
    }

    // trata colisão com os obstáculos
    Jogador *jogadores[2] = {j_1, j_2};
    for (auto& obst : this->obstaculos)
    {
        for (Jogador *j : jogadores)
        {
            GLfloat dist_x_1 = j->X() - obst.x;
            GLfloat dist_y_1 = j->Y() - obst.y;
            GLfloat dist_1 = sqrt(pow(dist_x_1, 2.0f) + pow(dist_y_1, 2.0f));
            GLfloat dist_min_1 = j->RaioColisao() + obst.raio;

            if (dist_1 < dist_min_1)
            {
                // 1. Se já está totalmente acima, ignora (passa voando)
                // 2. Se está caindo (velZ <= 0) e os pés estão próximos ao topo (margem de 5.0f), pousa.
                // 3. Caso contrário, é parede.
                
                bool pousou = false;

                // Está voando acima do obstáculo
                if (j->Z() >= ALTURA_OBSTACULO) { pousou = true; }
                else if (j->velZ() <= 0.0f && j->Z() > ALTURA_OBSTACULO - 5.0f) {
                    // Caindo e atingiu a borda superior -> Pousa
                    j->Pousa(ALTURA_OBSTACULO);
                    pousou = true;
                }

                // Se não pousou/não está em cima, é colisão lateral (parede)
                if (!pousou)
                {
                    GLfloat ajuste = dist_min_1 - dist_1;
                    v_1.push_back({ajuste * dist_x_1 / dist_1, ajuste * dist_y_1 / dist_1});
                }
            }
        }
    }

    // itera sobre os vetores pra tratar as colisões
    for (auto& c : v_1)
    {
        GLfloat d_x = c.first;
        GLfloat d_y = c.second;
        
        j_1->MoveX(d_x);
        j_1->MoveY(d_y);
    }

    for (auto& c : v_2)
    {
        GLfloat d_x = c.first;
        GLfloat d_y = c.second;
        
        j_2->MoveX(d_x);
        j_2->MoveY(d_y);
    }
}

void Arena::colisaoTiro(std::vector<Tiro>& tiros)
{
    std::vector<Tiro>::iterator it;

    for (it = tiros.begin(); it != tiros.end();)
    {
        Tiro& tiro = *it;
        std::array<std::pair<GLfloat, GLfloat>, 4> pontos = tiro.Pontos();

        // verifica se o tiro atingiu a borda da arena
        // confere cada extremidade do retângulo
        bool colisao = false;
        for (int i = 0; i < 4; i++)
        {
            GLfloat dist_x = pontos[i].first - this->gX;
            GLfloat dist_y = pontos[i].second - this->gY;
            GLfloat dist = sqrt(pow(dist_x, 2.0f) + pow(dist_y, 2.0f));

            if (dist > this->raio)
            {
                colisao = true;
                break;
            }
        }

        if (colisao)
        {
            it = tiros.erase(it);
            continue;
        }

        // verifica se o tiro colidiu com um obstáculo
        // confere cada extremidade do retângulo
        // essa é uma simplificação que assume
        // que a bala sempre é menor do que qualquer obstáculo
        // achei válido assumir isso dado o contexto do jogo
        for (const auto& obst : this->obstaculos)
        {
            for (int i = 0; i < 4; i++)
            {
                GLfloat dist_x = pontos[i].first - obst.x;
                GLfloat dist_y = pontos[i].second - obst.y;
                GLfloat dist = sqrt(pow(dist_x, 2.0f) + pow(dist_y, 2.0f));

                if (dist < obst.raio)
                {
                    colisao = true;
                    break;
                }
            }
            if (colisao)
                break;
        }

        if (colisao)
        {
            it = tiros.erase(it);
            continue;
        }

        it++;
    }
}