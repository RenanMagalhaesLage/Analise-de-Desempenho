#ifndef T2_H
#define T2_H
using namespace std;

enum Tipo {
    chegada_pacote,
    saida_pacote,
    nova_conex, //acho dispensável já que a cada nova conexão irei supor que chegou um pacote
    temp_coleta

};

// -------------- Structs -----------------------
typedef struct{
    double tempo_simulacao;
    double ocupacao;
    double media_chegada;
    double media_servico;
} parametros;

typedef struct {
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;
} little;

typedef struct{
    //enum, short... tipo_evento
    Tipo tipo_evento;
    double tempo;// tempo
    double tempo_duracao;
    //tempo_chegada --> tempo de chegada de cada pacote --> incrementado a cada 20 milisegundos
    //tempo_duracao --> exponencial de 120s
    // ???
} ligacao;

// Funcao para comparar o menor tempo de chegada da struct de ligacao
struct compare{
    bool operator()(const ligacao& lig1, const ligacao& lig2){
        return lig1.tempo > lig2.tempo;
    }
};

// ------------ Cabecalhos ----------------------
void inicia_little(little * l);
void le_parametros(parametros * params);
double uniforme();
double min(double d1, double d2);

#endif