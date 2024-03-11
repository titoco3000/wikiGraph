#include <iostream>
#include <string>

#include "Grafo.hpp"
#include "html.hpp"

const std::string masterUrl = "https://pt.wikipedia.org/wiki/";

void seek(Grafo *g, std::string subAddr)
{
    if (g->ContarNodes() < g->ObterCapacidade() && g->InserirNode(subAddr))
    {
        std::cout << '(' << g->ContarNodes() << '/' << g->ObterCapacidade() << ") " << wikipediaEscape(subAddr) << " inserido " << std::endl;

        std::string url = masterUrl + subAddr;
        std::string readBuffer = obterSite(url);

        // se conseguiu ler
        if (readBuffer != "")
        {
            // inicio delimitado pelas classes "mw-content-ltr mw-parser-output"
            int inicio = readBuffer.find("<p>", readBuffer.find("mw-content-ltr mw-parser-output"));
            // fim delimitado pelo id "catlinks"
            int fim = readBuffer.find("catlinks", inicio);

            if (fim == -1 || inicio == -1)
            {
                std::cout << "Conteudo indisponivel!" << std::endl;
                return;
            }

            // bloco é a parte que nos interessa, é o artigo em si
            std::string bloco = readBuffer.substr(inicio, fim - inicio);

            inicio = 0;

            // vai seguindo novos caminhos até esgotar o espaço
            while (g->ContarNodes() < g->ObterCapacidade())
            {
                int n = bloco.find("<a href=\"/wiki", inicio);
                if (n == std::string::npos)
                    break;
                inicio = n + 15;
                int fim = bloco.find("\"", inicio);
                std::string chave = bloco.substr(inicio, fim - inicio);
                
                
                // se for um arquivo, tem '.' e não deve ser buscado
                // se for link para pronucia, texto do link começa com '[' e não deve ser buscado
                if (chave.find('.') == -1 && bloco.substr(bloco.find(">",fim)+1,2).find('[') == -1)
                {
                    seek(g, chave);
                }
            }

            // conta referencias a todas as outras palavras-chave
            int indexProprio = g->ObterIndexNode(subAddr);
            for (int i = 0; i < g->ContarNodes(); i++)
            {
                std::string outro = g->ObterNode(i);
                if (i != indexProprio)
                {
                    int qtd = 0;
                    inicio = 0;
                    while (true)
                    {
                        int n = bloco.find(outro, inicio);
                        if (n == std::string::npos)
                            break;
                        else
                        {
                            qtd++;
                            inicio = n + outro.length();
                        }
                    }
                    if (qtd > 0)
                    {
                        g->InserirAresta(indexProprio, i, qtd);
                    }
                }
            }
        }
    }
}
int main(int argc, char *argv[])
{
    std::string root = "Filosofia";
    int qtd = 30;

    // entende os argumentos opcionais para quantidade e página inicial
    for (int i = 1; i < argc; i++)
    {
        try 
        {
            int x = std::stoi(argv[i]);
            qtd = x;
        }
        catch(...) { 
            root = argv[i];
         }
    }
    
    Grafo *g = new Grafo(qtd);

    seek(g, root);

    std::cout << "g:\n"
              << g << std::endl;

    g->ExportarParaGraphML();

    delete g;
    return 0;
}