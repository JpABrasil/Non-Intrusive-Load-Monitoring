#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct { //Define as propriedades da nossa M quina de Estado
    float T_ON; //Tempo de Inicio do consumo da aplica  o
    float T_OFF; //Tempo de Fim do consumo da aplica  o
    float T_N; //Tempo real do embarcado
    float P_t; //Pot ncia Atual
    float P_t_1; //Pot ncia do  ltimo evento ON ou OFF
    float ST; //Tempo para esperar o power settling
    float TD; //Tempo de delay para eventos  dificeis
    int step; //Qual dos Steps Ser o acionados - Equivalente ao goto no artigo
} Estado;

Estado e = { 0,0,0,0,0,0,0,0 };

#define MAX_LINHA 1024

int main(int argc, char* argv[]) {
    //Argumentos
    char* caminho = argv[1];
    float TD = strtof(argv[2],NULL);
    float erro = strtof(argv[3],NULL);  


    //Consumindo arquivo de Entrada
    FILE* file;
    errno_t err;
    err = fopen_s(&file, caminho, "r");
    if (err != 0 || file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }
    char linha[MAX_LINHA];

    //Criando Arquivo de Saída
    char nome_saida[256];
    strcpy(nome_saida, caminho);
    char* ponto = strrchr(nome_saida, '.');
    if (ponto != NULL) {
        strcpy(ponto, "_eventos.csv");
    }
    else {
        strcat(nome_saida, "_eventos.csv");
    }
    FILE* saida;
    err = fopen_s(&saida, nome_saida, "w");
    if (err != 0 || saida == NULL) {
        perror("Erro ao criar arquivo de saída");
        fclose(saida);
        return 1;
    }
    fprintf(saida, "Evento,TimeStamp\n");



    // Loop Principal
    while (fgets(linha, sizeof(linha), file)) {
        // LENDO ARQUIVO - COME O
        char* contexto; // <- usado por strtok_s
        int coluna = 0;

        // Remove quebra de linha se existir
        linha[strcspn(linha, "\r\n")] = 0;

        //Lendo e Convertendo em Float
        char* col1 = strtok_s(linha, ",",&contexto);
        char* col2 = strtok_s(NULL, ",", &contexto);
        char* col3 = strtok_s(NULL, ",", &contexto);
        char* col4 = strtok_s(NULL, ",", &contexto);
        char* col5 = strtok_s(NULL, ",", &contexto);
        float tempo = strtof(col1, NULL);
        float pa = strtof(col2, NULL);
        float fp = strtof(col3, NULL);
        float fr = strtof(col4, NULL);
        float fnl = strtof(col5, NULL);

        
        //STATE MACHINE
        e.P_t = pa; // Ler do ESP
        e.T_N = tempo; //Ler do ESP
        float delta_P = e.P_t - e.P_t_1;
        float delta_P_p = delta_P / e.P_t;
        float delta_t;

        //Salvando String do Tempo para Prints
        char str[50];
        sprintf_s(str, sizeof(str), "%.2f", tempo);

        //Caso 1: Vari  o de Pot ncia > Erro -> Subida de Pot ncia acima do Erro2
        if (delta_P > erro) {
            switch (e.step) {
            case 0:
                e.T_ON = e.T_N;
                e.step = 1;
                break;
            case 1:
                break;
            case 2:
                e.step = 1;
            case 3:
                break;
            case 4:
                break;
            case 6:
                e.step = 1;
                break;
            case 7:
                break;
            }

        }

        else {
            //Caso 2 -> Varia  o de Potência < -Erro -> Descida de Potência acima do erro
            if (delta_P < -erro) {
                switch (e.step) {
                case 0:
                    e.step = 3;
                    break;
                case 1:
                    e.step = 5;
                    break;
                case 2:
                    break;
                case 3:
                    e.step = 5;
                    break;
                case 4:
                    e.step = 3;
                    break;
                case 5:
                    e.ST = e.T_N;
                    e.step = 6;
                    break;
                case 6:
                    delta_t = e.T_N - e.ST;              
                    if (delta_t > TD) {
                        printf("on4 - %s\n", str);      
                        fprintf(saida, "on4,%s\n", str);
                    }
                    e.step = 7;
                    break;
                case 7:
                    break;
                }
            }

            //Caso 3:  Erro > Varia  o de Pot ncia > - Erro  -> Pot ncia Constante 
            else if (delta_P > -erro) {
                switch (e.step) {
                case 0:
                    break;
                case 1:
                    e.T_OFF = e.T_N;
                    e.step = 2;
                    break;
                case 2:
                    //Trigger ON*
                    printf("on1 - %s\n", str);
                    fprintf(saida, "on1,%s\n", str);

                    e.step = 0;
                    break;
                case 3:
                    e.step = 4;
                    break;
                case 4:
                    //Trigger OFF
                    printf("off - %s\n", str);
                    fprintf(saida, "off,%s\n", str);

                    e.step = 0;
                    break;
                case 5:
                    //Trigger ON**
                    printf("on2 - %s\n", str);
                    fprintf(saida, "on2,%s\n", str);
                    e.step = 0;
                    break;
                case 6:
                    //Trigger ON***
                    printf("on3 - %s\n", str);
                    fprintf(saida, "on3,%s\n", str);
                    e.step = 0;
                    break;
                case 7: //Adaptado para pegar o OFF do Ventilador - Verificar se faz sentido
                    printf("off -%s\n", str);
                    fprintf(saida, "off,%s\n", str);
                    e.step = 0;
                    break;
                }
            }
        }

        e.P_t_1 = e.P_t; //Passar Pot ncia em t para Pot ncia em t-1
    }

    fclose(file);
    return 0;
}


