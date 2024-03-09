#ifndef __GRAFO_HPP__
#define __GRAFO_HPP__

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include <cmath>
#include "html.hpp"
#include <unistd.h>

#define PI 3.14159265359

/*
Aresta é um tipo de lista ligada
*/
class Aresta
{
public:
    int intensidade;
    int id;
    Aresta *prox;

    /*
    Cria uma nova aresta com proximo item definido
    */
    Aresta(int id, int intensidade, Aresta *prox)
    {
        this->id = id;
        this->intensidade = intensidade;
        this->prox = prox;
    }
    /*
    Cria uma nova aresta sem proximo
    */
    Aresta(int id, int intensidade) : Aresta(id, intensidade, nullptr) {}

    /*
    Insere nova aresta no fim da lista
    */
    bool PorNoFim(Aresta *nova)
    {
        if (this->id == nova->id)
            return false;
        if (this->prox)
            return prox->PorNoFim(nova);
        this->prox = nova;
        return true;
    }
    /*
    Só precisa liberar o primeiro item da lista, porque cada um libera o proximo
    */
    ~Aresta()
    {
        if (prox)
            delete prox;
    }
};

/*
Grafo implementado com lista de adjecencia. Cada node possui um nome e cada aresta uma intensidade.
*/
class Grafo
{
private:
    std::string *nomes;
    Aresta **arestas;
    
    /*capacidade atual*/
    int capacidade;
    /*quantidade de nodes*/
    int n;
    /*quantidade de arestas*/
    int m;

public:
    /*Cria um grafo com espaço reservado*/
    Grafo(int capacidade)
    {
        this->nomes = (std::string *)malloc(sizeof(std::string) * capacidade);
        this->arestas = (Aresta **)malloc(sizeof(std::string) * capacidade);
        this->n = 0;
        this->m = 0;
        this->capacidade = capacidade;
    }

    /*getters*/
    int ContarNodes()
    {
        return this->n;
    }
    int ObterCapacidade()
    {
        return this->capacidade;
    }
    std::string ObterNode(int i)
    {
        if (i < n)
            return nomes[i];
        return "";
    }
    int ObterIndexNode(std::string nome)
    {
        for (int i = 0; i < this->n; i++)
        {
            if (this->nomes[i] == nome)
                return i;
        }
        return -1;
    }

    /*
    Insere o node, se o nome ainda nao tiver sido usado.
    Se nao tiver espaço, aumenta a capacidade antes de inserir
    */
    bool InserirNode(std::string nome)
    {
        if(this->n == this->capacidade){
            this->capacidade++;
            this->nomes = (std::string *)realloc(this->nomes,sizeof(std::string) * capacidade);
            this->arestas = (Aresta **)realloc(this->arestas, sizeof(std::string) * capacidade);
        }
        for (int i = 0; i < this->n; i++)
        {
            if (this->nomes[i] == nome)
                return false;
        }

        this->nomes[n++] = nome;
        return true;
    }
    /*
    Insere uma aresta ligando o node de index 'a' com o de index 'b'.
    Se ja existe uma aresta fazendo essa ligação, falha
    */
    bool InserirAresta(int a, int b, int intensidade)
    {
        Aresta *nova = new Aresta(b, intensidade);
        if (this->arestas[a])
        {
            Aresta *temp = this->arestas[a];
            if (!this->arestas[a]->PorNoFim(nova))
            {
                delete nova;
                return false;
            }
        }
        else
            this->arestas[a] = nova;
        this->m++;
        return true;
    }
    /*
    Insere uma aresta ligando o node de nome 'a' com o de nome 'b'.
    Se ja existe uma aresta fazendo essa ligação, falha
    */
    bool InserirAresta(std::string a, std::string b, int intensidade){
        int na = ObterIndexNode(a);
        int nb = ObterIndexNode(a);
        if(na<0||nb<0)
            return false;
        return InserirAresta(na,nb,intensidade);
    }

    /*
    Exporta o grafo para ser usado em https://graphonline.ru/en/ no arquvo 'destino'.
    Os nodes são organizados em uma circunferencia
    */
    void ExportarParaGraphML(std::string destino)
    {
        //configuracoes de aparencia
        int nodeSize = 40;
        float radius = 20.0 * this->n;
        float centro[2] = {400, 200};

        //abre o arquivo
        std::fstream file(destino, std::fstream::out);

        //insere o header
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <graphml> <graph id=\"Graph\" uidGraph=\"6\" uidEdge=\"10008\">";

        //insere cada node
        for (int i = 0; i < this->n; i++)
        {
            double angulo = i * 2.0 * PI / this->n;
            file << "<node positionX=\"" << centro[0] + cos(angulo) * radius << "\" positionY=\"" << centro[1] + sin(angulo) * radius << "\" id=\"" << i << "\" mainText=\"" << wikipediaEscape(this->nomes[i]) << "\" upText=\"\" size=\"" << nodeSize << "\"></node>";
        }
        //insere cada aresta
        for (int i = 0; i < n; i++)
        {
            Aresta *proximo = this->arestas[i];
            while (proximo)
            {
                file
                    << "<edge source=\"" << i << "\" target=\"" << proximo->id << "\" isDirect=\"true\" weight=\"" << proximo->intensidade << "\" useWeight=\"true\" id=\"" << 10000 + i << "\" text=\"\" upText=\"\" arrayStyleStart=\"\" arrayStyleFinish=\"\" model_width=\"4\" model_type=\"0\" model_curveValue=\"0.1\" model_default=\"NaN\"></edge>";

                proximo = proximo->prox;
            }
        }
        //insere o footer
        file << "</graph></graphml>";

        file.close();
    }

    /*
    Exporta o grafo para ser usado em https://graphonline.ru/en/ com um nome padrão na pasta atual.
    */
    void ExportarParaGraphML()
    {
        char buffer[FILENAME_MAX];
        if (getcwd(buffer, FILENAME_MAX) != nullptr)
        {
            std::string b{buffer};
            ExportarParaGraphML(b + "/export/export.graphml");
        }
    }

    /*Libera a memoria*/
    ~Grafo()
    {
        free(nomes);
        for (int i = 0; i < this->n; i++)
            if (this->arestas[i])
                delete this->arestas[i];
    }

    /*Declara friends*/
    friend std::ostream &operator<<(std::ostream &out, Grafo *const &data);
};

/*
Operator overload para permitir impressao
*/
std::ostream &operator<<(std::ostream &out, Grafo *const &data)
{
    out << "m: " << data->m << "  n: " << data->n << "\n";
    for (int i = 0; i < data->n; i++)
    {
        out << i << '(' << data->nomes[i] << ") ";
        Aresta *proxima = data->arestas[i];
        while (proxima)
        {
            out << " --" << proxima->intensidade << "-> " << proxima->id;
            proxima = proxima->prox;
        }
        out << '\n';
    }
    return out;
}

void testeBasicoGrafo()
{
    Grafo *g = new Grafo(5);
    g->InserirNode("A");
    g->InserirNode("B");
    g->InserirNode("C");
    g->InserirNode("D");
    g->InserirNode("E");

    g->InserirAresta(0, 1, 10);
    g->InserirAresta(0, 4, 13);
    g->InserirAresta(0, 3, 12);
    g->InserirAresta(1, 0, 11);
    g->InserirAresta(3, 4, 16);
    g->InserirAresta(3, 1, 14);
    g->InserirAresta(3, 1, 14);

    std::cout << g << std::endl;

    delete g;
}

#endif