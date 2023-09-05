#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
using namespace std;

double uniforme(){
    double u = rand() / ((double) RAND_MAX + 1);
    //LIMITANDO ENTRE (0,1]
    u = 1.0 - u;
    return (u);
}

int main() 
{
    /*
        Na exponencial, E[X] = 1/l
        E[X] = 5 --> 1/l = 5 --> l = 1/5 --> l = 0.2
    */
   double l;
   cout << "Informe o tempo médio em segundos: " << endl;
   scanf("%lF", &l);
    l = 1.0/l;

    /*
        Variáveis para cálculo da média final
    */
    double soma = 0.0;
    double qtd_valores_gerados = 1000000;

    /*
        Iniciando a semente p/ a geração dos números pseudo-aleatorios
    */
    int semente = time(NULL);
    srand(semente);
    int i;
    for(i = 0; i < qtd_valores_gerados; i++){
        double valor = (-1.0/l) * log(uniforme());
        //cout << valor << endl;
        //getchar();
        soma += valor;
    }
    cout << "Média: "<< soma/qtd_valores_gerados << endl;

    return 0;
}