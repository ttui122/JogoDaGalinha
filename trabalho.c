#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Galinha
typedef struct{

    int vida;
    int posX;
    int posY; // prossivelmente tirar
    int pista;
    char matriz[2][3];

} tGalinha;

//Carros
typedef struct{

    int velocidade;
    int pista;
    int posX;
    int posY; // possivelmenete tirar
    int index; 
    char direcao;
    char matriz[2][3];

} tCarro;

//Jogo
typedef struct{

    int iteracao;
    int numMovimentos;
    int numMovParaTras;
    int alturaMaxMorte;
    int alturaMinMorte;
    int alturaMax;
    int qtdPistas;
    int colunas;
    int animacao;
    
} tDados;

int main(int argc, char * argv[1001]){
    
    if (argc <= 1) {
        printf("ERRO: Informe o diretorio com os arquivos de configuracao.");
        exit(1);
    }

    tGalinha galinha;
    tCarro carros[120];
    tCarro carroBase;
    tDados dados;

    char lixo;                  // talvez tenha que inicializar dentro do else

    int pistaAtual = 0;
    int qtdCarros;
    int idxVetor = 0;
    int i, j;
    char letra;
    //int qtdCarrosTotal = 0; // para depurar, remover depois
    
    FILE * config_inicial;
    FILE * personagens;
    
    config_inicial = fopen("config_inicial.txt", "r");   // não sei como informar o diretorio

    if (!config_inicial){

        printf("Erro ao abrir o arquivo ""config_inicial.txt"" no diretorio: %s", argv[1]);
        return 1;

    }
    else {

        fscanf(config_inicial, "%d\n", &dados.animacao); // primeira linha
        fscanf(config_inicial, "%d %d", &dados.colunas, &dados.qtdPistas); // segunda linha

        /* printf("Dados da Animação: %d\n", dados.animacao);
        printf("COLUNAS: %d | PISTAS: %d\n", dados.colunas, dados.qtdPistas); */
        
        fscanf(config_inicial, "%c", &lixo);


        while (pistaAtual < dados.qtdPistas-1){

            fscanf(config_inicial, "%c", &letra); 

            if (letra == '\n') { // linha de chegada e pistas vazias
                //printf("Linha Vazia: %c | Pista: %d\n", letra, pistaAtual);
                pistaAtual++;
            }
            else {
                carroBase.direcao = letra;

                fscanf(config_inicial, " %d %d", &carroBase.velocidade, &qtdCarros); // escaneia cada linha, gerando um vetor de carros, com os primeiros elementos sendo
                for (i = 0; i < qtdCarros; i++){                                     // os carros da primeira pista
                    carros[idxVetor] = carroBase;
                    carros[idxVetor].pista = pistaAtual;
                    carros[idxVetor].index = i;
                    fscanf(config_inicial, " %d", &carros[idxVetor].posX);   
                    idxVetor++; 
                    //qtdCarrosTotal++;
                }

                fscanf(config_inicial, "%c", &lixo);
                pistaAtual++;

            }
            

        }

        fscanf(config_inicial, "%c %d %d", &letra, &galinha.posX, &galinha.vida); // linha da galinha

        /* printf("Letra galinha: %c\n", letra);

        printf("##############\nGALINHA:\nVida: %d | posX: %d\nCARROS:\n", galinha.vida, galinha.posX);
        for (i = 0; i < qtdCarrosTotal; i++){
            printf("velocidade: %d | posX: %d | pista: %d | direcao: %c | index: %d\n", carros[i].velocidade, carros[i].posX, carros[i].pista, carros[i].direcao, carros[i].index);
        }
        printf("Pista Atual: %d\n", pistaAtual);
        printf ("%d", idxVetor); */

    }    
    
    fclose(config_inicial);
    
    personagens = fopen("personagens.txt", "r");    // não sei como informar o diretorio
    if (!personagens){

        printf("Erro ao abrir o arquivo ""personagens.txt"" no diretorio: %s", argv[1]);
        return 1;
        
    }
    else {
        
        for (i = 0; i < 2; i++){
            for (j = 0; j < 3; j++){
                fscanf(personagens, "%c", &galinha.matriz[i][j]);
                //printf("%c", galinha.matriz[i][j]);
            }
            fscanf(personagens, "%c", &lixo); // \n
            //printf("%c", lixo);
        }

        printf("\n");

        int k;

        if (dados.animacao == 0) {
            for (i = 0; i < 2; i++){
                for (j = 0; j < 3; j++){
                        fscanf(personagens, "%c", &carroBase.matriz[i][j]);
                        //printf("%c", carroBase.matriz[i][j]);
                }
                fscanf(personagens, "%c", &lixo); // \n
                //printf("%c", lixo);
            }

            

            for (k = 0; k < 6; k++){
                for (i = 0; i < 2; i++){
                    for (j = 0; j < 3; j++){
                        carros[k].matriz[i][j] = carroBase.matriz[i][j];
                    }
                }
            }

        }
        else {
            printf("\n###Fazer animaçao###\n");
        }
    }
    
    fclose(personagens);

    return 0;
    
}