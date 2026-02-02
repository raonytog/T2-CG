#include "../includes/arena.h"

void Arena::DesenhaArena(GLfloat x, GLfloat y, GLfloat raio,
                         GLfloat R, GLfloat G, GLfloat B)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    DesenhaCirc(raio, R, G, B, DETALHE_ARENA);

    for (auto& obst : this->obstaculos)
    {
        glPushMatrix();
        glTranslatef(obst.x, obst.y, 0.0f);
        DesenhaCirc(obst.raio, 0.0f, 0.0f, 0.0f, DETALHE_OBSTACULO);
        glPopMatrix();
    }

    glPopMatrix();
}

GLfloat Arena::X()
{
    return this->gX;
}

GLfloat Arena::Y()
{
    return this->gY;
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
    for (auto& obst : this->obstaculos)
    {
        GLfloat dist_x_1 = j_1->X() - obst.x;
        GLfloat dist_y_1 = j_1->Y() - obst.y;
        GLfloat dist_1 = sqrt(pow(dist_x_1, 2.0f) + pow(dist_y_1, 2.0f));
        GLfloat dist_min_1 = j_1->RaioColisao() + obst.raio;

        if (dist_1 < dist_min_1)
        {
            GLfloat ajuste = dist_min_1 - dist_1;
            v_1.push_back({ajuste * dist_x_1 / dist_1, ajuste * dist_y_1 / dist_1});
        }

        GLfloat dist_x_2 = j_2->X() - obst.x;
        GLfloat dist_y_2 = j_2->Y() - obst.y;
        GLfloat dist_2 = sqrt(pow(dist_x_2, 2.0f) + pow(dist_y_2, 2.0f));
        GLfloat dist_min_2 = j_2->RaioColisao() + obst.raio;

        if (dist_2 < dist_min_2)
        {
            GLfloat ajuste = dist_min_2 - dist_2;
            v_2.push_back({ajuste * dist_x_2 / dist_2, ajuste * dist_y_2 / dist_2});
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