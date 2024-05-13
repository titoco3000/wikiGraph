Esse projeto cria um grafo representando a wikipédia, a partir de uma página raiz. O grafo criado é exportado para um formato que pode ser vizualizado em https://graphonline.ru/en/.

Para compilar e executar:

    g++ main.cpp -lcurl -o build/wiki.exe && ./build/wiki.exe \<quantidade> \<página inicial>

Sendo o valor padrão de quantidade = 30 e de página inicial = "Filosofia"


Parte 2:
Implementação coloração