#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINHA 1024
#define MAX_COLUNAS 100

typedef struct { //Define as propriedades da nossa Máquina de Estado
    float T_ON; //Tempo de Inicio do consumo da aplicação
	float T_OFF; //Tempo de Fim do consumo da aplicação
	float T_N; //Tempo real do embarcado
    float P_t; //Potência Atual
	float P_t_1; //Potência do último evento ON ou OFF
    float ST; //Tempo para esperar o power settling
    float TD; //Tempo de delay para eventos ´dificeis
    int step; //Qual dos Steps Serão acionados - Equivalente ao goto no artigo
} Estado;

Estado e = {0,0,0,0,0,0,5000,0};

#define MAX_LINHA 1024

int main() {
    FILE* file;
    errno_t err;

    err = fopen_s(&file, "C:\\Users\\joaop\\Área de Trabalho\\Projetos Pessoais\\Desagregação Energia\\dados_esp32_agrupado_60s.csv", "r");

    if (err != 0 || file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    char linha[MAX_LINHA];

    // Loop Principal
    while (fgets(linha, sizeof(linha), file)) {
        // LENDO ARQUIVO - COMEÇO
        char* contexto; // <- usado por strtok_s
        int coluna = 0;

        // Remove quebra de linha se existir
        linha[strcspn(linha, "\r\n")] = 0;

        char* col1 = strtok_s(linha, ",", &contexto);
        char* col2 = strtok_s(NULL, ",", &contexto);
        char* col3 = strtok_s(NULL, ",", &contexto);
        char* col4 = strtok_s(NULL, ",", &contexto);
        char* col5 = strtok_s(NULL, ",", &contexto);

        float pa = strtof(col1, NULL);
        float fp = strtof(col2, NULL);
        float fr = strtof(col3, NULL);
        float fnl = strtof(col4, NULL);
        float tempo = strtof(col5, NULL);

        // LENDO ARQUIVO - FINAL
        //STATE MACHINE - COMEÇO
        float erro = 0.3;

        e.P_t = pa; // Ler do ESP
        e.T_N = tempo; //Ler do ESP
        float delta_P = e.P_t - e.P_t_1;
        float delta_P_p = delta_P / e.P_t;
        float delta_t;

        char str[50];
        sprintf_s(str, sizeof(str), "%.2f", tempo);

        //Caso 1: Varição de Potência > Erro -> Subida de Potência acima do Erro2
        if (delta_P_p > erro) {
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
            //Caso 2 -> Variação de Potência < -Erro -> Descida de Potência acima do erro
            if (delta_P_p < -erro) {
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
                    if (delta_t > e.TD) {
                        printf("on4 - %s\n", str);
                        e.step = 7;
                    }
                    break;
                case 7:
                    break;
                }
            }

            //Caso 3:  Erro > Variação de Potência > - Erro  -> Potência Constante 
            else if (delta_P_p > -erro) {
                switch (e.step) {
                case 0:
                    break;
                case 1:
                    e.T_OFF = e.T_N;
                    e.step = 2;
                    break;
                case 2:
                    //Trigger ON*
                    sprintf_s(str, sizeof(str), "%.2f", tempo);
                    printf("on1 - %s\n", str);
                    e.step = 0;
                    break;
                case 3:
                    e.step = 4;
                    break;
                case 4:
                    //Trigger OFF
                    printf("off - %s\n", str);
                    e.step = 0;
                    break;
                case 5:
                    //Trigger ON**
                    printf("on2 - %s\n", str);
                    e.step = 0;
                    break;
                case 6:
                    //Trigger ON***
                    printf("on3 - %s\n", str);
                    e.step = 0;
                    break;
                case 7:
                    e.step = 0;
                    break;
                }
            }
        }

        e.P_t_1 = e.P_t; //Passar Potência em t para Potência em t-1
    }

    fclose(file);
    return 0;
}

    
  