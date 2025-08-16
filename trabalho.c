#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Galinha
typedef struct{

    int vida;
    int posX;
    int pista;
    char matriz[2][3];

} tGalinha;
tGalinha LeGalinha(FILE * config_inicial);
tGalinha LeMatrizGalinha(FILE * personagens);

//Carros
typedef struct{

    int velocidade;
    int pista;
    int posX;
    int index; 
    char direcao;
    char matriz[2][3];

} tCarro;
tCarro LeCarro(FILE * config_inicial, int pistaAtual, tCarro carroBase, int index);
tCarro LeMatrizCarro(FILE * personagens, int animacao);

//Dados
typedef struct{

    int pontos;
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
tDados LeDados(FILE * config_inicial);

//Jogo
typedef struct{

    int heatmap[35][100];
    char mapaBase[35][100];
    tDados dados;
    tGalinha galinha;
    tCarro carros[120];

} tJogo;
void InicializarMapaBase(int linhas, int colunas, char mapa[linhas][colunas]); // vai ir dentro da função InicializarJogo
//void PreencheMapa(int linhas, int colunas, char mapa[linhas][colunas]);
void ImprimeMapa(int pontos, int vidas, int iteracoes, int linhas, int colunas, char mapa[linhas][colunas]);
void AtualizaTela(tJogo jogo, int linhas, int colunas, char mapaBase[linhas][colunas]);
tJogo LeArquivosJogo(FILE *config_inicial, FILE *personagens);

//Validacao dos Arquivos
void geraCaminhoCompletoArquivos(char * diretorio, char * arquivo, char * caminho);
void AbreArquivos(char * argv, FILE **config_inicial, FILE **personagens);
void ValidaArquivos(int argc, char * argv, FILE *config_inicial, FILE *personagens);

int main(int argc, char * argv[]){
    tJogo jogo;

    FILE * config_inicial;
    FILE * personagens;

    if (argc == 1) {
        printf("ERRO: Informe o diretorio com os arquivos de configuracao.");
        exit(1);
    }
    AbreArquivos(argv[1], &config_inicial, &personagens);

    ValidaArquivos(argc, argv[1], config_inicial, personagens);

    jogo = LeArquivosJogo(config_inicial, personagens);

    //InicializarMapaBase(14, 45, jogo.mapaBase);
    //ImprimeMapa(4, 3, 10, 14, 45, jogo.mapaBase);  
      
}

//############## LEITURA DOS ARQUIVOS ###############
tJogo LeArquivosJogo(FILE *config_inicial, FILE *personagens){
    
    tJogo jogo;

    jogo.galinha = LeMatrizGalinha(personagens);

    jogo.dados = LeDados(config_inicial);
    
    
    int pistaAtual = 0, idxVetor = 0, qtdCarros, i, j;
    char direcao, lixo;
    tCarro carroBase;

    carroBase = LeMatrizCarro(personagens, jogo.dados.animacao);

    while (pistaAtual < jogo.dados.qtdPistas-1){

            fscanf(config_inicial, "%c", &direcao); 

            if (direcao == '\n') pistaAtual++;     // linha de chegada e pistas vazias
            else {

                carroBase.direcao = direcao;

                fscanf(config_inicial, " %d %d", &carroBase.velocidade, &qtdCarros); // escaneia cada linha, gerando um vetor de carros, com os primeiros elementos sendo os carros da primeira pista   
                
                for (i = 0; i < qtdCarros; i++){          
                    jogo.carros[idxVetor] = LeCarro(config_inicial, pistaAtual, carroBase, i);                          
                    idxVetor++; 
                }

                fscanf(config_inicial, "%c", &lixo);
               
                pistaAtual++;

            }
        }

        /* printf("DESENHO CARRO\n");
        for (i = 0; i < 2; i++){
            for (j = 0; j < 3; j++){
                printf("%c", jogo.carros[0].matriz[i][j]);
            }
            printf("\n");
        } */


    jogo.galinha = LeGalinha(config_inicial);

    fclose(config_inicial);
    fclose(personagens);
    
    return jogo;

}
tDados LeDados(FILE * config_inicial){
    
    tDados dados;
    char lixo;

    fscanf(config_inicial, "%d\n", &dados.animacao); // primeira linha
    fscanf(config_inicial, "%d %d", &dados.colunas, &dados.qtdPistas); // segunda linha
    fscanf(config_inicial, "%c", &lixo);

    /* printf("Dados da Animação: %d\n", dados.animacao);
    printf("COLUNAS: %d | PISTAS: %d\n----------------------------\n", dados.colunas, dados.qtdPistas); */

    return dados;

}
tGalinha LeGalinha(FILE * config_inicial){

    tGalinha galinha;
    char letra;

    fscanf(config_inicial, "%c %d %d", &letra, &galinha.posX, &galinha.vida);

    //printf("----------------------------\nGALINHA:\nVida: %d | posX: %d\n", galinha.vida, galinha.posX);

    return galinha;

}
tGalinha LeMatrizGalinha(FILE * personagens){

    tGalinha galinha;

    int i, j;
    char lixo;

    //printf("DESENHO GALINHA\n");
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++)
        {
            fscanf(personagens, "%c", &galinha.matriz[i][j]);
            //printf("%c", galinha.matriz[i][j]);
        }
        fscanf(personagens, "%c", &lixo); // \n
        //printf("%c", lixo);
    }

    return galinha;

}
tCarro LeCarro(FILE * config_inicial, int pistaAtual, tCarro carroBase, int index){
    
    tCarro carro;

    carro = carroBase;
    carro.pista = pistaAtual;
    carro.index = index;
    fscanf(config_inicial, " %d", &carro.posX);

    //printf("velocidade: %d | posX: %d | pista: %d | direcao: %c | index: %d\n", carro.velocidade, carro.posX, carro.pista, carro.direcao, carro.index);

    return carro;

}
tCarro LeMatrizCarro(FILE * personagens, int animacao){

    tCarro carro;

    int i, j;
    char lixo;

    if (animacao == 0) {
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 3; j++) {
                fscanf(personagens, "%c", &carro.matriz[i][j]);
                //printf("%c", carro.matriz[i][j]);
            }
            fscanf(personagens, "%c", &lixo); // \n
            //printf("%c", lixo);
        }
    }
    else {

        printf ("### FAZER ANIMACAO ###\n");
    }

    return carro;

}

//############## GERENCIAMENTO DO MAPA ##############
void AtualizaTela(tJogo jogo, int linhas, int colunas, char mapaBase[linhas][colunas]){
    char mapaLocal[linhas][colunas];

    //PreencheMapa(linhas, colunas, mapaLocal);
    ImprimeMapa(jogo.dados.pontos, jogo.galinha.vida, jogo.dados.iteracao, linhas, colunas, mapaBase);

}
void InicializarMapaBase(int linhas, int colunas, char mapa[linhas][colunas]){
    int i, j;

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas; j++){
            if ((i+1) % 3 == 0 && (j+1) % 3 != 0) mapa[i][j] = '-';
            else mapa[i][j] = ' ';
        }
    }
}
void ImprimeMapa(int pontos, int vidas, int iteracoes, int linhas, int colunas, char mapa[linhas][colunas]){
    int i, j;

    printf("Pontos: %d | Vidas: %d | Iteracoes: %d\n", pontos, vidas, iteracoes);

    for (j = 0; j < colunas+2; j++){
        if (j == 0 || j == colunas+1) printf("|");
        else printf("-");
    }

    printf("\n");

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas+2; j++){
            if (j == 0 || j == colunas+1) printf("|");
            else printf("%c", mapa[i][j-1]);
        }
        printf("\n");
    }

    for (j = 0; j < colunas+2; j++){
        if (j == 0 || j == colunas+1) printf("|");
        else printf("-");
    }
}

//############## VALIDACAO DE ARQUIVOS ##############
void AbreArquivos(char * argv, FILE **config_inicial, FILE **personagens){
    char caminhoConfig[1100];
    char caminhoPersonagens[1100];
    
    char arquivoConfig[] = "/config_inicial.txt";
    char arquivoPersonagens[] = "/personagens.txt";

    geraCaminhoCompletoArquivos(argv, arquivoConfig, caminhoConfig);
    geraCaminhoCompletoArquivos(argv, arquivoPersonagens, caminhoPersonagens);

    *config_inicial = fopen(caminhoConfig, "r");
    *personagens = fopen(caminhoPersonagens, "r");

}
void geraCaminhoCompletoArquivos(char * diretorio, char * arquivo, char * caminho){

    strcpy(caminho, diretorio);
    strcat(caminho, arquivo);

}
void ValidaArquivos(int argc, char * argv, FILE *config_inicial, FILE *personagens){    
    int comprimento = strlen(argv);
    
    if (comprimento > 1000) {
        printf("ERRO: Diretorio inválido (mais de 1000 caracteres)");
    }

    if (!config_inicial){

        printf("Erro ao abrir o arquivo ""config_inicial.txt"" no diretorio: %s", argv);
        exit(1);

    }
    
    if (!personagens){

        printf("Erro ao abrir o arquivo ""personagens.txt"" no diretorio: %s", argv);
        exit(1);
        
    }
}