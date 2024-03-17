#include <iostream>
#include <string>

#include "Grafo.hpp"
#include "html.hpp"

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
            g->ExportarParaTXT();
            std::cout << "Exportado.\n";
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
        else if (inputUsuario == "f"){
            int origemIndex, destinoIndex;
            std::cout << "Índice vértice de origem: ";
            std::cin >> origemIndex;
            std::cout << "Índice vértice de destino: ";
            std::cin >> destinoIndex;
            std::cout << (g->RemoverAresta(origemIndex, destinoIndex)?"Aresta removida":"Aresta não pôde ser removida")<<'\n';
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
        else if (inputUsuario == "i"){
            std::cout<< "Categoria de conexidade:"<<g->CategoriaConexidade()<<"\n\nReduzido:\n";
            Grafo *c = g->Reduzido();
            std::cout<<c<<std::endl;
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
            g = new Grafo(qtd);
            seek(g, root);
            std::cout << "Mapeado.\n";
        }
        else if (inputUsuario == "l"){
            g->ExportarParaGraphML();
            std::cout<<"Grafo exportado para ser usado em https://graphonline.ru/en/\n";
        }
        else
        {
            std::cout << "Opção não reconhecida\n";
        }
    }
}

int main(int argc, char *argv[])
{
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