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
#include <vector>

#define PI 3.14159265359

std::string cwd()
{
    char buffer[FILENAME_MAX];
    getcwd(buffer, FILENAME_MAX);
    std::string b = {buffer};
    return b;
}

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
    Retorna verdadeiro se tiver adicionado, falso se só tiver somado
    */
    bool AdicionarOuSomar(int id, int intensidade)
    {
        if (this->id == id)
        {
            this->intensidade += intensidade;
        }
        else{
            if (this->prox)
                return prox->AdicionarOuSomar(id, intensidade);
            this->prox = new Aresta(id, intensidade);
        } 
        return true;
    }
    int Contar(){
        return this->prox?this->prox->Contar()+1:1;
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

void preencherFechoTransitivoDireto(std::vector<Aresta *> &arestas, int *atingiveis, int v)
{
    atingiveis[v] = 1;
    Aresta *prox = arestas[v];
    while (prox != nullptr)
    {
        if (!atingiveis[prox->id])
            preencherFechoTransitivoDireto(arestas, atingiveis, prox->id);
        prox = prox->prox;
    }
}
void preencherFechoTransitivoInverso(std::vector<Aresta *> &arestas, int *atingem, int v)
{
    atingem[v] = 1;
    for (int i = 0; i < arestas.size(); i++)
    {
        if (!atingem[i])
        {
            Aresta *prox = arestas[i];
            while (prox != nullptr)
            {
                if (prox->id == v)
                    preencherFechoTransitivoInverso(arestas, atingem, i);
                prox = prox->prox;
            }
        }
    }
}

/*
Grafo implementado com lista de adjecencia. Cada node possui um nome e cada aresta uma intensidade.
*/
class Grafo
{
private:
    std::vector<std::string> nomes;
    std::vector<int> profundidades;
    std::vector<Aresta *> arestas;
public:
    /*Cria um grafo com espaço reservado*/
    Grafo(int capacidade)
    {
        this->nomes.reserve(capacidade);
        this->profundidades.reserve(capacidade);
        this->arestas.reserve(capacidade);
    }

    /*Cria um grafo a partir de txt*/
    Grafo(std::string addr)
    {
        std::string fullpath = cwd() + '/' + addr;
        std::ifstream inFile(fullpath);

        int temp;
        std::string linha;
        if (!inFile.is_open())
            throw std::ifstream::failure("Arquivo não pode ser aberto");

        // lê o tipo de grafo
        inFile >> temp;
        if (temp != 6)
            throw "Arquivo incompatível";

        int capacidade;
        inFile >> capacidade;

        this->nomes.reserve(capacidade);
        this->arestas.reserve(capacidade);

        // limpa buffer para permitir getline
        inFile.ignore();
        for (int i = 0; i < capacidade; i++)
        {
            getline(inFile, linha);
            int posUltimoEspaco = linha.rfind(" ");
            int posPrimeiroEspaco = linha.find(" ");
            std::string nodeName = linha.substr(posPrimeiroEspaco + 1, posUltimoEspaco - posPrimeiroEspaco - 1);
            std::string profundidadeStr = linha.substr(posUltimoEspaco + 1);
            std::cout << nodeName << " ==> " << profundidadeStr << std::endl;

            this->InserirNode(nodeName, std::stoi(profundidadeStr));
        }
        int m, origem, destino, peso;
        inFile >> m;
        for (int i = 0; i < m; i++)
        {
            inFile >> origem;
            inFile >> destino;
            inFile >> peso;
            this->InserirAresta(origem, destino, peso);
        }
    }

    /*getters*/
    int ContarNodes()
    {
        return this->nomes.size();
    }
    int ContarArestas()
    {
        int sum = 0;
        for (int i = 0; i < arestas.size(); i++)
            if(arestas[i])
                sum += arestas[i]->Contar();
        
        return sum;
    }
    int ObterCapacidade()
    {
        return this->nomes.capacity();
    }
    std::string ObterNode(int i)
    {
        if (i < this->ContarNodes())
            return nomes[i];
        return "";
    }
    int ObterIndexNode(std::string nome)
    {
        for (int i = 0; i < ContarNodes(); i++)
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
    bool InserirNode(std::string nome, int profundidade = 0)
    {
        for (int i = 0; i < this->ContarNodes(); i++)
        {
            if (this->nomes[i] == nome)
                return false;
        }

        this->nomes.push_back(nome);
        this->profundidades.push_back(profundidade);
        this->arestas.push_back(nullptr);
        return true;
    }
    /*
    Insere uma aresta ligando o node de index 'a' com o de index 'b'.
    Se ja existe uma aresta fazendo essa ligação, falha
    */
    bool InserirAresta(int a, int b, int intensidade)
    {
        if (a >= 0 && a < this->ContarNodes() && b >= 0 && b < this->ContarNodes())
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
            return true;
        }
        return false;
    }
    /*
    Insere uma aresta ligando o node de nome 'a' com o de nome 'b'.
    Se ja existe uma aresta fazendo essa ligação, falha
    */
    bool InserirAresta(std::string a, std::string b, int intensidade)
    {
        int na = ObterIndexNode(a);
        int nb = ObterIndexNode(a);
        if (na < 0 || nb < 0)
            return false;
        return InserirAresta(na, nb, intensidade);
    }

    bool InserirOuSomarAresta(int a, int b, int intensidade)
    {
        if (a >= 0 && a < this->ContarNodes() && b >= 0 && b < this->ContarNodes())
        {
            if (this->arestas[a])
                this->arestas[a]->AdicionarOuSomar(b, intensidade);
            else
                this->arestas[a] = new Aresta(b, intensidade);
            return true;
        }
        return false;
    }

    bool RemoverVertice(int id)
    {
        if (id < 0 || id > ContarNodes())
            return false;
        for (int i = 0; i < ContarNodes(); i++)
        {
            Aresta **base = &arestas[i];
            while (*base != nullptr)
            {
                if ((*base)->id == id)
                {
                    Aresta *temp = *base;
                    *base = temp->prox;
                    delete temp;
                }
                else
                {
                    if ((*base)->id > id)
                        (*base)->id--;
                    base = &((*base)->prox);
                }
            }
        }
        // remove o elementos do vetores
        nomes.erase(nomes.begin() + id);
        arestas.erase(arestas.begin() + id);
        return true;
    }

    bool RemoverAresta(int origem, int destino)
    {
        if (origem >= 0 || origem < ContarNodes())
        {
            Aresta **base = &arestas[origem];
            while (*base != nullptr)
            {
                if ((*base)->id == destino)
                {
                    Aresta *temp = *base;
                    *base = temp->prox;
                    delete temp;
                    return true;
                }
                else
                {
                    base = &((*base)->prox);
                }
            }
        }

        return false;
    }
    /*
    Exporta o grafo para ser usado em https://graphonline.ru/en/ no arquvo 'destino'.
    Os nodes são organizados em uma circunferencia
    */
    void ExportarParaGraphML(std::string destino)
    {
        // configuracoes de aparencia
        int nodeSize = 40;
        float centro[2] = {400, 200};

        // abre o arquivo
        std::fstream file(destino, std::fstream::out);

        // insere o header
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <graphml> <graph id=\"Graph\" uidGraph=\"6\" uidEdge=\"10008\">";

        std::vector<int> qtdPorNivel(this->ContarNodes());
        std::vector<int> qtdFeitaPorNivel(this->ContarNodes());
        for (int i = 0; i < this->ContarNodes(); i++)
        {
            qtdPorNivel[this->profundidades[i]]++;
        }

        // insere cada node
        for (int i = 0; i < this->ContarNodes(); i++)
        {
            double angulo = qtdFeitaPorNivel[this->profundidades[i]] * 2.0 * PI / qtdPorNivel[this->profundidades[i]];
            float radius = 200.0 * this->profundidades[i];

            qtdFeitaPorNivel[this->profundidades[i]]++;

            file << "<node positionX=\"" << centro[0] + cos(angulo) * radius << "\" positionY=\"" << centro[1] + sin(angulo) * radius << "\" id=\"" << i << "\" mainText=\"" << wikipediaEscape(this->nomes[i]) << "\" upText=\"\" size=\"" << nodeSize << "\"></node>";
        }
        // insere cada aresta
        for (int i = 0; i < this->ContarNodes(); i++)
        {
            Aresta *proximo = this->arestas[i];
            while (proximo)
            {
                file
                    << "<edge source=\"" << i << "\" target=\"" << proximo->id << "\" isDirect=\"true\" weight=\"" << proximo->intensidade << "\" useWeight=\"true\" id=\"" << 10000 + i << "\" text=\"\" upText=\"\" arrayStyleStart=\"\" arrayStyleFinish=\"\" model_width=\"4\" model_type=\"1\" model_curveValue=\"-0.2\" model_default=\"true\"></edge>";

                proximo = proximo->prox;
            }
        }
        // insere o footer
        file << "</graph></graphml>";

        file.close();
    }

    /*
    Exporta o grafo para ser usado em https://graphonline.ru/en/ com um nome padrão na pasta atual.
    */
    void ExportarParaGraphML()
    {
        ExportarParaGraphML(cwd() + "/export/export.graphml");
    }

    void ExportarParaTXT(std::string destino)
    {
        // abre o arquivo
        std::fstream file(destino, std::fstream::out);

        // insere o tipo de grafo
        file << 6 << '\n'
             // insere o numero de vertices
             << this->ContarNodes();

        // insere cada node
        for (int i = 0; i < this->ContarNodes(); i++)
            file << '\n'
                 << i << ' ' << wikipediaEscape(this->nomes[i]) << ' ' << this->profundidades[i];

        // insere o numero de arestas
        file << '\n'
             << this->ContarArestas();
        // insere cada aresta
        for (int i = 0; i < ContarNodes(); i++)
        {
            Aresta *proximo = this->arestas[i];
            while (proximo)
            {
                file << '\n'
                     << i << ' ' << proximo->id << ' ' << proximo->intensidade;
                proximo = proximo->prox;
            }
        }

        file.close();
    }

    void ExportarParaTXT()
    {
        ExportarParaTXT(cwd() + "/grafo.txt");
    }
    /*Libera a memoria*/
    ~Grafo()
    {
        for (int i = 0; i < ContarNodes(); i++)
            if (this->arestas[i])
                delete this->arestas[i];
    }

    int CategoriaConexidade()
    {
        int *atingidos = (int *)calloc(sizeof(int), ContarNodes());
        int *atingem = (int *)calloc(sizeof(int), ContarNodes());

        int categoria = 3;

        // verifica por desconexo
        for (int i = 0; i < ContarNodes(); i++)
        {
            Aresta *prox = arestas[i];
            while (prox != nullptr && categoria > 0)
            {
                atingem[prox->id] = 1;
                prox = prox->prox;
            }
        }
        for (int i = 0; i < ContarNodes(); i++)
            if (atingem[i] == 0)
                categoria = 0;

        if (categoria > 0)
            for (int v = 0; v < ContarNodes(); v++)
            {
                // limpa
                for (int i = 0; i < ContarNodes(); i++)
                {
                    atingidos[i] = 0;
                    atingem[i] = 0;
                }

                // preenche
                preencherFechoTransitivoDireto(this->arestas, atingidos, v);
                preencherFechoTransitivoInverso(this->arestas, atingem, v);

                if (categoria == 3)
                {
                    for (int i = 0; i < ContarNodes(); i++)
                        if (!atingidos[i])
                        {
                            categoria--;
                            break;
                        }
                }
                if (categoria == 2)
                {
                    for (int i = 0; i < ContarNodes(); i++)
                        if (!atingidos[i] && !atingem[i])
                        {
                            categoria--;
                            break;
                        }
                }
            }

        free(atingidos);
        free(atingem);
        return categoria;
    }

    Grafo *Reduzido()
    {
        int *atingidos = (int *)malloc(sizeof(int) * ContarNodes());
        int *atingem = (int *)malloc(sizeof(int) * ContarNodes());

        // cada pos de grupos em q grupo está o vertice no index
        int *grupo = (int *)malloc(sizeof(int) * ContarNodes());
        int qtd_grupos = 0;

        for (int i = 0; i < ContarNodes(); i++)
            grupo[i] = -1;

        for (int v = 0; v < ContarNodes(); v++)
        {
            // se ainda nao foi movido para um grupo
            if (grupo[v] == -1)
            {
                grupo[v] = qtd_grupos;

                // limpa
                for (int i = 0; i < ContarNodes(); i++)
                {
                    atingidos[i] = 0;
                    atingem[i] = 0;
                }

                // preenche
                preencherFechoTransitivoDireto(arestas, atingidos, v);
                preencherFechoTransitivoInverso(arestas, atingem, v);

                // // imprime
                // std::cout<< std::endl;
                // for (int i = 0; i < ContarNodes(); i++)
                //     std::cout << atingidos[i] << ' ';
                // std::cout << "    ";
                // for (int i = 0; i < ContarNodes(); i++)
                //     std::cout << atingem[i] << ' ';
                // std::cout << "    ";
                // for (int i = 0; i < ContarNodes(); i++)
                //     std::cout << grupo[i] << ' ';
                // std::cout<< std::endl;

                for (int i = 0; i < ContarNodes(); i++)
                {
                    if (grupo[i] == -1 && atingidos[i] && atingem[i])
                    {
                        grupo[i] = qtd_grupos;
                    }
                }
                qtd_grupos++;
            }
        }

        Grafo *c = new Grafo(qtd_grupos);
        for (int i = 0; i < qtd_grupos; i++)
        {
            std::stringstream s;
            bool prim = true;
            for (int j = 0; j < ContarNodes(); j++)
            {
                if (grupo[j] == i)
                {
                    if (prim)
                        prim = false;
                    else
                        s << '+';
                    s << j;
                }
            }
            c->InserirNode(s.str());
        }

        // associa as arestas
        for (int i = 0; i < ContarNodes(); i++)
        {
            Aresta *prox = arestas[i];
            while (prox)
            {
                if (grupo[i] != grupo[prox->id])
                    c->InserirAresta(grupo[i], grupo[prox->id], 1); // Não deveria ser 1! Foi 1 por folga. Deveria somar todos
                prox = prox->prox;
            }
        }

        free(atingem);
        free(atingidos);
        free(grupo);

        return c;
    }

    /*Declara friends*/
    friend std::ostream &operator<<(std::ostream &out, Grafo *const &data);
};

/*
Operator overload para permitir impressao
*/
std::ostream &operator<<(std::ostream &out, Grafo *const &data)
{
    out << "m: " << data->ContarArestas() << "  n: " << data->ContarNodes() << "\n";
    for (int i = 0; i < data->ContarNodes(); i++)
    {
        out << i << '(' << data->nomes[i] << ") ";
        Aresta *proxima = data->arestas[i];
        while (proxima)
        {
            out << " ──(" << proxima->intensidade << ")──> " << proxima->id;
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