#ifndef ___MATRIZ___

#define ___MATRIZ___

#include <stdlib.h>
#include <iostream>

class Matriz
{
private:
    int *vec;
    int m;
    int n;

public:
    Matriz(int largura, int altura);
    Matriz(int largura, int altura, int filler);
    int get(int y, int x);
    int largura();
    int altura();
    void set(int y, int x, int val);
    ~Matriz();
};

Matriz::Matriz(int largura, int altura)
{
    m = altura;
    n = largura;
    vec = (int *)malloc(sizeof(int) * largura * altura);
}
Matriz::Matriz(int largura, int altura, int filler){
    m = altura;
    n = largura;
    vec = (int *)malloc(sizeof(int) * largura * altura);
    for (int i = 0; i < largura*altura; i++)
        vec[i] = filler;
    
}

int Matriz::get(int y, int x)
{
    return this->vec[y * n + x];
}
void Matriz::set(int y, int x, int val){
    this->vec[y * n + x] = val;
}

int Matriz::largura()
{
    return this->n;
}
int Matriz::altura()
{
    return this->m;
}

Matriz::~Matriz()
{
    free(vec);
}

/*
Operator overload para permitir impressao
*/
std::ostream &operator<<(std::ostream &out, Matriz *const &data)
{
    std::cout << '\n';

    for (int i = 0; i < data->altura(); i++)
    {
        for (int j = 0; j < data->largura(); j++)
            std::cout << '\t' << data->get(j, i);
        std::cout << '\n';
    }

    return out;
}
#endif