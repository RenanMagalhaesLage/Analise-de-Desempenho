/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
 *           UNIFAL − Universidade Federal de Alfenas.
 *             BACHARELADO EM CIENCIA DA COMPUTACAO.
 * Trabalho..: Relatório de Análise de Desempenho
 * Disciplina: Análise de Desempenho
 * Professor.: Flávio
 * Aluno.....: Renan Magalhães Lage
 * Data......: 10/10/2023
 *−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <float.h>
using namespace std;

double uniforme(){
    double u = rand() / ((double) RAND_MAX + 1);
    //LIMITANDO ENTRE (0,1]
    u = 1.0 - u;
    return (u);
}

double minimo(double d1, double d2){
    if(d1 < d2) return d1;
    return d2;
}

typedef struct {
    double media_chegada;
    double media_servico;
    double tempo_simulacao;
} parametros;

typedef struct{
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;
}little;

void inicia_little(little * l){
    l->no_eventos = 0;
    l->soma_areas = 0.0;
    l->tempo_anterior = 0.0;
}

void le_parametros(parametros * params, double valor_servico){
    params->media_chegada = 0.5; //Fixando a media_chegada pois esse valor é o necessário para obter as ocupações desejadas
    params->media_chegada = 1.0/params->media_chegada;

    params->media_servico = valor_servico;
    params->media_servico = 1.0/params->media_servico;

    params->tempo_simulacao = 864000; //Fixando o tempo pois foi determinado no trabalho
}



int main() 
{
    int semente = time(NULL);
    srand(semente);
    int qtd_calculos = 1;
    double valores_media_servico[4]= {0.3,0.4,0.45,0.495};
    while(qtd_calculos <= 4){
        parametros params;
        le_parametros(&params, valores_media_servico[qtd_calculos-1]);

        /* Variáveis de controle de Simulação */
        double tempo_decorrido = 0.0;
        double tempo_chegada = (-1.0/params.media_chegada) * log(uniforme()); //recebe o valor do primeiro
        double tempo_coleta = 10;
        double tempo_saida = DBL_MAX;
        unsigned long int fila = 0;
        unsigned long int max_fila = 0;

        /* Variáveis de medidas de Interesse */
        double soma_ocupacao = 0.0; //Ocupação <= 1.0
        little e_n;
        little e_w_chegada;
        little e_w_saida;

        inicia_little(&e_n);
        inicia_little(&e_w_chegada);
        inicia_little(&e_w_saida);

        /* Variáveis Cálculo*/
        double e_n_calculo;
        double e_w_calculo;
        double lambda;

        /* Criando arquivo para armazenar os erros a cada 10s */
        char Str[100];
        FILE *arq;
        int result;

        switch(qtd_calculos){
            case 1:
                arq = fopen("Erros1.csv", "wt");  // Cria um arquivo texto para gravação
                break;
            case 2:
                arq = fopen("Erros2.csv", "wt");
                break;
            case 3:
                arq = fopen("Erros3.csv", "wt");
                break;
            case 4:
                arq = fopen("Erros4.csv", "wt");
                break;
        }
        
        if (arq == NULL) // Se não conseguiu criar
        {
            printf("Problemas na CRIACAO do arquivo\n");
            return (1);
        }

        while (tempo_decorrido < params.tempo_simulacao)
        {
            //tempo_decorrido = min(tempo_chegada, tempo_saida);
            tempo_decorrido = min(min(tempo_chegada, tempo_saida), tempo_coleta);

            if (tempo_decorrido == tempo_chegada)
            {
                /* CHEGADA */
                /* A cabeça da fila é quem está em atendimento */
                if (!fila)
                {
                    double tempo_servico = (-1.0/params.media_servico) * log(uniforme());
                    tempo_saida = tempo_decorrido + tempo_servico;
                    soma_ocupacao += tempo_servico;
                }
                fila++;
                max_fila = fila > max_fila? fila:max_fila;
                tempo_chegada = tempo_decorrido + (-1.0/params.media_chegada) * log(uniforme());

                /* Cálculo de Little -- E[N] */
                e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
                e_n.no_eventos++;
                e_n.tempo_anterior = tempo_decorrido;

                /* Cálculo de Little -- E[W] -- Chegada */
                e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
                e_w_chegada.no_eventos++;
                e_w_chegada.tempo_anterior = tempo_decorrido;

                    
            }else if (tempo_decorrido == tempo_saida){
                /* SAÍDA */
                fila--;
                if (fila)
                {
                    double tempo_servico = (-1.0/params.media_servico) * log(uniforme());
                    tempo_saida = tempo_decorrido + tempo_servico;
                    soma_ocupacao += tempo_servico;
                }else{
                    tempo_saida = DBL_MAX;
                }
                /* Cálculo de Little -- E[N] */
                e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
                e_n.no_eventos--;
                e_n.tempo_anterior = tempo_decorrido;

                /* Cálculo de Little -- E[W] -- Saída */
                e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
                e_w_saida.no_eventos++;
                e_w_saida.tempo_anterior = tempo_decorrido;

            }else if(tempo_decorrido == tempo_coleta){
                /* Calculando o Erro de little a cada 10s */
                e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
                e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
                e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

                e_n.tempo_anterior = tempo_decorrido;
                e_w_chegada.tempo_anterior = tempo_decorrido;
                e_w_saida.tempo_anterior = tempo_decorrido;

                e_n_calculo = e_n.soma_areas / tempo_decorrido;
                e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
                lambda = e_w_chegada.no_eventos / tempo_decorrido; 
                //printf(" Erro de Little em (%lF s): %.20lF\n", tempo_coleta, e_n_calculo - (lambda * e_w_calculo));
                result = fprintf(arq,"%.20F\n", e_n_calculo - (lambda * e_w_calculo) );
                if (result == EOF)
                    printf("Erro na Gravacao\n");
                tempo_coleta = tempo_coleta + 10;

            }else{
                cout << "Evento invalido!" << endl;
                return (1);
            }
        }
        e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
        e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
        printf(" ---------- Simulação %d ----------\n", qtd_calculos);
        printf(" Tempo médio entre clientes: 0.5\n");
        printf(" Tempo médio de serviço: %lF\n", valores_media_servico[qtd_calculos-1]);
        printf(" Tempo de simulacao: 864000\n");
        printf(" Ocupacao: %lF\n", soma_ocupacao / tempo_decorrido);
        printf(" Tamanho Máximo da Fila: %ld\n", max_fila);

        e_n_calculo = e_n.soma_areas / tempo_decorrido;
        e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
        lambda = e_w_chegada.no_eventos / tempo_decorrido; 
        printf(" E[N]: %lF\n", e_n_calculo);
        printf(" E[W]: %lF\n", e_w_calculo);
        printf(" Erro de Little: %.20lF\n", e_n_calculo - (lambda * e_w_calculo));
        qtd_calculos++;
        fclose(arq);

    }


   

    return 0;
}