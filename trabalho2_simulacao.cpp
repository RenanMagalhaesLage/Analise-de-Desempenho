#include <iostream>
#include <cmath>
#include <cfloat>
#include <iomanip>
#include <queue>
#include <vector>
#include <cstdio>
#include "t2.hpp"

// -------------- Funcoes Auxiliares --------------
/*
    Inicia parametros para a lei de Little
    @param: struct de parametros para little
*/
void inicia_little(little * l){
    l->no_eventos = 0;
    l->tempo_anterior = 0.0;
    l->soma_areas = 0.0;
}

/*
    Le parametros para as variaveis de embasamento da simulacao
    @param: struct de parametros
*/
void le_parametros(parametros * params){
    params->tempo_simulacao = 259200.0;

    cout << "Informe a ocupacao desejada (%): " << endl;
    cin >> params->ocupacao;
    params->ocupacao /= 100;
}

/*
    Calcula o uniforme
    @return: uniforme
*/
double uniforme() {
	double u = rand() / ((double) RAND_MAX + 1);
	// Limitando entre (0,1]
	u = 1.0 - u;

	return (u);
}

/*
    Funcao auxiliar para achar o minimo
    @param: duas variaveis para comparacao
    @return: o menor numero
*/
double min(double d1, double d2){
    if(d1 < d2) return d1;
    return d2;
}

int main(){

    // Inicializa o gerador de numeros aleatorios com uma semente baseada no tempo
    int semente = time(NULL);
    srand(semente);

    // Le valores parametrizados
    parametros params;
    le_parametros(&params);

    // Variaveis da base da simulação
    double tamanho_pacote = 0.000000188;
    double largura_da_banda = 0.0000752;
    double tempo_exponencial = 120.0;

    // Variaveis de controle da evento_atual e ocupacao
    double tempo_servico = tamanho_pacote/(1.0/8.0); // Tempo de Servico: tam pct/ largura de banda
    double total_clientes = params.ocupacao * 1.0 / largura_da_banda;
    double taxa_de_chegada = total_clientes / tempo_exponencial;
    params.media_chegada = (1.0 / taxa_de_chegada);
    params.media_chegada = (1.0 / params.media_chegada);

    // Variaveis de controle da simulacao
    //double tempo_chegada = (-1.0/params.media_chegada) * log(uniforme());
    double tempo_decorrido = 0.0;
    // double tempo_saida = DBL_MAX;

    // Variaveis de controle da fila
    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

    // Variaveis de medidas de interesse
    double soma_ocupacao = 0.0;
    little e_n;
    little e_w_chegada;
    little e_w_saida;

    // Inicia littles
    inicia_little(&e_n);
    inicia_little(&e_w_chegada);
    inicia_little(&e_w_saida);

    // Variaveis de calculo
    double e_n_calculo;
    double e_w_calculo;
    double lambda;

    // Inicia arvore heap
    ligacao evento_atual, inserir;
    priority_queue <ligacao, vector<ligacao>, compare> min_heap;

    // Adicionando tempo de coleta na heap
    inserir.tempo = 10.0;
    inserir.tipo_evento = temp_coleta;
    inserir.tempo_duracao = 0.0;
    min_heap.push(inserir);

    // Adicionando nova conexao
    inserir.tempo = (-1.0/(params.media_chegada)) * log(uniforme());
    inserir.tipo_evento = nova_conex;
    inserir.tempo_duracao = 0.0;
    min_heap.push(inserir);

    // Configurando arquivo para escrita dos resultados
    FILE *en, *ew, *little, *oc;
    en = fopen("en_results.txt", "w");
    ew = fopen("ew_results.txt", "w");
    little = fopen("little_results.txt", "w");
    oc = fopen("ocupacao_results.txt", "w");
    if ((en == NULL) && (ew == NULL) && (little == NULL)) {
        perror("Erro ao abrir o arquivo.");
        return 1;
    }

    while(tempo_decorrido < params.tempo_simulacao){
        // Pegando o menor no da min heap
        evento_atual = min_heap.top();
        min_heap.pop();

        // Atualizando o tempo_decorrido com o menor tempo
        tempo_decorrido = evento_atual.tempo; //aqui n tem que ficar somando n ?

        // Evento 1: chega nova evento_atual
        if(evento_atual.tipo_evento == nova_conex){
            //e_n.no_eventos++;

            // Gera nova conexao com o novo tempo de quando será a proxima
            evento_atual.tempo += (-1.0/params.media_chegada) * log(uniforme());
            evento_atual.tempo_duracao = 0.0;
            min_heap.push(evento_atual);

            // Evento inicia ja gerando pacote
            inserir.tempo = tempo_decorrido;
            inserir.tempo_duracao = tempo_decorrido + (-1.0/(1.0/tempo_exponencial)) * log(uniforme());
            inserir.tipo_evento = chegada_pacote;
            min_heap.push(inserir);

        // Evento 2: chegada de novo pacote
        }else if(evento_atual.tipo_evento == chegada_pacote){

            // Atualiza o tempo da evento_atual e verifica se ele nao ultrapassou do tempo de duracao
            evento_atual.tempo = evento_atual.tempo + 0.020;
            if(evento_atual.tempo <= evento_atual.tempo_duracao){
                min_heap.push(evento_atual);
            }
            // Fila ta vazia entao temos que tratar o atendimento do pacote que chegou
            if(!fila){

                // Adicionando tempo de servico(saida do pacote) na heap
                inserir.tempo = tempo_servico + tempo_decorrido;
                inserir.tipo_evento = saida_pacote;
                inserir.tempo_duracao = 0.0;
                min_heap.push(inserir);

                // Vai somar tempo de duracao ou tempo?
                soma_ocupacao += tempo_servico ;
            }

            // Atualizando fila
            fila++;
            max_fila = fila > max_fila? fila:max_fila;


            // Calculo de Little -- E[N]
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos++;
            e_n.tempo_anterior = tempo_decorrido;

            // Calculo de Little -- E[W] -- Chegada
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_chegada.no_eventos++;
            e_w_chegada.tempo_anterior = tempo_decorrido;

        // Evento 3: saida de novo pacote
        }else if(evento_atual.tipo_evento == saida_pacote){
            fila--;
            if (fila)
            {
                evento_atual.tempo = tempo_decorrido + tempo_servico;
                min_heap.push(evento_atual);

                soma_ocupacao += tempo_servico;
            }

            // Calculo de Little -- E[N]
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos--;
            e_n.tempo_anterior = tempo_decorrido;

            // Calculo de Little -- E[W] -- Saida
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
            e_w_saida.no_eventos++;
            e_w_saida.tempo_anterior = tempo_decorrido;

        // Evento 4: Coleta de Little
        }else if(evento_atual.tipo_evento == temp_coleta){

            // Calculando o Erro de little a cada 10s
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

            e_n.tempo_anterior = tempo_decorrido;
            e_w_chegada.tempo_anterior = tempo_decorrido;
            e_w_saida.tempo_anterior = tempo_decorrido;

            e_n_calculo = e_n.soma_areas / tempo_decorrido;
            e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
            lambda = e_w_chegada.no_eventos / tempo_decorrido;

            // Escrevendo em en
            fprintf(en, "E[N]: %.10lf\n\n", e_n_calculo);

            // Escrevendo em ew
            fprintf(ew, "E[W]: %.10lf\n\n", e_w_calculo);

            // Escrevendo em little
            fprintf(little, "Erro de Little: %.10lf\n\n", e_n_calculo - lambda * e_w_calculo);

            // Escrevendo ocupação
            fprintf(oc, "Ocupação: %.10lf\n\n", soma_ocupacao/tempo_decorrido);
            // Escrevendo ocupação
            fprintf(oc, "maxfila: %ld\n\n", max_fila);

            evento_atual.tempo = evento_atual.tempo + 10.0;
            min_heap.push(evento_atual);
        }else{
            printf("Evento invalido!\n");
            return(1);
        }
    }

    e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
    e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

    e_n_calculo = e_n.soma_areas / tempo_decorrido;
    e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
    lambda = e_w_chegada.no_eventos / tempo_decorrido;

    // Escrevendo em en
    fprintf(en, "E[N]: %.10lf\n\n", e_n_calculo);

    // Escrevendo em ew
    fprintf(ew, "E[W]: %.10lf\n\n", e_w_calculo);

    // Escrevendo em little
    fprintf(little, "Erro de Little: %.10lf\n\n\n", e_n_calculo - lambda * e_w_calculo);

    // Fechando arquivos
    fclose(en);
    fclose(ew);
    fclose(little);

    printf("ocupacao: %lF\n",soma_ocupacao/tempo_decorrido);
    printf("tamanho da fila: %ld\n", max_fila);
    printf("E[N]: %lF\n", e_n_calculo);
    printf("E[W]: %lF\n", e_w_calculo);
    printf("Erro de Little: %lF\n", e_n_calculo - lambda * e_w_calculo);
}