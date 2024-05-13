#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

#include "Grafo.hpp"
#include "html.hpp"

#define TAMANHO_LISTA_NEGRA 1
const std::string listaNegra[TAMANHO_LISTA_NEGRA] = {"Wikiquote"};

const std::string masterUrl = "https://pt.wikipedia.org/wiki/";

void log(std::string msg)
{
    // abre o arquivo
    std::fstream file(cwd() + "/log.txt", std::fstream::out);
    file << msg;
    file.close();
}

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
                if (chave.find('.') == -1 && bloco.substr(bloco.find(">", fim) + 1, 2).find('[') == -1)
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
                    std::cout << "Procurando \"" + outro + "\" em " << bloco.substr(0, 20) << std::endl;
                    log("Procurando \"" + outro + "\" em " + bloco);
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

struct WikiLink
{
    std::string titulo;
    int qtd;

    static bool comparar(const WikiLink &a, const WikiLink &b)
    {
        return a.qtd > b.qtd;
    }

    bool existeTituloNoVetor(const std::vector<WikiLink> &vec)
    {
        for (auto it = vec.begin(); it != vec.end(); it++)
        {
            if (it->titulo == this->titulo)
            {
                return true;
            }
        }
        return false;
    }

    static bool tituloIgual(const WikiLink &a, const WikiLink &b)
    {
        return a.titulo == b.titulo;
    }
};
/*
Modificado de:
https://stackoverflow.com/questions/22489073/counting-the-number-of-occurrences-of-a-string-within-a-string
*/
int contarReferencias(std::string str, std::string substr)
{
    int count = 0;
    for (int i = 0; i < str.size() - 1; i++)
    {
        int m = 0;
        int n = i;
        for (int j = 0; j < substr.size(); j++)
        {
            if (str[n] == substr[j])
            {
                m++;
            }
            n++;
        }
        if (m == substr.size())
        {
            count++;
        }
    }
    return count;
}

Grafo *seekLimitadoPorNivel(int tamanho, std::string subAddr)
{
    int nodesPorNivel = 5;

    std::vector<std::string> titulos;
    std::vector<std::vector<WikiLink>> links(tamanho);
    std::vector<int> niveis;
    titulos.reserve(tamanho);
    links.reserve(tamanho);

    titulos.push_back(subAddr);
    niveis.push_back(0);
    for (int i = 0; i < titulos.size(); i++)
    {
        std::cout << '(' << i + 1 << '/' << tamanho << ')' << " adicionando: " << wikipediaEscape(titulos[i]) << std::endl;
        std::string url = masterUrl + titulos[i];
        std::string readBuffer = obterSite(url);

        // se conseguiu ler
        if (readBuffer != "")
        {
            // inicio delimitado pelas classes "mw-content-ltr mw-parser-output"
            int inicio = readBuffer.find("<p>", readBuffer.find("mw-content-ltr mw-parser-output"));
            // fim delimitado pela classe "reflist"
            int fim = readBuffer.find("reflist", inicio);

            if (fim == -1 || inicio == -1)
            {
                std::cout << "Conteudo indisponivel!" << std::endl;
            }
            else
            {
                // bloco é a parte que nos interessa, é o artigo em si
                std::string bloco = readBuffer.substr(inicio, fim - inicio);

                // busca todos os links
                inicio = 0;
                fim = 0;
                while (true)
                {
                    inicio = bloco.find("<a href=\"/wiki", inicio);
                    if (inicio == std::string::npos)
                        break;
                    else
                    {
                        inicio += 15;
                        fim = bloco.find("\"", inicio);
                        std::string addr = bloco.substr(inicio, fim - inicio);

                        WikiLink l;
                        l.titulo = addr;

                        bool existeNaListaNegra = false;
                        for (int i = 0; i<TAMANHO_LISTA_NEGRA ; i++)
                        {
                            if (listaNegra[i] == addr)
                            {
                                existeNaListaNegra = true;
                                break;
                            }
                        }

                        // verifica que nao contem ':', se não está na lista negra e se já foi adicionado no vetor
                        if (addr.find(':') == std::string::npos && !existeNaListaNegra && !l.existeTituloNoVetor(links[i]))
                        {
                            std::string escaped = wikipediaEscape(addr);
                            l.qtd = contarReferencias(bloco, escaped);

                            links[i].push_back(l);
                        }
                    }
                }
                // ordena por qtd de referencias
                std::sort(links[i].begin(), links[i].end(), WikiLink::comparar);

                int index = 0;
                int adicionadas = 0;
                while (adicionadas < nodesPorNivel && index < links[i].size() && titulos.size() < tamanho)
                {
                    // verifica se já foi adicionado no vetor
                    if (std::find(titulos.begin(), titulos.end(), links[i][index].titulo) == titulos.end())
                    {
                        titulos.push_back(links[i][index].titulo);
                        niveis.push_back(niveis[i] + 1);
                        adicionadas++;
                    }
                    index++;
                }
            }
        }
    }

    Grafo *g = new Grafo(tamanho);
    for (int i = 0; i < titulos.size(); i++)
        g->InserirNode(titulos[i], niveis[i]);
    for (int i = 0; i < titulos.size(); i++)
    {
        for (int j = 0; j < links[i].size(); j++)
        {
            int index = g->ObterIndexNode(links[i][j].titulo);
            if (index != -1)
                g->InserirOuSomarAresta(i, index, links[i][j].qtd);
        }
    }
    return g;
}

void menu()
{
    std::cout << "\n    WIKIGRAPH\n";

    std::string inputUsuario;

    bool emExec = true;

    Grafo *g = new Grafo(10);

    while (emExec)
    {

        std::cout << "\n────────────────────────────────────────────────────────────────────────────────────────────────────\n"
                  << "Escolha uma das opções:\n"
                  << "a) Ler dados do arquivo grafo.txt\n"
                  << "b) Gravar dados no arquivo grafo.txt\n"
                  << "c) Inserir vértice\n"
                  << "d) Inserir aresta\n"
                  << "e) Remove vértice\n"
                  << "f) Remove aresta\n"
                  << "g) Mostrar conteúdo do arquivo\n"
                  << "h) Mostrar grafo\n"
                  << "i) Apresentar a conexidade do grafo e o reduzido\n"
                  << "j) Encerrar a aplicação\n"
                  << "k) Carregar dados a partir da wikipedia\n"
                  << "l) Exportar para graphML\n"
                  << "m) Colorir vértices\n"
                  << "\n>> ";
        std::cin >> inputUsuario;

        if (inputUsuario == "a")
        {
            try
            {
                delete g;
                g = new Grafo("grafo.txt");
                std::cout << "Carregado.\n";
            }
            catch (const std::exception &e)
            {
                std::cout << e.what() << '\n';
            }
        }
        else if (inputUsuario == "b")
        {
            std::cout << (g->ExportarParaTXT() ? "Exportado.\n" : "Grafo não pôde ser exportado\n");
        }
        else if (inputUsuario == "c")
        {
            std::string nome;
            std::cout << "Nome do vértice: ";
            std::cin >> nome;
            std::cout << (g->InserirNode(nome) ? "Inserido com sucesso" : "Não pôde ser inserido") << std::endl;
        }
        else if (inputUsuario == "d")
        {
            int origemIndex, destinoIndex, intensidade;
            std::cout << "Índice vértice de origem: ";
            std::cin >> origemIndex;
            std::cout << "Índice vértice de destino: ";
            std::cin >> destinoIndex;
            std::cout << "Peso: ";
            std::cin >> intensidade;

            if (origemIndex >= 0 && destinoIndex >= 0 && destinoIndex < g->ContarNodes() && origemIndex < g->ContarNodes() &&
                g->InserirAresta(origemIndex, destinoIndex, intensidade))
                std::cout << "Aresta inserida com sucesso\n";
            else
                std::cout << "Aresta não pôde ser inserida\n";
        }
        else if (inputUsuario == "e")
        {
            int id;
            std::cout << "Índice do vértice a ser removido: ";
            std::cin >> id;
            g->RemoverVertice(id);
            std::cout << "\nRemovido.\n";
        }
        else if (inputUsuario == "f")
        {
            int origemIndex, destinoIndex;
            std::cout << "Índice vértice de origem: ";
            std::cin >> origemIndex;
            std::cout << "Índice vértice de destino: ";
            std::cin >> destinoIndex;
            std::cout << (g->RemoverAresta(origemIndex, destinoIndex) ? "Aresta removida" : "Aresta não pôde ser removida") << '\n';
        }
        else if (inputUsuario == "g")
        {
            std::string fullpath = cwd() + "/grafo.txt";
            std::ifstream inFile(fullpath);

            if (!inFile.is_open())
                std::cout << "Falha ao abrir o arquivo\n";
            else
            {
                std::string line;
                while (getline(inFile, line))
                {
                    std::cout << line << '\n';
                }
                inFile.close();
            }
        }
        else if (inputUsuario == "h")
        {
            std::cout << g << std::endl;
        }
        else if (inputUsuario == "i")
        {
            std::cout << "Categoria de conexidade:" << g->CategoriaConexidade() << "\n\nReduzido:\n";
            Grafo *c = g->Reduzido();
            std::cout << c << std::endl;
            delete c;
        }
        else if (inputUsuario == "j")
        {
            delete g;
            std::cout << "\nEncerrado.\n";
            break;
        }
        else if (inputUsuario == "k")
        {
            std::string root;
            int qtd;
            std::cout << "De qual página da wikipédia deve começar: ";
            std::cin >> root;
            std::cout << "Quantas páginas devem ser mapeadas: ";
            std::cin >> qtd;
            delete g;
            // g = new Grafo(qtd);
            // seek(g, root);
            g = seekLimitadoPorNivel(qtd, root);
            std::cout << "Mapeado.\n";
        }
        else if (inputUsuario == "l")
        {
            std::cout << (g->ExportarParaGraphML() ? "Grafo exportado para ser usado em https://graphonline.ru/en/\n" : "Grafo não pôde ser exportado\n");
        }
        else if (inputUsuario == "m")
        {
            int grupos[g->ContarNodes()];
            int maxGrupoOcupado = g->AgruparEmCores(grupos)-1;
            std::cout << "Coloração dos vértices\n";
            for (int i = 0; i <= maxGrupoOcupado; i++)
            {
                std::cout << "Grupo " << i + 1 << ": {";
                bool primeiro = true;
                for (int j = 0; j < g->ContarNodes(); j++)
                {
                    if (grupos[j] == i)
                    {
                        if (!primeiro)
                            std::cout << ',';
                        primeiro = false;
                        std::cout << ' ' << j + 1;
                    }
                }
                std::cout << " }\n";
            }
            std::cout
            << "\nEste número pode não ser o número cromático mínimo, pois é calculado pelo algorítmo de coloração sequencial.\n";
        }
        else
        {
            std::cout << "Opção não reconhecida\n";
        }
    }
}

int main(int argc, char *argv[])
{
    // seekLimitadoPorNivel(10, "Filosofia");
    menu();

    // std::string root = "Filosofia";
    // int qtd = 3;

    // // entende os argumentos opcionais para quantidade e página inicial
    // for (int i = 1; i < argc; i++)
    // {
    //     try
    //     {
    //         int x = std::stoi(argv[i]);
    //         qtd = x;
    //     }
    //     catch (...)
    //     {
    //         root = argv[i];
    //     }
    // }

    // Grafo *g = new Grafo(qtd);

    // seek(g, root);

    // std::cout << "g:\n"
    //           << g << std::endl;

    // g->ExportarParaGraphML();

    // delete g;
    return 0;
}