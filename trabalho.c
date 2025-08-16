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
tGalinha LeGalinha(FILE * config_inicial, tGalinha galinha);
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
    tDados dados;
    tGalinha galinha;
    tCarro carros[120];

} tJogo;
tJogo InicializarJogo(FILE *config_inicial, FILE *personagens);
tJogo LeArquivosJogo(FILE * config_inicial, FILE * personagens);
tJogo InicializarMapaBase(tJogo jogo); // vai ir dentro da função InicializarJogo
void InicializaMapaLocal(int linhas, int colunas,char mapa[linhas][colunas]);
void PreencherMapa(tJogo jogo, int linhas, int colunas, char mapa[linhas][colunas]);
//void PreencheMapa(int linhas, int colunas, char mapa[linhas][colunas]);
void ImprimeMapa(int pontos, int vidas, int iteracoes, int linhas, int colunas, char mapa[linhas][colunas]);



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

    jogo = InicializarJogo(config_inicial, personagens);

    /* while (!TerminarJogo()){

        ContinuarJogo();

    }

    GerarArquivoEstatistica();
    GerarArquivoResumo();
    GerarArquivoRanking();
    GerarArquivoHeatmap(); */

}



//############## LEITURA DOS ARQUIVOS ###############
tJogo LeArquivosJogo(FILE * config_inicial, FILE * personagens){
    
    tJogo jogo;

    tGalinha galinhaMatriz = LeMatrizGalinha(personagens);

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


    jogo.galinha = LeGalinha(config_inicial, galinhaMatriz);

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
tGalinha LeGalinha(FILE * config_inicial, tGalinha galinha){

    tGalinha galinhaL = galinha;
    galinhaL.pista = 0;

    char letra;

    fscanf(config_inicial, "%c %d %d", &letra, &galinhaL.posX, &galinhaL.vida);

    //printf("----------------------------\nGALINHA:\nVida: %d | posX: %d\n", galinhaL.vida, galinhaL.posX);

    return galinhaL;

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
tJogo InicializarJogo(FILE *config_inicial, FILE *personagens){
    
    tJogo jogo;

    
    jogo = LeArquivosJogo(config_inicial, personagens);
    
    jogo.dados.alturaMax = 0;
    jogo.dados.alturaMaxMorte = 0;
    jogo.dados.alturaMinMorte = 0;
    jogo.dados.iteracao = 0;
    jogo.dados.numMovimentos = 0;
    jogo.dados.numMovParaTras = 0;
    jogo.dados.pontos = 0;
    
    int linhas = 3 * jogo.dados.qtdPistas - 1;
    char mapaInicialL[linhas][jogo.dados.colunas];

    InicializaMapaLocal(linhas, jogo.dados.colunas, mapaInicialL);
    PreencherMapa(jogo, linhas, jogo.dados.colunas, mapaInicialL);
    ImprimeMapa(jogo.dados.pontos, jogo.galinha.vida, jogo.dados.iteracao, linhas, jogo.dados.colunas, mapaInicialL);
    
    //jogo = InicializarHeatmap(jogo);
    //GerarArquivoInicializacao();

    return jogo;

}
void PreencherMapa(tJogo jogo, int linhas, int colunas, char mapa[linhas][colunas]){
    
    int pistaAtual = 0;
    int idxCarro = 0;
    int i, j;
    
    
    for (i = 0; i < linhas; i++){
        
        if ((i+1) % 3 == 0) {
            pistaAtual++;
            continue;
        }

        for (j = 0; j < colunas; j++){
            

            if (i == linhas-2 && j+1 == jogo.galinha.posX) {
                
                mapa[i][j-1] = jogo.galinha.matriz[0][0];
                mapa[i][j] = jogo.galinha.matriz[0][1];
                mapa[i][j+1] = jogo.galinha.matriz[0][2];      // não estou fazendo a checagem do limite
                mapa[i+1][j-1] = jogo.galinha.matriz[1][0];
                mapa[i+1][j] = jogo.galinha.matriz[1][1];
                mapa[i+1][j+1] = jogo.galinha.matriz[1][2];

            
            }
            else if (pistaAtual == jogo.carros[idxCarro].pista && i % 3 == 0 && j+1 == jogo.carros[idxCarro].posX) {
           
                mapa[i][j-1] = jogo.carros[idxCarro].matriz[0][0];
                mapa[i][j] = jogo.carros[idxCarro].matriz[0][1];        // não estou fazendo a checagem do limite;
                mapa[i][j+1] = jogo.carros[idxCarro].matriz[0][2];
                mapa[i+1][j-1] = jogo.carros[idxCarro].matriz[1][0];
                mapa[i+1][j] = jogo.carros[idxCarro].matriz[1][1];
                mapa[i+1][j+1] = jogo.carros[idxCarro].matriz[1][2];

                idxCarro++;
                
            } 
        }
    }
    
}
void InicializaMapaLocal(int linhas, int colunas, char mapa[linhas][colunas]){
    
    int i, j;

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas; j++){
            if ((i+1) % 3 == 0 && (j+1) % 3 != 0) mapa[i][j] = '-';
            else mapa[i][j] = ' ';
            //printf("%c", jogoL.mapaBase[i][j]);
        }
        //printf("\n");
    }

}
void ImprimeMapa(int pontos, int vidas, int iteracoes, int linhas, int colunas, char mapa[linhas][colunas]) { 
    
    int i, j;

    printf("Pontos: %d | Vidas: %d | Iteracoes: %d\n", pontos, vidas, iteracoes);

    for (j = 0; j < colunas + 2; j++){
        if (j == 0 || j == colunas+1) printf("|");
        else printf("-");
    }
    printf("\n");

    for (i = 0; i < linhas; i++){
        for (j = 0; j < colunas + 2; j++){
            if (j == 0 || j == colunas+1) printf("|");
            else printf("%c", mapa[i][j-1]);
        }
        printf("\n");
    }

    // linha inferior
    for (j = 0; j < colunas + 2; j++) {
        if (j == 0 || j == colunas+1) 
            printf("|");
        else 
            printf("-");
    }
    printf("\n");

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